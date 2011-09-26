#include "stream.h"
#include <cell/audio.h>
#include <cell/sysmodule.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"

#define samples_times_two 512

static inline void s16_to_float(float * restrict out, const s16 * restrict in, u32 samples)
{
   for (u32 i = 0; i < samples; i++)
      out[i] = (float)in[i]/0x8000;
}

static void init_audioport(void)
{
   static int init_count = 0;
   if (init_count == 0)
   {
      cellSysmoduleLoadModule(CELL_SYSMODULE_AUDIO);
      cellAudioInit();
      init_count++;
   }
}

typedef struct audioport
{
   volatile u64 quit_thread;
   u32 audio_port;

   u32 channels;

   pthread_mutex_t lock;
   pthread_mutex_t cond_lock;
   pthread_cond_t cond;
   pthread_t thread;

   fifo_buffer_t *buffer;

   cell_audio_sample_cb_t sample_cb;
   void *userdata;

   u32 is_paused;
} audioport_t;

static void* event_loop(void *data)
{
   audioport_t *port = data;

   sys_event_queue_t id;
   sys_ipc_key_t key;

   cellAudioCreateNotifyEventQueue(&id, &key);
   cellAudioSetNotifyEventQueue(key);

   //pull_event_loop - BEGIN
   sys_event_t event;

   s16 *in_buf = memalign(128, samples_times_two * sizeof(s16));
   float *conv_buf = memalign(128, samples_times_two * sizeof(float));
   do
   {
      u32 has_read = 0;
      if (port->sample_cb)
         has_read = port->sample_cb(in_buf, samples_times_two, port->userdata);
      else
      {
         has_read = samples_times_two;
         pthread_mutex_lock(&port->lock);
         u32 avail = fifo_read_avail(port->buffer);
         if (avail < samples_times_two * sizeof(s16))
            has_read = avail / sizeof(s16);

         fifo_read(port->buffer, in_buf, has_read * sizeof(s16));
         pthread_mutex_unlock(&port->lock);
      }

      if (has_read < samples_times_two)
         memset(in_buf + has_read, 0, (samples_times_two - has_read) * sizeof(s16));

      for (u32 i = 0; i < samples_times_two; i++)
         conv_buf[i] = (float)in_buf[i]/0x8000;

      sys_event_queue_receive(id, &event, SYS_NO_TIMEOUT);
      cellAudioAddData(port->audio_port, conv_buf, CELL_AUDIO_BLOCK_SAMPLES, 1.0);

      pthread_cond_signal(&port->cond);
   }while(!port->quit_thread);
   free(conv_buf);
   //pull_event_loop - END

   cellAudioRemoveNotifyEventQueue(key);
   pthread_exit(NULL);
   return NULL;
}

static cell_audio_handle_t audioport_init(const struct cell_audio_params *params)
{
   init_audioport();

   audioport_t *handle = calloc(1, sizeof(*handle));

   CellAudioPortParam port_params = {
      .nChannel = params->channels,
      .nBlock = 8,
      .attr = 0
   };

   handle->channels = params->channels;

   handle->sample_cb = params->sample_cb;
   handle->userdata = params->userdata;
   handle->buffer = fifo_new(params->buffer_size ? params->buffer_size : 4096);

   pthread_mutex_init(&handle->lock, NULL);
   pthread_mutex_init(&handle->cond_lock, NULL);
   pthread_cond_init(&handle->cond, NULL);

   cellAudioPortOpen(&port_params, &handle->audio_port);
   cellAudioPortStart(handle->audio_port);

   pthread_create(&handle->thread, NULL, event_loop, handle);
   return handle;
}

static void audioport_pause(cell_audio_handle_t handle)
{
   audioport_t *port = handle;
   port->is_paused = 1;
   cellAudioPortStop(port->audio_port);
}

static s32 audioport_unpause(cell_audio_handle_t handle)
{
   audioport_t *port = handle;
   port->is_paused = 0;
   cellAudioPortStart(port->audio_port);
   return 0;
}

static u32 audioport_is_paused(cell_audio_handle_t handle)
{
   audioport_t *port = handle;
   return port->is_paused;
}

static void audioport_free(cell_audio_handle_t handle)
{
   audioport_t *port = handle;

   port->quit_thread = 1;
   pthread_join(port->thread, NULL);

   pthread_mutex_destroy(&port->lock);
   pthread_mutex_destroy(&port->cond_lock);
   pthread_cond_destroy(&port->cond);

   if (port->buffer)
      fifo_free(port->buffer);

   cellAudioPortStop(port->audio_port);
   cellAudioPortClose(port->audio_port);

   free(port);
}

static u32 audioport_write_avail(cell_audio_handle_t handle)
{
   audioport_t *port = handle;

   pthread_mutex_lock(&port->lock);
   u32 ret = fifo_write_avail(port->buffer);
   pthread_mutex_unlock(&port->lock);
   return ret / sizeof(s16);
}

static s32 audioport_write(cell_audio_handle_t handle, const s16 *data, u32 samples)
{
   s32 ret = samples;
   u32 bytes = samples * sizeof(s16);

   audioport_t *port = handle;
   do
   {
      pthread_mutex_lock(&port->lock);
      u32 avail = fifo_write_avail(port->buffer);
      pthread_mutex_unlock(&port->lock);

      u32 to_write = avail < bytes ? avail : bytes;
      if (to_write > 0)
      {
         pthread_mutex_lock(&port->lock);
         fifo_write(port->buffer, data, to_write);
         pthread_mutex_unlock(&port->lock);
         bytes -= to_write;
         data += to_write >> 1;
      }
      else
      {
         pthread_mutex_lock(&port->cond_lock);
         pthread_cond_wait(&port->cond, &port->cond_lock);
         pthread_mutex_unlock(&port->cond_lock);
      }
   }while(bytes);

   return ret;
}

const cell_audio_driver_t cell_audio_audioport = {
   .init = audioport_init,
   .write = audioport_write,
   .write_avail = audioport_write_avail,
   .pause = audioport_pause,
   .unpause = audioport_unpause,
   .is_paused = audioport_is_paused,
   .free = audioport_free
};
