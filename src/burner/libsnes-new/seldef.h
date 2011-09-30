// Selector definition, which will be used by other module, added by regret

#define MASKMISC		(1 << (HARDWARE_PREFIX_MISC		>> 24))
#define MASKCPS			(1 << (HARDWARE_PREFIX_CPS1		>> 24))
#define MASKNEOGEO		(1 << (HARDWARE_PREFIX_SNK		>> 24))
#define MASKSEGA		(1 << (HARDWARE_PREFIX_SEGA		>> 24))
#define MASKTOAPLAN		(1 << (HARDWARE_PREFIX_TOAPLAN	>> 24))
#define MASKCAVE		(1 << (HARDWARE_PREFIX_CAVE		>> 24))
#define MASKCPS2		(1 << (HARDWARE_PREFIX_CPS2		>> 24))
#define MASKMD			(1 << (HARDWARE_PREFIX_SEGAMD	>> 24))
#define MASKPGM			(1 << (HARDWARE_PREFIX_PGM		>> 24))
#define MASKCPS3		(1 << (HARDWARE_PREFIX_CPS3		>> 24))
#define MASKTAITO		(1 << (HARDWARE_PREFIX_TAITO	>> 24))
#define MASKPSIKYO		(1 << (HARDWARE_PREFIX_PSIKYO	>> 24))
#define MASKKANEKO16	(1 << (HARDWARE_PREFIX_KANEKO16	>> 24))
#define MASKKONAMI		(1 << (HARDWARE_PREFIX_KONAMI	>> 24))
#define MASKPACMAN		(1 << (HARDWARE_PREFIX_PACMAN	>> 24))
#define MASKGALAXIAN	(1 << (HARDWARE_PREFIX_GALAXIAN >> 24))
#define MASKATARI		(1 << (HARDWARE_PREFIX_ATARI	>> 24))

#define MASKALL \
	(MASKMISC | MASKCPS | MASKNEOGEO | MASKSEGA | MASKTOAPLAN \
	| MASKCAVE | MASKCPS2 | MASKMD | MASKPGM | MASKCPS3 \
	| MASKTAITO | MASKPSIKYO | MASKKANEKO16 | MASKKONAMI | MASKPACMAN \
	| MASKGALAXIAN | MASKATARI)

#define AVAILONLY		(1 << 0)
#define AUTOEXPAND		(1 << 1)
#define SHOWSHORT		(1 << 2)
#define ASCIIONLY		(1 << 3)
#define MANGLENAME		(1 << 4)
#define SHOWFAVORITE	(1 << 5)
#define TRANSLANG		(1 << 6)
#define NOICON			(1 << 7)
#define KEEPGAMEASPECT	(1 << 8)
#define CHECK7ZIP		(1 << 9)
#define DISABLECRC		(1 << 10)
