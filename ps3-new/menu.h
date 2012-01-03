/******************************************************************************* 
 * menu.h - FBA Next Slim PS3
 *
 *  Created on: Jan 1, 2012
********************************************************************************/

#ifndef MENU_H_
#define MENU_H_

#include "colors.h"
#include "emu-ps3-constants.h"

/* structs */

typedef struct
{
	uint32_t enum_id;		// enum ID of item
	char text[256];			// item label text
	float text_xpos;		// text X position item (upper left corner)
	float text_ypos;		// text Y position item (upper left corner)
	uint32_t text_selected_color;	// text color if item is selected
	uint32_t text_unselected_color;	// text color if item is not selected
	char comment[256];		// item comment - displayed in info bar
	uint32_t comment_color;		// color of item comment
	float comment_scalefont;	// font size of item comment
	float comment_xpos;		// comment X position (upper left corner)
	float comment_ypos;		// comment Y position (upper left corner)
	unsigned int * setting_ptr;	// pointer to setting member
	char comment_yes[256];		// item comment (yes - if setting_ptr true)
	char comment_no[256];		// item comment (no - if setting_ptr false)
	uint32_t default_value;		// default value of item
	uint32_t enabled;		// is the item enabled?
} item;

typedef struct
{
	char title[64];			// menu title
	uint32_t enum_id;		// enum ID of menu
	uint8_t selected;		// index of selected item
	uint8_t page;			// page
	uint32_t refreshpage;		// bit whether or not to refresh page
	uint32_t first_setting;		// first setting
	uint32_t max_settings;		// max no of settings in menu
	item *items;			// menu items
} menu;

/* constants */

#define FILE_BROWSER_MENU		0
#define GENERAL_VIDEO_MENU		1
#define GENERAL_AUDIO_MENU		2
#define EMU_GENERAL_MENU		3
#define EMU_AUDIO_MENU			4
#define EMU_VIDEO_MENU			5
#define PATH_MENU			6
#define CONTROLS_MENU			7
#define GAME_AWARE_SHADER_CHOICE	8
#define SHADER_CHOICE			9
#define PRESET_CHOICE			10
#define BORDER_CHOICE			11
#define PATH_SAVESTATES_DIR_CHOICE	12
#define PATH_DEFAULT_ROM_DIR_CHOICE	13
#define PATH_SRAM_DIR_CHOICE		15
#define EXCART_CHOICE			16
#define INPUT_PRESET_CHOICE		17

enum
{
	SETTING_CHANGE_RESOLUTION,
	//SETTING_PAL60_MODE,
	SETTING_SHADER_PRESETS,
	SETTING_BORDER,
	SETTING_SHADER,
	SETTING_SHADER_2,
	SETTING_FONT_SIZE,
	SETTING_KEEP_ASPECT_RATIO,
	SETTING_HW_TEXTURE_FILTER,
	SETTING_HW_TEXTURE_FILTER_2,
	SETTING_SCALE_ENABLED,
	SETTING_SCALE_FACTOR,
	SETTING_HW_OVERSCAN_AMOUNT,
	SETTING_THROTTLE_MODE,
	SETTING_TRIPLE_BUFFERING,
	SETTING_ENABLE_SCREENSHOTS,
	SETTING_SAVE_SHADER_PRESET,
	SETTING_APPLY_SHADER_PRESET_ON_STARTUP,
	SETTING_DEFAULT_VIDEO_ALL,
	SETTING_SOUND_MODE,
	SETTING_RSOUND_SERVER_IP_ADDRESS,
	SETTING_DEFAULT_AUDIO_ALL,
	SETTING_EMU_CURRENT_SAVE_STATE_SLOT,
	SETTING_EMU_DEFAULT_ALL,
	SETTING_EMU_VIDEO_DEFAULT_ALL,
	SETTING_EMU_AUDIO_DEFAULT_ALL,
	SETTING_PATH_DEFAULT_ROM_DIRECTORY,
	SETTING_PATH_SAVESTATES_DIRECTORY,
	SETTING_PATH_SRAM_DIRECTORY,
	SETTING_PATH_DEFAULT_ALL,
	SETTING_CONTROLS_SCHEME,                  // CONTROLS settings
	SETTING_CONTROLS_NUMBER,
	SETTING_CONTROLS_DPAD_UP,
	SETTING_CONTROLS_DPAD_DOWN,
	SETTING_CONTROLS_DPAD_LEFT,
	SETTING_CONTROLS_DPAD_RIGHT,
	SETTING_CONTROLS_BUTTON_CIRCLE,
	SETTING_CONTROLS_BUTTON_CROSS,
	SETTING_CONTROLS_BUTTON_TRIANGLE,
	SETTING_CONTROLS_BUTTON_SQUARE,
	SETTING_CONTROLS_BUTTON_SELECT,
	SETTING_CONTROLS_BUTTON_START,
	SETTING_CONTROLS_BUTTON_L1,
	SETTING_CONTROLS_BUTTON_R1,
	SETTING_CONTROLS_BUTTON_L2,
	SETTING_CONTROLS_BUTTON_R2,
	SETTING_CONTROLS_BUTTON_L3,
	SETTING_CONTROLS_BUTTON_R3,
	SETTING_CONTROLS_BUTTON_L2_BUTTON_L3,
	SETTING_CONTROLS_BUTTON_L2_BUTTON_R3,
	SETTING_CONTROLS_BUTTON_L2_ANALOG_R_RIGHT,
	SETTING_CONTROLS_BUTTON_L2_ANALOG_R_LEFT,
	SETTING_CONTROLS_BUTTON_L2_ANALOG_R_UP,
	SETTING_CONTROLS_BUTTON_L2_ANALOG_R_DOWN,
	SETTING_CONTROLS_BUTTON_R2_ANALOG_R_RIGHT,
	SETTING_CONTROLS_BUTTON_R2_ANALOG_R_LEFT,
	SETTING_CONTROLS_BUTTON_R2_ANALOG_R_UP,
	SETTING_CONTROLS_BUTTON_R2_ANALOG_R_DOWN,
	SETTING_CONTROLS_BUTTON_R2_BUTTON_R3,
	SETTING_CONTROLS_BUTTON_R3_BUTTON_L3,
	SETTING_CONTROLS_ANALOG_R_UP,
	SETTING_CONTROLS_ANALOG_R_DOWN,
	SETTING_CONTROLS_ANALOG_R_LEFT,
	SETTING_CONTROLS_ANALOG_R_RIGHT,
	SETTING_CONTROLS_SAVE_CUSTOM_CONTROLS,
	SETTING_CONTROLS_DEFAULT_ALL
};

#define FIRST_VIDEO_SETTING				0
#define FIRST_AUDIO_SETTING				SETTING_DEFAULT_VIDEO_ALL+1
#define FIRST_EMU_SETTING				SETTING_DEFAULT_AUDIO_ALL+1
#define FIRST_EMU_VIDEO_SETTING				SETTING_EMU_DEFAULT_ALL+1
#define FIRST_EMU_AUDIO_SETTING				SETTING_EMU_VIDEO_DEFAULT_ALL+1
#define FIRST_PATH_SETTING				SETTING_EMU_AUDIO_DEFAULT_ALL+1
#define FIRST_CONTROLS_SETTING_PAGE_1			SETTING_PATH_DEFAULT_ALL+1
#define FIRST_CONTROL_BIND				SETTING_CONTROLS_DPAD_UP

#define MAX_NO_OF_VIDEO_SETTINGS			SETTING_DEFAULT_VIDEO_ALL+1
#define MAX_NO_OF_AUDIO_SETTINGS			SETTING_DEFAULT_AUDIO_ALL+1
#define MAX_NO_OF_EMU_SETTINGS				SETTING_EMU_DEFAULT_ALL+1
#define MAX_NO_OF_EMU_VIDEO_SETTINGS			SETTING_EMU_VIDEO_DEFAULT_ALL+1
#define MAX_NO_OF_EMU_AUDIO_SETTINGS			SETTING_EMU_AUDIO_DEFAULT_ALL+1
#define MAX_NO_OF_PATH_SETTINGS				SETTING_PATH_DEFAULT_ALL+1
#define MAX_NO_OF_CONTROLS_SETTINGS			SETTING_CONTROLS_DEFAULT_ALL+1

void MenuInit(void);
void MenuMainLoop();

extern uint32_t menu_is_running;

#endif /* MENU_H_ */

