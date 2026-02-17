#pragma once

#include <wtypes.h>
constexpr const double PI = 3.141592653589793;

// ============================================================
// ■ 常数
// ============================================================
const int			BOARD_WIDTH = 800;
const int			BOARD_HEIGHT = 600;
const int			WIDE_BOARD_WIDTH = 800;
const int			BOARD_OFFSET = 220;
const int			BOARD_EDGE = -100;
const int			BOARD_IMAGE_WIDTH_OFFSET = 1180;
const int           BOARD_ICE_START = 800;
const int           LAWN_XMIN = 40;
const int           LAWN_YMIN = 80;
const int           HIGH_GROUND_HEIGHT = 30;

const int           SEEDBANK_MAX = 10;
const int           SEED_BANK_OFFSET_X = 0;
const int           SEED_BANK_OFFSET_X_END = 10;
const int           SEED_CHOOSER_OFFSET_Y = 516;
const int           SEED_PACKET_WIDTH = 50;
const int           SEED_PACKET_HEIGHT = 70;
const int           IMITATER_DIALOG_WIDTH = 500;
const int           IMITATER_DIALOG_HEIGHT = 600;

const int			WIDESCREEN_OFFSETX = -240;
const int			WIDESCREEN_OFFSETY = -60;

const int			STREET_ZOMBIE_START_X = 1030; // PC: 830 - Console : 1000
const int			STREET_ZOMBIE_ROOF_START_X = 900;
const int			STREET_ZOMBIE_START_Y = 70;
const int			STREET_ZOMBIE_GRID_SIZE_X = 30; // PC : 56 - Console : 30
const int			STREET_ZOMBIE_GRID_SIZE_Y = 90;
const int			STREET_ZOMBIE_ROOF_OFFSET = 30; // PC : 30 - Console : 15

// ============================================================
// ■ 关卡相关
// ============================================================
const int			ADVENTURE_AREAS = 5;
const int			LEVELS_PER_AREA = 10;
const int           NUM_LEVELS = ADVENTURE_AREAS * LEVELS_PER_AREA;
const int			FINAL_LEVEL = NUM_LEVELS;
const int           FLAG_RAISE_TIME = 100;
const int           LAST_STAND_FLAGS = 5;
const int           ZOMBIE_COUNTDOWN_FIRST_WAVE = 1800;
const int           ZOMBIE_COUNTDOWN = 2500;
const int           ZOMBIE_COUNTDOWN_RANGE = 600;
const int           ZOMBIE_COUNTDOWN_BEFORE_FLAG = 4500;
const int           ZOMBIE_COUNTDOWN_BEFORE_REPICK = 5499;
const int           ZOMBIE_COUNTDOWN_MIN = 400;
const int           FOG_BLOW_RETURN_TIME = 2000;
const int           SUN_COUNTDOWN = 425;
const int           SUN_COUNTDOWN_RANGE = 275;
const int           SUN_COUNTDOWN_MAX = 950;
const int           SURVIVAL_NORMAL_FLAGS = 5;
const int           SURVIVAL_HARD_FLAGS = 10;

// ============================================================
// ■ 商店相关
// ============================================================
const int           STORESCREEN_ITEMOFFSET_1_X = 422;
const int           STORESCREEN_ITEMOFFSET_1_Y = 206;
const int           STORESCREEN_ITEMOFFSET_2_X = 372;
const int           STORESCREEN_ITEMOFFSET_2_Y = 310;
const int           STORESCREEN_ITEMSIZE = 74;
const int           STORESCREEN_COINBANK_X = 650;
const int           STORESCREEN_COINBANK_Y = 559;
const int           STORESCREEN_PAGESTRING_X = 470;
const int           STORESCREEN_PAGESTRING_Y = 500;

// ============================================================
// ■ REMOVE THE // from the defines below to ENABLE them or INSERT // to DISABLE the feature.
// ============================================================

#ifdef _GOTY
#define _HAS_NEW_DANCERS
#define _HAS_ACHIEVEMENTS
#define _HAS_ZOMBATAR
#define _HAS_UNLOCK
// Unused or Restored
//#define _HAS_LEVELSELECTOR 
//#define _HAS_MORESCREEN
#endif

#ifdef _DEBUG
#define _SHOW_OUTPUT_CONSOLE
#define _SHOW_LIMBO_PAGE
#endif

// Resources
#define _ALLOW_RESOURCE_PACKS

// Unused or Restored
//#define _HAS_BLOOM_AND_DOOM_CONTENTS
#define _HAS_EXTENDED_MINIGAMES
//#define _HAS_UNUSED_ACHIEVEMENTS

//Ported from other editions
#define _MOBILE_MINIGAMES
#define _DS_MINIGAMES
//#define _CONSOLE_MINIGAMES
#define _REPLANTED_SPEED_CONTROL
//#define _HAS_NEW_GIGA_ZOMBIES
//#define _HAS_SCORE_SYSTEM

// Quality Of Life
#define _HAS_HEALTHBAR_TOGGLE
//#define _ALLOW_SWIPE
//#define _HAS_GAMESELECTOR_SPOTLIGHT
#define _HAS_ANIMATED_WOOD_SIGN
#define _HAS_KERNELPULT_BUTTER_IDLE

// Quirky stuff
//#define _HAS_ROOF_SLOPE_ANGLE

// Rebalancing
//#define _PIERCING_CACTUS
//#define _SPLASH_SNOWPEA

#include "ConstEnums.h" // Include the enums at the end to properly setup the entries related to the defines