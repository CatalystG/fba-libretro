#ifndef INTERFACE_H
#define INTERFACE_H

#ifdef FBA_DEBUG
 #define PRINT_DEBUG_INFO
#endif

// GameInp structure
#include "gameinp.h"

// Interface info (used for all modules)
struct InterfaceInfo
{
	const char * pszModuleName;
	char ** ppszInterfaceSettings;
	char ** ppszModuleSettings;

	// device info, added by regret
	unsigned int deviceNum;
	char ** deviceName;
};

int IntInfoFree(InterfaceInfo* pInfo);
int IntInfoInit(InterfaceInfo* pInfo);
int IntInfoAddStringInterface(InterfaceInfo* pInfo, char * szString);
int IntInfoAddStringModule(InterfaceInfo* pInfo, char * szString);

int InputInit();
int InputExit();
void InputMake(void);

extern bool bInputOkay;

// Audio Output plugin
int AudWriteSilence(int draw = 0);	// Write silence into the buffer

extern int nAudSampleRate;		// sample rate
extern int nAudSegCount;		// Segments in the pdsbLoop buffer
extern int nAudSegLen;			// Segment length in samples (calculated from sound rate/FPS)
extern int nAudAllocSegLen;		// Allocated segment length in samples
extern int16_t * pAudNextSound;		// The next sound segment we will add to the sample loop
extern bool bAudOkay;			// True if sound was inited okay

// Video Output plugin:
struct VidOut {
	int (*Init)();
	int (*Exit)();
};

enum VID_OUT {
	VID_PSGL,
};

int VidSelect(unsigned int driver);
int VidInit();
int VidExit();
int VidFrame();
int VidRedraw();
int VidRecalcPal();
int VidPaint(int bValidate);
int VidReinit();
int VidResize(int nWidth, int nHeight);
const char* VidGetName();
InterfaceInfo* VidGetInfo();
const char* VidDriverName(unsigned int driver);

void VidSwitchFilter(int nEffect);

extern bool bVidOkay;
extern unsigned int nVidSelect;
extern int nVidDepth;
extern int nVidRefresh;
extern int nVidFullscreen;
extern int bVidCorrectAspect;
extern int nVidRotationAdjust;
extern int bVidForce16bit;
extern int bVidFullStretch;
extern int bVidTripleBuffer;
extern int bVidVSync;
extern int nVidScrnWidth, nVidScrnHeight;
extern int nVidScrnDepth;

extern unsigned int vidFilterLinear;

extern int nVidScrnAspectX, nVidScrnAspectY;
extern float vidScrnAspect;
extern bool autoVidScrnAspect;

extern unsigned char* pVidImage;
extern int nVidImageWidth, nVidImageHeight;
extern int nVidImageLeft, nVidImageTop;
extern int nVidImagePitch, nVidImageBPP;
extern int nVidImageDepth;

extern int shaderindex;

extern "C" unsigned int (__cdecl *VidHighCol) (int r, int g, int b, int i);

#endif
