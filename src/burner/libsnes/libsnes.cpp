#include "libsnes.hpp"

#include <stdio.h>

static snes_video_refresh_t video_cb = NULL;
static snes_audio_sample_t audio_cb = NULL;
static snes_input_poll_t poll_cb = NULL;
static snes_input_state_t input_cb = NULL;

unsigned snes_library_revision_major(void)
{
   return 1;
}

unsigned snes_library_revision_minor(void)
{
   return 0;
}

const char *snes_library_id(void)
{
   return "FBANext";
}

void snes_set_video_refresh(snes_video_refresh_t cb)
{
   video_cb = cb;
}

void snes_set_audio_sample(snes_audio_sample_t cb)
{
   audio_cb = cb;
}

void snes_set_input_poll(snes_input_poll_t cb)
{
   poll_cb = cb;
}

void snes_set_input_state(snes_input_state_t cb)
{
   input_cb = cb;
}

void snes_set_controller_port_device(bool, unsigned)
{}

void snes_set_cartridge_basename(const char*)
{}

static uint8_t *state_buf = NULL;

void snes_init(void)
{
   state_buf = new uint8_t[2000000];
}

static unsigned serialize_size = 0;

static void gba_init(void)
{
   cpuSaveType = 0;
   flashSize = 0x10000;
   enableRtc = false;
   mirroringEnable = false;

   utilUpdateSystemColorMaps();

   CPUInit(0, false);
   CPUReset();

   soundInit();
   soundSetSampleRate(31900);
   soundReset();
   soundResume();

   serialize_size = CPUWriteState_libgba(state_buf, 2000000);
}

void snes_term(void)
{
   delete[] state_buf;
}

void snes_power(void)
{}

void snes_reset(void)
{}

void snes_run(void)
{
}


unsigned snes_serialize_size(void)
{
   return serialize_size;
}

bool snes_serialize(uint8_t *data, unsigned size)
{
   //return CPUWriteState_libgba(data, size);
}

bool snes_unserialize(const uint8_t *data, unsigned size)
{
   //return CPUReadState_libgba(data, size);
}

void snes_cheat_reset(void)
{}

void snes_cheat_set(unsigned, bool, const char*)
{}

bool snes_load_cartridge_normal(const char*, const uint8_t *rom_data, unsigned rom_size)
{
   const char *tmppath = tmpnam(NULL);
   if (!tmppath)
      return false;

   FILE *file = fopen(tmppath, "wb");
   if (!file)
      return false;

   fwrite(rom_data, 1, rom_size, file);
   fclose(file);
   //unsigned ret = CPULoadRom(tmppath);
   //unlink(tmppath);

   //gba_init();

   return ret;
}

bool snes_load_cartridge_bsx_slotted(
  const char*, const uint8_t*, unsigned,
  const char*, const uint8_t*, unsigned
)
{ return false; }

bool snes_load_cartridge_bsx(
  const char*, const uint8_t *, unsigned,
  const char*, const uint8_t *, unsigned
)
{ return false; }

bool snes_load_cartridge_sufami_turbo(
  const char*, const uint8_t*, unsigned,
  const char*, const uint8_t*, unsigned,
  const char*, const uint8_t*, unsigned
)
{ return false; }

bool snes_load_cartridge_super_game_boy(
  const char*, const uint8_t*, unsigned,
  const char*, const uint8_t*, unsigned
)
{ return false; }

void snes_unload_cartridge(void)
{}

bool snes_get_region(void)
{
   return SNES_REGION_NTSC;
}

uint8_t *snes_get_memory_data(unsigned id)
{
   if (id != SNES_MEMORY_CARTRIDGE_RAM)
      return 0;
   return flashSaveMemory;
}

unsigned snes_get_memory_size(unsigned id)
{
   if (id != SNES_MEMORY_CARTRIDGE_RAM)
      return 0;

   return 0x10000;
}

#if 0
void systemOnWriteDataToSoundBuffer(uint16_t *finalWave, int length)
{
   for (int i = 0; i < length; i += 2)
      audio_cb(finalWave[i + 0], finalWave[i + 1]);
}
#endif

static uint16_t pix_buf[160 * 1024];

#if 0
void systemDrawScreen()
{
   for (unsigned y = 0; y < 160; y++)
   {
      uint16_t *dst = pix_buf + y * 1024;
      const uint32_t *src = (const uint32_t*)pix + 241 * y; // Don't ask why ... :(
      for (unsigned x = 0; x < 240; x++)
         dst[x] = (uint16_t)(src[x] & 0x7fff);
   }

   video_cb(pix_buf, 240, 160);
}
#endif

// Stubs

#if 0
u32 systemReadJoypad(int)
{
   poll_cb();

   u32 J = 0;

   static const unsigned binds[] = {
      SNES_DEVICE_ID_JOYPAD_A,
      SNES_DEVICE_ID_JOYPAD_B,
      SNES_DEVICE_ID_JOYPAD_SELECT,
      SNES_DEVICE_ID_JOYPAD_START,
      SNES_DEVICE_ID_JOYPAD_RIGHT,
      SNES_DEVICE_ID_JOYPAD_LEFT,
      SNES_DEVICE_ID_JOYPAD_UP,
      SNES_DEVICE_ID_JOYPAD_DOWN,
      SNES_DEVICE_ID_JOYPAD_R,
      SNES_DEVICE_ID_JOYPAD_L
   };

   for (unsigned i = 0; i < 10; i++)
      J |= input_cb(SNES_PORT_1, SNES_DEVICE_JOYPAD, 0, binds[i]) << i;

   return J;
}
#endif
