// FB Alpha Funky Jet driver module
// Based on MAME driver by Bryan McPhail

#include "tiles_generic.h"
#include "deco16ic.h"
#include "burn_ym2151.h"
#include "msm6295.h"

static unsigned char *AllMem;
static unsigned char *MemEnd;
static unsigned char *AllRam;
static unsigned char *RamEnd;
static unsigned char *Drv68KROM;
static unsigned char *DrvHucROM;
static unsigned char *DrvGfxROM0;
static unsigned char *DrvGfxROM1;
static unsigned char *DrvGfxROM2;
static unsigned char *DrvSndROM;
static unsigned char *Drv68KRAM;
static unsigned char *DrvPalRAM;
static unsigned char *DrvSprRAM;
static unsigned char *DrvPrtRAM;

static unsigned int  *DrvPalette;
static unsigned char DrvRecalc;

static unsigned char *soundlatch;
static unsigned char *flipscreen;

static unsigned char DrvJoy1[16];
static unsigned char DrvJoy2[16];
static unsigned char DrvDips[2];
static unsigned char DrvReset;
static unsigned short DrvInputs[3];

static struct BurnInputInfo FunkyjetInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"	},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 3"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 15,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy1 + 8,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy1 + 9,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy1 + 10,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy1 + 11,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 12,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 13,	"p2 fire 2"	},
	{"P2 Button 3",		BIT_DIGITAL,	DrvJoy1 + 14,	"p2 fire 3"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Service",		BIT_DIGITAL,	DrvJoy2 + 2,	"service"	},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Funkyjet)

static struct BurnDIPInfo FunkyjetDIPList[]=
{
	{0x14, 0xff, 0xff, 0xff, NULL			},
	{0x15, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x14, 0x01, 0x01, 0x01, "Off"			},
	{0x14, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x14, 0x01, 0x02, 0x02, "Off"			},
	{0x14, 0x01, 0x02, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x14, 0x01, 0x1c, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x10, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x1c, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x1c, 0x0c, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x1c, 0x14, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x1c, 0x04, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x1c, 0x18, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0x1c, 0x08, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x14, 0x01, 0xe0, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0x80, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0xe0, 0xc0, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0xe0, 0x40, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x15, 0x01, 0x01, 0x00, "Off"			},
	{0x15, 0x01, 0x01, 0x01, "On"			},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x15, 0x01, 0x02, 0x00, "No"			},
	{0x15, 0x01, 0x02, 0x02, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Free Play"		},
	{0x15, 0x01, 0x04, 0x04, "Off"			},
	{0x15, 0x01, 0x04, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Freeze"		},
	{0x15, 0x01, 0x08, 0x08, "Off"			},
	{0x15, 0x01, 0x08, 0x00, "On"			},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x15, 0x01, 0x30, 0x10, "Easy"			},
	{0x15, 0x01, 0x30, 0x30, "Normal"		},
	{0x15, 0x01, 0x30, 0x20, "Hard"			},
	{0x15, 0x01, 0x30, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x15, 0x01, 0xc0, 0x80, "1"			},
	{0x15, 0x01, 0xc0, 0xc0, "2"			},
	{0x15, 0x01, 0xc0, 0x40, "3"			},
	{0x15, 0x01, 0xc0, 0x00, "4"			},
};

STDDIPINFO(Funkyjet)

static struct BurnDIPInfo FunkyjetjDIPList[]=
{
	{0x14, 0xff, 0xff, 0xff, NULL			},
	{0x15, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x14, 0x01, 0x01, 0x01, "Off"			},
	{0x14, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x14, 0x01, 0x02, 0x02, "Off"			},
	{0x14, 0x01, 0x02, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x14, 0x01, 0x1c, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x10, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x1c, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x1c, 0x0c, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x1c, 0x14, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x1c, 0x04, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x1c, 0x18, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0x1c, 0x08, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x14, 0x01, 0xe0, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0x80, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0xe0, 0xc0, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0xe0, 0x40, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x15, 0x01, 0x01, 0x01, "Off"			},
	{0x15, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x15, 0x01, 0x02, 0x00, "No"			},
	{0x15, 0x01, 0x02, 0x02, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Free Play"		},
	{0x15, 0x01, 0x04, 0x04, "Off"			},
	{0x15, 0x01, 0x04, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Freeze"		},
	{0x15, 0x01, 0x08, 0x08, "Off"			},
	{0x15, 0x01, 0x08, 0x00, "On"			},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x15, 0x01, 0x30, 0x10, "Easy"			},
	{0x15, 0x01, 0x30, 0x30, "Normal"		},
	{0x15, 0x01, 0x30, 0x20, "Hard"			},
	{0x15, 0x01, 0x30, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x15, 0x01, 0xc0, 0x80, "1"			},
	{0x15, 0x01, 0xc0, 0xc0, "2"			},
	{0x15, 0x01, 0xc0, 0x40, "3"			},
	{0x15, 0x01, 0xc0, 0x00, "4"			},
};

STDDIPINFO(Funkyjetj)

static struct BurnDIPInfo SotsugyoDIPList[]=
{
	{0x14, 0xff, 0xff, 0xff, NULL			},
	{0x15, 0xff, 0xff, 0xed, NULL			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x14, 0x01, 0x01, 0x01, "Off"			},
	{0x14, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x14, 0x01, 0x02, 0x02, "Off"			},
	{0x14, 0x01, 0x02, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x14, 0x01, 0x1c, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x10, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x1c, 0x1c, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x1c, 0x0c, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x1c, 0x14, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x1c, 0x04, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x1c, 0x18, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0x1c, 0x08, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x14, 0x01, 0xe0, 0x00, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0x80, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0xe0, 0xc0, "1 Coin  5 Credits"	},
	{0x14, 0x01, 0xe0, 0x40, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    0, "Demo Sounds"		},
	{0x15, 0x01, 0x02, 0x02, "Off"			},
	{0x15, 0x01, 0x02, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x15, 0x01, 0x0c, 0x04, "1"			},
	{0x15, 0x01, 0x0c, 0x08, "2"			},
	{0x15, 0x01, 0x0c, 0x0c, "3"			},
	{0x15, 0x01, 0x0c, 0x00, "4"			},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x15, 0x01, 0x30, 0x30, "Easy"			},
	{0x15, 0x01, 0x30, 0x20, "Normal"		},
	{0x15, 0x01, 0x30, 0x10, "Hard"			},
	{0x15, 0x01, 0x30, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Free Play"		},
	{0x15, 0x01, 0x40, 0x40, "Off"			},
	{0x15, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Freeze"		},
	{0x15, 0x01, 0x80, 0x80, "Off"			},
	{0x15, 0x01, 0x80, 0x00, "On"			},
};

STDDIPINFO(Sotsugyo)

void __fastcall funkyjet_main_write_word(unsigned int address, unsigned short data)
{
	deco16_write_control_word(0, address, 0x300000, data)

	switch (address)
	{
		case 0x18010a:
			*soundlatch = data;
		//	cputag_set_input_line(space->machine, "audiocpu", 0, HOLD_LINE);
		break;
	}

	if ((address & 0xfff800) == 0x180000) {
		*((unsigned short*)(DrvPrtRAM + (address & 0x7fe))) = data;
		return;
	}
}

void __fastcall funkyjet_main_write_byte(unsigned int address, unsigned char data)
{
	switch (address)
	{
		case 0x18010a:
		case 0x18010b:
			*soundlatch = data;
		//	cputag_set_input_line(space->machine, "audiocpu", 0, HOLD_LINE);
		break;
	}

	if ((address & 0xfff800) == 0x180000) {
		DrvPrtRAM[(address & 0x7ff)^1] = data;
		return;
	}
}

unsigned short __fastcall funkyjet_main_read_word(unsigned int address)
{
	if ((address & 0xfff800) == 0x180000) {
		return deco16_146_funkyjet_prot_r(address);
	}

	return 0;
}

unsigned char __fastcall funkyjet_main_read_byte(unsigned int address)
{
	if ((address & 0xfff800) == 0x180000) {
		return deco16_146_funkyjet_prot_r(address) >> ((~address & 1) << 3);
	}

	return 0;
}

static void DrvYM2151IrqHandler(int state)
{
	state = state; // kill warnings...
//	HucSetIRQLine(1, state ? HUC_IRQSTATUS_ACK : HUC_IRQSTATUS_NONE);
}

static int DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

	SekOpen(0);
	SekReset();
	SekClose();

	// Huc6280

	MSM6295Reset(0);
	BurnYM2151Reset();

	deco16Reset();

	return 0;
}

static int MemIndex()
{
	unsigned char *Next; Next = AllMem;

	Drv68KROM	= Next; Next += 0x080000;
	DrvHucROM	= Next; Next += 0x010000;

	DrvGfxROM0	= Next; Next += 0x100000;
	DrvGfxROM1	= Next; Next += 0x100000;
	DrvGfxROM2	= Next; Next += 0x200000;

	MSM6295ROM	= Next;
	DrvSndROM	= Next; Next += 0x040000;

	DrvPalette	= (unsigned int*)Next; Next += 0x0400 * sizeof(int);

	AllRam		= Next;

	Drv68KRAM	= Next; Next += 0x004000;
	DrvSprRAM	= Next; Next += 0x000800;
	deco16_prot_ram	= (unsigned short*)Next;
	DrvPrtRAM	= Next; Next += 0x000800;
	DrvPalRAM	= Next; Next += 0x000800;

	soundlatch	= Next; Next += 0x000001;
	flipscreen	= Next; Next += 0x000001;

	RamEnd		= Next;

	MemEnd		= Next;

	return 0;
}

static int DrvInit()
{
	BurnSetRefreshRate(58.00);

	AllMem = NULL;
	MemIndex();
	int nLen = MemEnd - (unsigned char *)0;
	if ((AllMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(Drv68KROM  + 0x00001,  0, 2)) return 1;
		if (BurnLoadRom(Drv68KROM  + 0x00000,  1, 2)) return 1;

		if (BurnLoadRom(DrvHucROM  + 0x00000,  2, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  3, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM2 + 0x00000,  4, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x80000,  5, 1)) return 1;

		if (BurnLoadRom(DrvSndROM  + 0x00000,  6, 1)) return 1;

		deco74_decrypt_gfx(DrvGfxROM1, 0x080000);

		deco16_tile_decode(DrvGfxROM1, DrvGfxROM0, 0x080000, 1);
		deco16_tile_decode(DrvGfxROM1, DrvGfxROM1, 0x080000, 0);
		deco16_tile_decode(DrvGfxROM2, DrvGfxROM2, 0x100000, 0);
	}	

	deco16Init(1, 0, 1);
	deco16_set_graphics(DrvGfxROM0, 0x80000 * 2, DrvGfxROM1, 0x80000 * 2, NULL, 0);
	deco16_set_color_base(0, 256);
	deco16_set_color_base(1, 512);
	deco16_set_global_offsets(0, 8);

	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KROM,			0x000000, 0x07ffff, SM_ROM);
	SekMapMemory(DrvPalRAM,			0x120000, 0x1207ff, SM_RAM);
	SekMapMemory(Drv68KRAM,			0x140000, 0x143fff, SM_RAM);
	SekMapMemory(DrvSprRAM,			0x160000, 0x1607ff, SM_RAM);
	SekMapMemory(deco16_pf_ram[0],		0x320000, 0x321fff, SM_RAM);
	SekMapMemory(deco16_pf_ram[1],		0x322000, 0x323fff, SM_RAM);
	SekMapMemory(deco16_pf_rowscroll[0],	0x340000, 0x340bff, SM_RAM);
	SekMapMemory(deco16_pf_rowscroll[1],	0x342000, 0x342bff, SM_RAM);
	SekSetWriteWordHandler(0,		funkyjet_main_write_word);
	SekSetWriteByteHandler(0,		funkyjet_main_write_byte);
	SekSetReadWordHandler(0,		funkyjet_main_read_word);
	SekSetReadByteHandler(0,		funkyjet_main_read_byte);
	SekClose();

	// Huc6280...

	MSM6295Init(0, 1000000 / 132, 50.0, 1);

	BurnYM2151Init(3580000, 40.0);
	BurnYM2151SetIrqHandler(&DrvYM2151IrqHandler);

	GenericTilesInit();

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	GenericTilesExit();
	deco16Exit();

	MSM6295Exit(0);
	BurnYM2151Exit();

	SekExit();
	// huc6280

	if (AllMem) {
		free (AllMem);
		AllMem = NULL;
	}

	MSM6295ROM = NULL;

	return 0;
}

static void DrvPaletteRecalc()
{
	unsigned short *p = (unsigned short*)DrvPalRAM;

	for (int i = 0; i < 0x800 / 2; i++) {
		int b = (p[i] >> 8) & 0x0f;
		int g = (p[i] >> 4) & 0x0f;
		int r = (p[i] >> 0) & 0x0f;

		r |= r << 4;
		g |= g << 4;
		b |= b << 4;

		DrvPalette[i] = BurnHighCol(r, g, b, 0);
	}
}

static void draw_sprites()
{
	unsigned short *ram = (unsigned short*)DrvSprRAM;

	for (int offs = 0; offs < 0x400; offs += 4)
	{
		int inc, mult;

		int sy     = ram[offs + 0];
		int code   = ram[offs + 1] & 0x3fff;
		int sx     = ram[offs + 2];

		if ((sy & 0x1000) && (nCurrentFrame & 1)) continue;

		int color = (sx >> 9) & 0x1f;

		int flipx = sy & 0x2000;
		int flipy = sy & 0x4000;
		int multi = (1 << ((sy & 0x0600) >> 9)) - 1;

		sx &= 0x01ff;
		sy &= 0x01ff;
		if (sx >= 320) sx -= 512;
		if (sy >= 256) sy -= 512;
		sy = 240 - sy;
		sx = 304 - sx;

		code &= ~multi;

		if (flipy) {
			inc = -1;
		} else {
			code += multi;
			inc = 1;
		}

		if (*flipscreen)
		{
			sy = 240 - sy;
			sx = 304 - sx;
			flipx = !flipx;
			flipy = !flipy;
			mult = 16;
		}
		else
			mult = -16;

		if (sx >= 320 || sx < -15 || sy >= nScreenHeight) continue;

		while (multi >= 0)
		{
			int y = ((sy + mult * multi) & 0x1ff) - 8;
			int c = (code - multi * inc) & 0x3fff;

			if (flipy) {
				if (flipx) {
					Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, c, sx, y, color, 4, 0, 0, DrvGfxROM2);
				} else {
					Render16x16Tile_Mask_FlipY_Clip(pTransDraw, c, sx, y, color, 4, 0, 0, DrvGfxROM2);
				}
			} else {
				if (flipx) {
					Render16x16Tile_Mask_FlipX_Clip(pTransDraw, c, sx, y, color, 4, 0, 0, DrvGfxROM2);
				} else {
					Render16x16Tile_Mask_Clip(pTransDraw, c, sx, y, color, 4, 0, 0, DrvGfxROM2);
				}
			}

			multi--;
		}
	}
}

static int DrvDraw()
{
//	if (DrvRecalc) {
		DrvPaletteRecalc();
		DrvRecalc = 0;
//	}

	deco16_pf12_update();

	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
		pTransDraw[i] = 0x300;
	}

	if (nBurnLayer & 1) deco16_draw_layer(1, pTransDraw, DECO16_LAYER_OPAQUE);
	if (nBurnLayer & 2) deco16_draw_layer(0, pTransDraw, 0);

	if (nBurnLayer & 4) draw_sprites();

	BurnTransferCopy(DrvPalette);

	return 0;
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	{
		deco16_prot_inputs = DrvInputs;
		memset (DrvInputs, 0xff, 2 * sizeof(short)); 
		for (int i = 0; i < 16; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
		DrvInputs[2] = (DrvDips[1] << 8) | (DrvDips[0] << 0);
	}

	int nInterleave = 256;
	int nCyclesTotal[2] = { 14000000 / 58, 8055000 / 58 };
	int nCyclesDone[2] = { 0, 0 };

	SekOpen(0);

	deco16_vblank = 0;

	for (int i = 0; i < nInterleave; i++)
	{
		nCyclesDone[0] += SekRun(nCyclesTotal[0] / nInterleave);
	//	nCyclesDone[1] += HucRun(nCyclesTotal[1] / nInterleave);

		if (i == 248) deco16_vblank = 0x08;
	}

	SekSetIRQLine(6, SEK_IRQSTATUS_AUTO);

	SekClose();

	if (pBurnSoundOut) {
		BurnYM2151Render(pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	}

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}

static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {
		*pnMin = 0x029682;
	}

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd-AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		SekScan(nAction);
	//	huc6280

		deco16Scan();

		BurnYM2151Scan(nAction);
		MSM6295Scan(0, nAction);
	}

	return 0;
}


// Funky Jet (World)

static struct BurnRomInfo funkyjetRomDesc[] = {
	{ "jk00.12f",		0x40000, 0x712089c1, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "jk01.13f",		0x40000, 0xbe3920d7, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "jk02.16f",		0x10000, 0x748c0bd8, 2 | BRF_PRG | BRF_ESS }, //  2 Huc6280 Code

	{ "mat02",		0x80000, 0xe4b94c7e, 3 | BRF_GRA },           //  3 Characters & Background Tiles

	{ "mat01",		0x80000, 0x24093a8d, 4 | BRF_GRA },           //  4 Sprites
	{ "mat00",		0x80000, 0xfbda0228, 4 | BRF_GRA },           //  5

	{ "jk03.15h",		0x20000, 0x69a0eaf7, 5 | BRF_SND },           //  6 OKI M6295 Samples
};

STD_ROM_PICK(funkyjet)
STD_ROM_FN(funkyjet)

struct BurnDriver BurnDrvFunkyjet = {
	"funkyjet", NULL, NULL, NULL, "1992",
	"Funky Jet (World)\0", "No sound", "[Data East] (Mitchell license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_POST90S, GBF_PLATFORM, 0,
	NULL, NULL, funkyjetRomInfo, funkyjetRomName, NULL, NULL, FunkyjetInputInfo, FunkyjetDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	320, 240, 4, 3
};


// Funky Jet (Japan)

static struct BurnRomInfo funkyjetjRomDesc[] = {
	{ "jh00-2.11f",		0x40000, 0x5b98b700, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "jh01-2.13f",		0x40000, 0x21280220, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "jk02.16f",		0x10000, 0x748c0bd8, 2 | BRF_PRG | BRF_ESS }, //  2 Huc6280 Code

	{ "mat02",		0x80000, 0xe4b94c7e, 3 | BRF_GRA },           //  3 Characters & Background Tiles

	{ "mat01",		0x80000, 0x24093a8d, 4 | BRF_GRA },           //  4 Sprites
	{ "mat00",		0x80000, 0xfbda0228, 4 | BRF_GRA },           //  5

	{ "jk03.15h",		0x20000, 0x69a0eaf7, 5 | BRF_SND },           //  6 OKI M6295 Samples
};

STD_ROM_PICK(funkyjetj)
STD_ROM_FN(funkyjetj)

struct BurnDriver BurnDrvFunkyjetj = {
	"funkyjetj", "funkyjet", NULL, NULL, "1992",
	"Funky Jet (Japan)\0", "No sound", "Data East Corporation", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_POST90S, GBF_PLATFORM, 0,
	NULL, NULL, funkyjetjRomInfo, funkyjetjRomName, NULL, NULL, FunkyjetInputInfo, FunkyjetjDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	320, 240, 4, 3
};


// Sotsugyo Shousho

static struct BurnRomInfo sotsugyoRomDesc[] = {
	{ "03.12f",		0x40000, 0xd175dfd1, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "04.13f",		0x40000, 0x2072477c, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "sb020.16f",		0x10000, 0xbaf5ec93, 2 | BRF_PRG | BRF_ESS }, //  2 Huc6280 Code

	{ "02.2f",		0x80000, 0x337b1451, 3 | BRF_GRA },           //  3 Characters & Background Tiles

	{ "01.4a",		0x80000, 0xfa10dd54, 4 | BRF_GRA },           //  4 Sprites
	{ "00.2a",		0x80000, 0xd35a14ef, 4 | BRF_GRA },           //  5

	{ "sb030.15h",		0x20000, 0x1ea43f48, 5 | BRF_SND },           //  6 OKI M6295 Samples
};

STD_ROM_PICK(sotsugyo)
STD_ROM_FN(sotsugyo)

struct BurnDriver BurnDrvSotsugyo = {
	"sotsugyo", NULL, NULL, NULL, "1995",
	"Sotsugyo Shousho\0", "No sound", "Mitchell (Atlus license)", "Miscellaneous",
	L"\u5352\u696D\u8A3C\u66F8\0Sotsugyo Shousho\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_POST90S, GBF_MINIGAMES, 0,
	NULL, NULL, sotsugyoRomInfo, sotsugyoRomName, NULL, NULL, FunkyjetInputInfo, SotsugyoDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	320, 240, 4, 3
};
