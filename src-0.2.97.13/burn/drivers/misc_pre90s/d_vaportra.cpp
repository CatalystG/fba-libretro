// FB Alpha Vapor Trail driver module
// Based on MAME driver by Bryan McPhail

#include "tiles_generic.h"
#include "deco16ic.h"
#include "burn_ym2203.h"
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
static unsigned char *DrvGfxROM3;
static unsigned char *DrvSndROM0;
static unsigned char *DrvSndROM1;
static unsigned char *Drv68KRAM;
static unsigned char *DrvPalRAM0;
static unsigned char *DrvPalRAM1;
static unsigned char *DrvSprRAM;
static unsigned char *DrvSprBuf;

static unsigned int  *DrvPalette;
static unsigned char DrvRecalc;

static unsigned char *soundlatch;
static unsigned char *flipscreen;
static unsigned short *priority;

static unsigned char DrvJoy1[16];
static unsigned char DrvJoy2[16];
static unsigned char DrvDips[2];
static unsigned char DrvReset;
static unsigned short DrvInputs[2];

static struct BurnInputInfo VaportraInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 15,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy1 + 8,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy1 + 9,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy1 + 10,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy1 + 11,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 12,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 13,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Vaportra)

static struct BurnDIPInfo VaportraDIPList[]=
{
	{0x11, 0xff, 0xff, 0x7f, NULL			},
	{0x12, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x11, 0x01, 0x03, 0x00, "2"			},
	{0x11, 0x01, 0x03, 0x03, "3"			},
	{0x11, 0x01, 0x03, 0x02, "4"			},
	{0x11, 0x01, 0x03, 0x01, "5"			},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x11, 0x01, 0x0c, 0x08, "Easy"			},
	{0x11, 0x01, 0x0c, 0x0c, "Normal"		},
	{0x11, 0x01, 0x0c, 0x04, "Hard"			},
	{0x11, 0x01, 0x0c, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x11, 0x01, 0x30, 0x20, "150k, 300k and 600k"	},
	{0x11, 0x01, 0x30, 0x30, "200k and 600k"	},
	{0x11, 0x01, 0x30, 0x10, "300k only"		},
	{0x11, 0x01, 0x30, 0x00, "None"			},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x11, 0x01, 0x40, 0x00, "No"			},
	{0x11, 0x01, 0x40, 0x40, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x11, 0x01, 0x80, 0x80, "Off"			},
	{0x11, 0x01, 0x80, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x12, 0x01, 0x07, 0x00, "3 Coins 1 Credits"	},
	{0x12, 0x01, 0x07, 0x01, "2 Coins 1 Credits"	},
	{0x12, 0x01, 0x07, 0x07, "1 Coin  1 Credits"	},
	{0x12, 0x01, 0x07, 0x06, "1 Coin  2 Credits"	},
	{0x12, 0x01, 0x07, 0x05, "1 Coin  3 Credits"	},
	{0x12, 0x01, 0x07, 0x04, "1 Coin  4 Credits"	},
	{0x12, 0x01, 0x07, 0x03, "1 Coin  5 Credits"	},
	{0x12, 0x01, 0x07, 0x02, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x12, 0x01, 0x38, 0x00, "3 Coins 1 Credits"	},
	{0x12, 0x01, 0x38, 0x08, "2 Coins 1 Credits"	},
	{0x12, 0x01, 0x38, 0x38, "1 Coin  1 Credits"	},
	{0x12, 0x01, 0x38, 0x30, "1 Coin  2 Credits"	},
	{0x12, 0x01, 0x38, 0x28, "1 Coin  3 Credits"	},
	{0x12, 0x01, 0x38, 0x20, "1 Coin  4 Credits"	},
	{0x12, 0x01, 0x38, 0x18, "1 Coin  5 Credits"	},
	{0x12, 0x01, 0x38, 0x10, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x12, 0x01, 0x40, 0x40, "Off"			},
	{0x12, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x12, 0x01, 0x80, 0x80, "Off"			},
	{0x12, 0x01, 0x80, 0x00, "On"			},
};

STDDIPINFO(Vaportra)

void __fastcall vaportra_main_write_word(unsigned int address, unsigned short data)
{
	deco16_write_control_word(1, address, 0x240000, data)
	deco16_write_control_word(0, address, 0x2c0000, data)

	switch (address)
	{
		case 0x100000:
		case 0x100002:
			priority[(address & 2)/2] = data;
		return;

		case 0x100006:
			*soundlatch = data;
			// cpu_set_input_line(state->Huc6280 Code, 0, ASSERT_LINE);
		return;

		case 0x30c000:
			memcpy (DrvSprBuf, DrvSprRAM, 0x800);
		return;
	}
}

void __fastcall vaportra_main_write_byte(unsigned int address, unsigned char data)
{
	switch (address)
	{
		case 0x100000:
		case 0x100001:
		case 0x100002:
		case 0x100003:
			priority[(address & 2)/2] = data;
		return;

		case 0x100006:
		case 0x100007:
			*soundlatch = data;
			// cpu_set_input_line(state->Huc6280 Code, 0, ASSERT_LINE);
		return;

		case 0x30c000:
		case 0x30c001:
			memcpy (DrvSprBuf, DrvSprRAM, 0x800);
		return;
	}
}

unsigned short __fastcall vaportra_main_read_word(unsigned int address)
{
	switch (address)
	{
		case 0x100000:
			return DrvInputs[0];

		case 0x100002:
			return (DrvInputs[1] & 0xfff7) | (deco16_vblank & 0x08);

		case 0x100004:
			return (DrvDips[1] << 8) | (DrvDips[0] << 0);

		case 0x100006:
		case 0x100008:
		case 0x10000a:
		case 0x10000c:
		case 0x10000e:
			return 0xffff;
	}

	return 0;
}

unsigned char __fastcall vaportra_main_read_byte(unsigned int address)
{
	switch (address)
	{
		case 0x100000:
			return DrvInputs[0] >> 8;

		case 0x100001:
			return DrvInputs[0] >> 0;

		case 0x100002:
		case 0x100003:
			return (DrvInputs[1] & 0xf7) | (deco16_vblank & 0x08);

		case 0x100004:
			return DrvDips[1];

		case 0x100005:
			return DrvDips[0];

		case 0x100006:
		case 0x100007:
		case 0x100008:
		case 0x100009:
		case 0x10000a:
		case 0x10000b:
		case 0x10000c:
		case 0x10000d:
		case 0x10000e:
		case 0x10000f:
			return 0xff;
	}

	return 0;
}

static int DrvSynchroniseStream(int nSoundRate)
{
	return 0 * nSoundRate; //(long long)HucTotalCycles() * nSoundRate / 8055000;
}

static double DrvGetTime()
{
	return 0; //(double)HucTotalCycles() / 8055000.0;
}

static void DrvYM2151IrqHandler(int state)
{
	state = state; // kill warnings...
//	HucSetIRQLine(1, state ? HUC_IRQSTATUS_ACK : HUC_IRQSTATUS_NONE);
}

static int vaportra_bank_callback( const int bank )
{
	return ((bank >> 4) & 0x7) * 0x1000;
}

static int DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

	SekOpen(0);
	SekReset();
	SekClose();

	// Huc6280

	BurnYM2203Reset();
	BurnYM2151Reset();
	MSM6295Reset(0);
	MSM6295Reset(1);

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
	DrvGfxROM3	= Next; Next += 0x200000;

	MSM6295ROM	= Next;
	DrvSndROM0	= Next; Next += 0x040000;
	DrvSndROM1	= Next; Next += 0x040000;

	DrvPalette	= (unsigned int*)Next; Next += 0x0500 * sizeof(int);

	AllRam		= Next;

	Drv68KRAM	= Next; Next += 0x004000;
	DrvSprRAM	= Next; Next += 0x000800;
	DrvSprBuf	= Next; Next += 0x000800;
	DrvPalRAM0	= Next; Next += 0x000a00;
	DrvPalRAM1	= Next; Next += 0x000a00;

	soundlatch	= Next; Next += 0x000001;
	flipscreen	= Next; Next += 0x000001;
	priority	= (unsigned short*)Next; Next += 0x000002 * sizeof(short);

	RamEnd		= Next;

	MemEnd		= Next;

	return 0;
}

static void VaportraDecode()
{
	for (int i = 0x00000; i < 0x80000; i++)
		Drv68KROM[i] = (Drv68KROM[i] & 0x7e) | ((Drv68KROM[i] & 0x01) << 7) | ((Drv68KROM[i] & 0x80) >> 7);
}

static int DrvInit(int type)
{
	BurnSetRefreshRate(58.00);

	AllMem = NULL;
	MemIndex();
	int nLen = MemEnd - (unsigned char *)0;
	if ((AllMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(Drv68KROM  + 0x000001,  0, 2)) return 1;
		if (BurnLoadRom(Drv68KROM  + 0x000000,  1, 2)) return 1;
		if (BurnLoadRom(Drv68KROM  + 0x040001,  2, 2)) return 1;
		if (BurnLoadRom(Drv68KROM  + 0x040000,  3, 2)) return 1;
		VaportraDecode();

		if (BurnLoadRom(DrvHucROM  + 0x000000,  4, 1)) return 1;

		if (type) // world revision 3
		{
			if (BurnLoadRom(DrvGfxROM0 + 0x000000,  5, 2)) return 1;
			if (BurnLoadRom(DrvGfxROM0 + 0x000001,  6, 2)) return 1;
			if (BurnLoadRom(DrvGfxROM0 + 0x040000,  7, 2)) return 1;
			if (BurnLoadRom(DrvGfxROM0 + 0x040001,  8, 2)) return 1;

			if (BurnLoadRom(DrvGfxROM2 + 0x000000,  9, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + 0x080000, 10, 1)) return 1;

			if (BurnLoadRom(DrvGfxROM3 + 0x000000, 11, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM3 + 0x080000, 12, 1)) return 1;

			if (BurnLoadRom(DrvSndROM0 + 0x000000, 13, 1)) return 1;

			if (BurnLoadRom(DrvSndROM1 + 0x000000, 14, 1)) return 1;
		} else {
			if (BurnLoadRom(DrvGfxROM0 + 0x000000,  5, 1)) return 1;

			if (BurnLoadRom(DrvGfxROM2 + 0x000000,  6, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + 0x080000,  7, 1)) return 1;

			if (BurnLoadRom(DrvGfxROM3 + 0x000000,  8, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM3 + 0x080000,  9, 1)) return 1;

			if (BurnLoadRom(DrvSndROM0 + 0x000000, 10, 1)) return 1;

			if (BurnLoadRom(DrvSndROM1 + 0x000000, 11, 1)) return 1;
		}

		memcpy (DrvGfxROM1 + 0x040000, DrvGfxROM0 + 0x000000, 0x040000);
		memcpy (DrvGfxROM1 + 0x000000, DrvGfxROM0 + 0x040000, 0x040000);

		deco16_tile_decode(DrvGfxROM1, DrvGfxROM0, 0x080000, 1);
		deco16_tile_decode(DrvGfxROM1, DrvGfxROM1, 0x080000, 0);
		deco16_tile_decode(DrvGfxROM2, DrvGfxROM2, 0x100000, 0);
		deco16_tile_decode(DrvGfxROM3, DrvGfxROM3, 0x100000, 0);
	}

	deco16Init(0, 0, 1);
	deco16_set_graphics(DrvGfxROM0, 0x100000, DrvGfxROM1, 0x100000, DrvGfxROM2, 0x200000);
	deco16_set_color_base(0, 0x000);
	deco16_set_color_base(1, 0x200);
	deco16_set_color_base(2, 0x300);
	deco16_set_color_base(3, 0x400);
	deco16_set_bank_callback(0, vaportra_bank_callback);
	deco16_set_bank_callback(1, vaportra_bank_callback);
	deco16_set_bank_callback(2, vaportra_bank_callback);
	deco16_set_bank_callback(3, vaportra_bank_callback);

	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KROM,			0x000000, 0x07ffff, SM_ROM);
	SekMapMemory(deco16_pf_ram[2],		0x200000, 0x201fff, SM_RAM);
	SekMapMemory(deco16_pf_ram[3],		0x202000, 0x203fff, SM_RAM);
	SekMapMemory(deco16_pf_ram[0],		0x280000, 0x281fff, SM_RAM);
	SekMapMemory(deco16_pf_ram[1],		0x282000, 0x283fff, SM_RAM);
	SekMapMemory(DrvPalRAM0,		0x300000, 0x3009ff, SM_RAM);
	SekMapMemory(DrvPalRAM1,		0x304000, 0x3049ff, SM_RAM);
	SekMapMemory(DrvSprRAM,			0xff8000, 0xff87ff, SM_RAM);
	SekMapMemory(Drv68KRAM,			0xffc000, 0xffffff, SM_RAM);
	SekSetWriteWordHandler(0,		vaportra_main_write_word);
	SekSetWriteByteHandler(0,		vaportra_main_write_byte);
	SekSetReadWordHandler(0,		vaportra_main_read_word);
	SekSetReadByteHandler(0,		vaportra_main_read_byte);
	SekClose();

	// Huc6280...

	BurnYM2151Init(3580000, 40.0);
	BurnYM2151SetIrqHandler(&DrvYM2151IrqHandler);

	BurnYM2203Init(1, 4027500, NULL, DrvSynchroniseStream, DrvGetTime, 1);
//	BurnTimerAttachHuc(8055000);

	MSM6295Init(0, 1006875 / 132, 75.0, 1);
	MSM6295Init(1, 2013750 / 132, 60.0, 1);

	GenericTilesInit();

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	GenericTilesExit();
	deco16Exit();

	BurnYM2203Exit();
	BurnYM2151Exit();
	MSM6295Exit(0);
	MSM6295Exit(1);

	SekExit();

	if (AllMem) {
		free (AllMem);
		AllMem = NULL;
	}

	MSM6295ROM = NULL;

	return 0;
}

static void DrvPaletteRecalc()
{
	unsigned short *p0 = (unsigned short*)DrvPalRAM0;
	unsigned short *p1 = (unsigned short*)DrvPalRAM1;

	for (int i = 0; i < 0xa00 / 2; i++) {
		DrvPalette[i] = BurnHighCol(p0[i] & 0xff, p0[i] >> 8, p1[i]& 0xff, 0);
	}
}

static void draw_sprites(int pri)
{
	unsigned short *buffered_spriteram = (unsigned short*)DrvSprBuf;

	int priority_value = priority[1];

	for (int offs = 0; offs < 0x400; offs += 4)
	{
		int inc, mult;

		int y = buffered_spriteram[offs + 0];
		if ((y & 0x8000) == 0) continue;

		int sprite = buffered_spriteram[offs + 1] & 0x1fff;

		int x = buffered_spriteram[offs + 2];

		int colour = (x >> 12) & 0xf;

		if ( pri &&  (colour >= priority_value)) continue;
		if (!pri && !(colour >= priority_value)) continue;

		if ((x & 0x0800) && (nCurrentFrame & 1)) continue; // flash

		int fx = y & 0x2000;
		int fy = y & 0x4000;
		int multi = (1 << ((y & 0x1800) >> 11)) - 1;

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 256) x -= 512;
		if (y >= 256) y -= 512;
		x = 240 - x;
		y = 240 - y;

		if (x > 256) continue;

		sprite &= ~multi;
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

		if (*flipscreen)
		{
			y = 240 - y;
			x = 240 - x;
			if (fx) fx = 0; else fx = 1;
			if (fy) fy = 0; else fy = 1;
			mult = 16;
		}
		else mult = -16;

		while (multi >= 0)
		{
			if (fy) {
				if (fx) {
					Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, sprite - multi * inc, x, (y + mult * multi) - 8, colour, 4, 0, 0x100, DrvGfxROM3);
				} else {
					Render16x16Tile_Mask_FlipY_Clip(pTransDraw, sprite - multi * inc, x, (y + mult * multi) - 8, colour, 4, 0, 0x100, DrvGfxROM3);
				}
			} else {
				if (fx) {
					Render16x16Tile_Mask_FlipX_Clip(pTransDraw, sprite - multi * inc, x, (y + mult * multi) - 8, colour, 4, 0, 0x100, DrvGfxROM3);
				} else {
					Render16x16Tile_Mask_Clip(pTransDraw, sprite - multi * inc, x, (y + mult * multi) - 8, colour, 4, 0, 0x100, DrvGfxROM3);
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
	deco16_pf34_update();

	int layer_prio = priority[0] & 0x03;
	int layer_order[4][4] = { { 3, 2, 1, 0 }, { 2, 3, 1, 0 }, { 3, 1, 2, 0 }, { 2, 1, 3, 0 } };

	deco16_draw_layer(layer_order[layer_prio][0], pTransDraw, DECO16_LAYER_OPAQUE);
	deco16_draw_layer(layer_order[layer_prio][1], pTransDraw, 0);
	draw_sprites(0);
	deco16_draw_layer(layer_order[layer_prio][2], pTransDraw, 0);
	draw_sprites(1);
	deco16_draw_layer(layer_order[layer_prio][3], pTransDraw, 0);

	BurnTransferCopy(DrvPalette);

	return 0;
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	{
		memset (DrvInputs, 0xff, 2 * sizeof(short)); 
		for (int i = 0; i < 16; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	int nInterleave = 256;
	int nCyclesTotal[2] = { 12000000 / 58, 8055000 / 58 };
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
		BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(1, pBurnSoundOut, nBurnSoundLen);
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

		BurnYM2203Scan(nAction, pnMin);
		BurnYM2151Scan(nAction);
		MSM6295Scan(0, nAction);
		MSM6295Scan(1, nAction);
	}

	return 0;
}


// Vapor Trail - Hyper Offence Formation (World revision 1)

static struct BurnRomInfo vaportraRomDesc[] = {
	{ "fl_02-1.bin",	0x20000, 0x9ae36095, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "fl_00-1.bin",	0x20000, 0xc08cc048, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "fl_03.bin",		0x20000, 0x80bd2844, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "fl_01.bin",		0x20000, 0x9474b085, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "fj04",		0x10000, 0xe9aedf9b, 2 | BRF_PRG | BRF_ESS }, //  4 Huc6280 Code

	{ "vtmaa00.bin",	0x80000, 0x0330e13b, 3 | BRF_GRA },           //  5 Characters and Foreground Tiles

	{ "vtmaa01.bin",	0x80000, 0xc217a31b, 4 | BRF_GRA },           //  7
	{ "vtmaa02.bin",	0x80000, 0x091ff98e, 4 | BRF_GRA },           //  6 Background Tiles

	{ "vtmaa04.bin",	0x80000, 0xb713e9cc, 5 | BRF_GRA },           //  9
	{ "vtmaa03.bin",	0x80000, 0x1a30bf81, 5 | BRF_GRA },           //  8 Sprites

	{ "fj06",		0x20000, 0x6e98a235, 6 | BRF_SND },           // 10 OKI M6295 Samples 0

	{ "fj05",		0x20000, 0x39cda2b5, 7 | BRF_SND },           // 11 OKI M6295 Samples 1

	{ "fj-27.bin",		0x00200, 0x65045742, 8 | BRF_OPT },           // 12 Unknown PROMs

	{ "pal16l8a.6l",	0x00104, 0xee748e8f, 9 | BRF_OPT },           // 13 PLDs
	{ "pal16l8b.13g",	0x00104, 0x6da13bda, 9 | BRF_OPT },           // 14
	{ "pal16l8b.13h",	0x00104, 0x62a9e098, 9 | BRF_OPT },           // 15
	{ "pal16l8b.14g",	0x00104, 0x036768aa, 9 | BRF_OPT },           // 16
	{ "pal16l8b.14h",	0x00104, 0xbf421fce, 9 | BRF_OPT },           // 17
};

STD_ROM_PICK(vaportra)
STD_ROM_FN(vaportra)

static int VaportraInit()
{
	return DrvInit(0);
}

struct BurnDriver BurnDrvVaportra = {
	"vaportra", NULL, NULL, NULL, "1989",
	"Vapor Trail - Hyper Offence Formation (World revision 1)\0", "No sound", "Data East Corporation", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, NULL, vaportraRomInfo, vaportraRomName, NULL, NULL, VaportraInputInfo, VaportraDIPInfo,
	VaportraInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x500,
	240, 256, 3, 4
};


// Vapor Trail - Hyper Offence Formation (World revision 3)

static struct BurnRomInfo vaportraw3RomDesc[] = {
	{ "fl02-3.bin",		0x20000, 0x6c59be54, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "fl00-3.bin",		0x20000, 0x69f8bef4, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "fl_03.bin",		0x20000, 0x80bd2844, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "fl_01.bin",		0x20000, 0x9474b085, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "fj04",			0x10000, 0xe9aedf9b, 2 | BRF_PRG | BRF_ESS }, //  4 Huc6280 Code

	{ "fl23",			0x20000, 0x6089f9e7, 3 | BRF_GRA },           //  5 Characters and Foreground Tiles
	{ "fl25",			0x20000, 0x3989290a, 3 | BRF_GRA },           //  6
	{ "fl24",			0x20000, 0x41551bfa, 3 | BRF_GRA },           //  7
	{ "fl26",			0x20000, 0xdc67fa5c, 3 | BRF_GRA },           //  8

	{ "vtmaa01.bin",	0x80000, 0xc217a31b, 4 | BRF_GRA },           //  9 Background Tiles
	{ "vtmaa02.bin",	0x80000, 0x091ff98e, 4 | BRF_GRA },           // 10

	{ "vtmaa04.bin",	0x80000, 0xb713e9cc, 5 | BRF_GRA },           // 11 Sprites
	{ "vtmaa03.bin",	0x80000, 0x1a30bf81, 5 | BRF_GRA },           // 12

	{ "fj06",			0x20000, 0x6e98a235, 6 | BRF_SND },           // 13 OKI M6295 Samples 0

	{ "fj05",			0x20000, 0x39cda2b5, 7 | BRF_SND },           // 14 OKI M6295 Samples 1

	{ "fj-27.bin",		0x00200, 0x65045742, 8 | BRF_OPT },           // 15 Unknown PROMs
	
	{ "pal16l8a.6l",	0x00104, 0xee748e8f, 9 | BRF_OPT },           // 13 PLDs
	{ "pal16l8b.13g",	0x00104, 0x6da13bda, 9 | BRF_OPT },           // 14
	{ "pal16l8b.13h",	0x00104, 0x62a9e098, 9 | BRF_OPT },           // 15
	{ "pal16l8b.14g",	0x00104, 0x036768aa, 9 | BRF_OPT },           // 16
	{ "pal16l8b.14h",	0x00104, 0xbf421fce, 9 | BRF_OPT },           // 17
};

STD_ROM_PICK(vaportraw3)
STD_ROM_FN(vaportraw3)

static int Vaportraw3Init()
{
	return DrvInit(1);
}

struct BurnDriver BurnDrvVaportraw3 = {
	"vaportra3", "vaportra", NULL, NULL, "1989",
	"Vapor Trail - Hyper Offence Formation (World revision 3)\0", "No sound", "Data East Corporation", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, NULL, vaportraw3RomInfo, vaportraw3RomName, NULL, NULL, VaportraInputInfo, VaportraDIPInfo,
	Vaportraw3Init, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x500,
	240, 256, 3, 4
};


// Vapor Trail - Hyper Offence Formation (US)

static struct BurnRomInfo vaportrauRomDesc[] = {
	{ "fj02",		0x20000, 0xa2affb73, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "fj00",		0x20000, 0xef05e07b, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "fj03",		0x20000, 0x44893379, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "fj01",		0x20000, 0x97fbc107, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "fj04",		0x10000, 0xe9aedf9b, 2 | BRF_PRG | BRF_ESS }, //  4 Huc6280 Code

	{ "vtmaa00.bin",	0x80000, 0x0330e13b, 3 | BRF_GRA },           //  5 Characters and Foreground Tiles

	{ "vtmaa01.bin",	0x80000, 0xc217a31b, 4 | BRF_GRA },           //  7
	{ "vtmaa02.bin",	0x80000, 0x091ff98e, 4 | BRF_GRA },           //  6 Background Tiles

	{ "vtmaa04.bin",	0x80000, 0xb713e9cc, 5 | BRF_GRA },           //  9
	{ "vtmaa03.bin",	0x80000, 0x1a30bf81, 5 | BRF_GRA },           //  8 Sprites

	{ "fj06",		0x20000, 0x6e98a235, 6 | BRF_SND },           // 10 OKI M6295 Samples 0

	{ "fj05",		0x20000, 0x39cda2b5, 7 | BRF_SND },           // 11 OKI M6295 Samples 1

	{ "fj-27.bin",		0x00200, 0x65045742, 8 | BRF_OPT },           // 12 Unknown PROMs

	{ "pal16l8a.6l",	0x00104, 0xee748e8f, 9 | BRF_OPT },           // 13 PLDs
	{ "pal16l8b.13g",	0x00104, 0x6da13bda, 9 | BRF_OPT },           // 14
	{ "pal16l8b.13h",	0x00104, 0x62a9e098, 9 | BRF_OPT },           // 15
	{ "pal16l8b.14g",	0x00104, 0x036768aa, 9 | BRF_OPT },           // 16
	{ "pal16l8b.14h",	0x00104, 0xbf421fce, 9 | BRF_OPT },           // 17
};

STD_ROM_PICK(vaportrau)
STD_ROM_FN(vaportrau)

struct BurnDriver BurnDrvVaportrau = {
	"vaportrau", "vaportra", NULL, NULL, "1989",
	"Vapor Trail - Hyper Offence Formation (US)\0", "No sound", "Data East USA", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, NULL, vaportrauRomInfo, vaportrauRomName, NULL, NULL, VaportraInputInfo, VaportraDIPInfo,
	VaportraInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x500,
	240, 256, 3, 4
};


// Kuhga - Operation Code 'Vapor Trail' (Japan revision 3)

static struct BurnRomInfo kuhgaRomDesc[] = {
	{ "fp02-3.bin",		0x20000, 0xd0705ef4, 1 | BRF_PRG | BRF_ESS }, //  0 68k Code
	{ "fp00-3.bin",		0x20000, 0x1da92e48, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "fp03.bin",		0x20000, 0xea0da0f1, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "fp01.bin",		0x20000, 0xe3ecbe86, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "fj04",		0x10000, 0xe9aedf9b, 2 | BRF_PRG | BRF_ESS }, //  4 Huc6280 Code

	{ "vtmaa00.bin",	0x80000, 0x0330e13b, 3 | BRF_GRA },           //  5 Characters and Foreground Tiles

	{ "vtmaa01.bin",	0x80000, 0xc217a31b, 4 | BRF_GRA },           //  7
	{ "vtmaa02.bin",	0x80000, 0x091ff98e, 4 | BRF_GRA },           //  6 Background Tiles

	{ "vtmaa04.bin",	0x80000, 0xb713e9cc, 5 | BRF_GRA },           //  9
	{ "vtmaa03.bin",	0x80000, 0x1a30bf81, 5 | BRF_GRA },           //  8 Sprites

	{ "fj06",		0x20000, 0x6e98a235, 6 | BRF_SND },           // 10 OKI M6295 Samples 0

	{ "fj05",		0x20000, 0x39cda2b5, 7 | BRF_SND },           // 11 OKI M6295 Samples 1

	{ "fj-27.bin",		0x00200, 0x65045742, 8 | BRF_OPT },           // 12 Unknown PROMs
};

STD_ROM_PICK(kuhga)
STD_ROM_FN(kuhga)

struct BurnDriver BurnDrvKuhga = {
	"kuhga", "vaportra", NULL, NULL, "1989",
	"Kuhga - Operation Code 'Vapor Trail' (Japan revision 3)\0", "No sound", "Data East Corporation", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, NULL, kuhgaRomInfo, kuhgaRomName, NULL, NULL, VaportraInputInfo, VaportraDIPInfo,
	VaportraInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x500,
	240, 256, 3, 4
};
