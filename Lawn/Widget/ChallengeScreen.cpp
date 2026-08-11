#include "../../GameConstants.h"
#include "GameButton.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "ChallengeScreen.h"
#include "../../Resources.h"
#include "../ToolTipWidget.h"
#include "../System/PlayerInfo.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/Debug.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "ImageScrollbar.h"

ChallengeDefinition gChallengeDefs[NUM_CHALLENGE_MODES] = {
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1,              0,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    0,  0,  _S("[SURVIVAL_DAY_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_2,              1,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    0,  1,  _S("[SURVIVAL_NIGHT_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_3,              2,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    0,  2,  _S("[SURVIVAL_POOL_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_4,              3,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    0,  3,  _S("[SURVIVAL_FOG_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_5,              4,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    0,  4,  _S("[SURVIVAL_ROOF_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_1,                5,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    1,  0,  _S("[SURVIVAL_DAY_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_2,                6,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    1,  1,  _S("[SURVIVAL_NIGHT_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_3,                7,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    1,  2,  _S("[SURVIVAL_POOL_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_4,                8,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    1,  3,  _S("[SURVIVAL_FOG_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_5,                9,   ChallengePage::CHALLENGE_PAGE_SURVIVAL,    1,  4,  _S("[SURVIVAL_ROOF_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_1,             10,  ChallengePage::CHALLENGE_PAGE_SURVIVAL,	   2,  0,  _S("[SURVIVAL_DAY_ENDLESS]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_2,             11,  ChallengePage::CHALLENGE_PAGE_SURVIVAL,    2,  1,  _S("[SURVIVAL_NIGHT_ENDLESS]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_3,             12,  ChallengePage::CHALLENGE_PAGE_SURVIVAL,    2,  2,  _S("[SURVIVAL_POOL_ENDLESS]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_4,             13,  ChallengePage::CHALLENGE_PAGE_SURVIVAL,    2,  3,  _S("[SURVIVAL_FOG_ENDLESS]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_5,             14,  ChallengePage::CHALLENGE_PAGE_SURVIVAL,    2,  4,  _S("[SURVIVAL_ROOF_ENDLESS]") },
	{ GameMode::GAMEMODE_CHALLENGE_WAR_AND_PEAS,               0,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   0,  0,  _S("[WAR_AND_PEAS]") },
	{ GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING,            6,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   0,  1,  _S("[WALL_NUT_BOWLING]") },
	{ GameMode::GAMEMODE_CHALLENGE_SLOT_MACHINE,               2,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   0,  2,  _S("[SLOT_MACHINE]") },
	{ GameMode::GAMEMODE_CHALLENGE_RAINING_SEEDS,              3,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   0,  3,  _S("[ITS_RAINING_SEEDS]") },
	{ GameMode::GAMEMODE_CHALLENGE_BEGHOULED,                  1,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   0,  4,  _S("[BEGHOULED]") },
	{ GameMode::GAMEMODE_CHALLENGE_INVISIGHOUL,                8,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   1,  0,  _S("[INVISIGHOUL]") },
	{ GameMode::GAMEMODE_CHALLENGE_SEEING_STARS,               5,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   1,  1,  _S("[SEEING_STARS]") },
	{ GameMode::GAMEMODE_CHALLENGE_ZOMBIQUARIUM,               7,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   1,  2,  _S("[ZOMBIQUARIUM]") },
	{ GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST,            20,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   1,  3,  _S("[BEGHOULED_TWIST]") },
	{ GameMode::GAMEMODE_CHALLENGE_LITTLE_TROUBLE,             12,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   1,  4,  _S("[LITTLE_TROUBLE]") },
	{ GameMode::GAMEMODE_CHALLENGE_PORTAL_COMBAT,              15,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   2,  0,  _S("[PORTAL_COMBAT]") },
	{ GameMode::GAMEMODE_CHALLENGE_COLUMN,                     4,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   2,  1,  _S("[COLUMN_AS_YOU_SEE_EM]") },
	{ GameMode::GAMEMODE_CHALLENGE_BOBSLED_BONANZA,            17,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   2,  2,  _S("[BOBSLED_BONANZA]") },
	{ GameMode::GAMEMODE_CHALLENGE_SPEED,                      18,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   2,  3,  _S("[ZOMBIES_ON_SPEED]") },
	{ GameMode::GAMEMODE_CHALLENGE_WHACK_A_ZOMBIE,             16,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   2,  4,  _S("[WHACK_A_ZOMBIE]") },
	{ GameMode::GAMEMODE_CHALLENGE_LAST_STAND,                 21,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   3,  0,  _S("[LAST_STAND]") },
	{ GameMode::GAMEMODE_CHALLENGE_WAR_AND_PEAS_2,             0,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   3,  1,  _S("[WAR_AND_PEAS_2]") },
	{ GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING_2,          6,   ChallengePage::CHALLENGE_PAGE_CHALLENGE,   3,  2,  _S("[WALL_NUT_BOWLING_EXTREME]") },
	{ GameMode::GAMEMODE_CHALLENGE_POGO_PARTY,                 14,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   3,  3,  _S("[POGO_PARTY]") },
	{ GameMode::GAMEMODE_CHALLENGE_FINAL_BOSS,                 19,  ChallengePage::CHALLENGE_PAGE_CHALLENGE,   3,  4,  _S("[FINAL_BOSS]") },
	{ GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_WALLNUT,      0,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       0,  0,  _S("[ART_CHALLENGE_WALL_NUT]") },
	{ GameMode::GAMEMODE_CHALLENGE_SUNNY_DAY,                  1,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       0,  1,  _S("[SUNNY_DAY]") },
	{ GameMode::GAMEMODE_CHALLENGE_RESODDED,                   2,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       0,  2,  _S("[UNSODDED]") },
	{ GameMode::GAMEMODE_CHALLENGE_BIG_TIME,                   3,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       0,  3,  _S("[BIG_TIME]") },
	{ GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_SUNFLOWER,    4,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       0,  4,  _S("[ART_CHALLENGE_SUNFLOWER]") },
	{ GameMode::GAMEMODE_CHALLENGE_AIR_RAID,                   5,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       1,  0,  _S("[AIR_RAID]") },
	{ GameMode::GAMEMODE_CHALLENGE_ICE,                        6,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       1,  1,  _S("[ICE_LEVEL]") },
	{ GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN,                 7,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       1,  2,  _S("[ZEN_GARDEN]") },
	{ GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY,               8,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       1,  3,  _S("[HIGH_GRAVITY]") },
	{ GameMode::GAMEMODE_CHALLENGE_GRAVE_DANGER,               11,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       1,  4,  _S("[GRAVE_DANGER]") },
	{ GameMode::GAMEMODE_CHALLENGE_SHOVEL,                     10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       2,  0,  _S("[CAN_YOU_DIG_IT]") },
	{ GameMode::GAMEMODE_CHALLENGE_STORMY_NIGHT,               13,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       2,  1,  _S("[DARK_STORMY_NIGHT]") },
	{ GameMode::GAMEMODE_CHALLENGE_BUNGEE_BLITZ,               9,   ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       2,  2,  _S("[BUNGEE_BLITZ]") },
	{ GameMode::GAMEMODE_CHALLENGE_SQUIRREL,                   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       2,  3,  _S("Squirrel") },
	{ GameMode::GAMEMODE_TREE_OF_WISDOM,                       10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       2,  4,  _S("Tree Of Wisdom") },
	{ GameMode::GAMEMODE_SCARY_POTTER_1,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      0,  0,  _S("[SCARY_POTTER_1]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_2,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      0,  1,  _S("[SCARY_POTTER_2]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_3,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      0,  2,  _S("[SCARY_POTTER_3]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_4,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      0,  3,  _S("[SCARY_POTTER_4]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_5,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      0,  4,  _S("[SCARY_POTTER_5]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_6,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      1,  0,  _S("[SCARY_POTTER_6]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_7,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      1,  1,  _S("[SCARY_POTTER_7]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_8,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      1,  2,  _S("[SCARY_POTTER_8]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_9,                       10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      1,  3,  _S("[SCARY_POTTER_9]") },
	{ GameMode::GAMEMODE_SCARY_POTTER_ENDLESS,                 10,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      1,  4,  _S("[SCARY_POTTER_ENDLESS]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      2,  0,  _S("[I_ZOMBIE_1]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_2,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      2,  1,  _S("[I_ZOMBIE_2]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_3,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      2,  2,  _S("[I_ZOMBIE_3]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_4,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      2,  3,  _S("[I_ZOMBIE_4]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_5,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      2,  4,  _S("[I_ZOMBIE_5]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_6,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      3,  0,  _S("[I_ZOMBIE_6]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_7,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      3,  1,  _S("[I_ZOMBIE_7]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_8,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      3,  2,  _S("[I_ZOMBIE_8]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_9,                    11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      3,  3,  _S("[I_ZOMBIE_9]") },
	{ GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS,              11,  ChallengePage::CHALLENGE_PAGE_PUZZLE,      3,  4,  _S("[I_ZOMBIE_ENDLESS]") },
	{ GameMode::GAMEMODE_UPSELL,                               10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       3,  0,  _S("Upsell") },
	{ GameMode::GAMEMODE_INTRO,                                10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       3,  1,  _S("Intro") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_6,				4,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 0,  0,  _S("[SURVIVAL_NIGHTROOF_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_HIGHGROUND,      1,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 0,  1,  _S("[SURVIVAL_HIGHGROUND_NORMAL]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_6,					9,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 1,  0,  _S("[SURVIVAL_NIGHTROOF_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_HIGHGROUND,		6,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 1,  1,  _S("[SURVIVAL_HIGHGROUND_HARD]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_6,			   14,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 2,  0,  _S("[SURVIVAL_NIGHTROOF_ENDLESS]") },
	{ GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_HIGHGROUND,    11,   ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL,		 2,  1,  _S("[SURVIVAL_HIGHGROUND_ENDLESS]") },
	{ GameMode::GAMEMODE_LAST_STAND_STAGE_1,                   10,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,	0,  0,  _S("[LAST_STAND_DAY]") },
	{ GameMode::GAMEMODE_LAST_STAND_STAGE_2,                   11,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   0,  1,  _S("[LAST_STAND_NIGHT]") },
	{ GameMode::GAMEMODE_LAST_STAND_STAGE_3,                   12,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   0,  2,  _S("[LAST_STAND_POOL]") },
	{ GameMode::GAMEMODE_LAST_STAND_STAGE_4,                   13,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   0,  3,  _S("[LAST_STAND_FOG]") },
	{ GameMode::GAMEMODE_LAST_STAND_STAGE_5,                   14,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   0,  4,  _S("[LAST_STAND_ROOF]") },
	{ GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_1,           10,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,	1,  0,  _S("[LAST_STAND_DAY_ENDLESS]") },
	{ GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_2,           11,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   1,  1,  _S("[LAST_STAND_NIGHT_ENDLESS]") },
	{ GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_3,           12,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   1,  2,  _S("[LAST_STAND_POOL_ENDLESS]") },
	{ GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_4,           13,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   1,  3,  _S("[LAST_STAND_FOG_ENDLESS]") },
	{ GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_5,           14,  ChallengePage::CHALLENGE_PAGE_LAST_STAND,   1,  4,  _S("[LAST_STAND_ROOF_ENDLESS]") },
#ifdef _MOBILE_MINIGAMES
	{ GameMode::GAMEMODE_CHALLENGE_VASEBREAKER,				   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       3,  2,  _S("[VASE_BREAKER]") },
	{ GameMode::GAMEMODE_CHALLENGE_BUTTERED_POPCORN,		   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       3,  3,  _S("[BUTTERED_POPCORN]") },
#endif
#ifdef _DS_MINIGAMES
	{ GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE,				   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       3,  4,  _S("[HEAT_WAVE]") },
	{ GameMode::GAMEMODE_CHALLENGE_BOMB_ALL_TOGETHER,		   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       4,  0,  _S("[BOMB_ALL_TOGETHER]") },
	{ GameMode::GAMEMODE_CHALLENGE_HOMERUN_DERBY,			   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       4,  1,  _S("[HOMERUN_DERBY]") },
	{ GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP,				   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       4,  2,  _S("[ZOMBIE_TRAP]") },
#endif
#ifdef _CONSOLE_MINIGAMES
	{ GameMode::GAMEMODE_CHALLENGE_HEAVY_WEAPON,			   10,  ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE,       4,  3,  _S("[HEAVY_WEAPON]") },
#endif
};

//0x42DAE0
ChallengeScreen::ChallengeScreen(LawnApp* theApp, ChallengePage thePage)
{
	mLockShakeX = 0;
	mLockShakeY = 0;
	mPageIndex = thePage;
	mApp = theApp;
	mClip = false;
	mCheatEnableChallenges = false;
	mUnlockState = UNLOCK_OFF;
	mUnlockChallengeIndex = -1;
	mUnlockStateCounter = 0;
	TodLoadResources("DelayLoad_ChallengeScreen");

	mBackButton = MakeNewButton(ChallengeScreen::ChallengeScreen_Back, this, _S("[BACK_TO_MENU]"), nullptr, Sexy::IMAGE_SEEDCHOOSER_BUTTON2, 
		Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
	mBackButton->mTextDownOffsetX = 1;
	mBackButton->mTextDownOffsetY = 1;
	mBackButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
	mBackButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
	mBackButton->Resize(18, 568, 111, 26);

	for (int aPageIdx = CHALLENGE_PAGE_SURVIVAL; aPageIdx < MAX_CHALLANGE_PAGES; aPageIdx++)
	{
		ButtonWidget* aPageButton = new ButtonWidget(ChallengeScreen::ChallengeScreen_Page + aPageIdx, this);
		aPageButton->mDoFinger = true;
		mPageButton[aPageIdx] = aPageButton;
		if (aPageIdx == ChallengePage::CHALLENGE_PAGE_CHALLENGE)
			aPageButton->mLabel = TodStringTranslate(_S("[CHALLENGE_PAGE]"));
		else if (aPageIdx == CHALLENGE_PAGE_SURVIVAL)
			aPageButton->mLabel = TodStringTranslate(_S("[SURVIVAL_PAGE]"));
		else if (aPageIdx == CHALLENGE_PAGE_PUZZLE)
			aPageButton->mLabel = TodStringTranslate(_S("[PUZZLE_PAGE]"));
		else if (aPageIdx == CHALLENGE_PAGE_LIMBO_CHALLENGE || aPageIdx == CHALLENGE_PAGE_LIMBO_SURVIVAL)
			aPageButton->mLabel = TodStringTranslate(_S("[LIMBO_PAGE]"));
		else if (aPageIdx == CHALLENGE_PAGE_LAST_STAND)
			aPageButton->mLabel = TodStringTranslate(_S("[LAST_STAND_PAGE]"));
		else
			aPageButton->mLabel = TodReplaceNumberString(_S("[PAGE_X]"), _S("{PAGE}"), aPageIdx);
		aPageButton->mButtonImage = Sexy::IMAGE_BLANK;
		aPageButton->mOverImage = Sexy::IMAGE_BLANK;
		aPageButton->mDownImage = Sexy::IMAGE_BLANK;
		aPageButton->SetFont(Sexy::FONT_BRIANNETOD12);
		aPageButton->mColors[ButtonWidget::COLOR_LABEL] = Color(255, 240, 0);
		aPageButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(220, 220, 0);

		int pageOffset = 0;
		if (thePage == ChallengePage::CHALLENGE_PAGE_CHALLENGE && (aPageIdx == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE || aPageIdx == ChallengePage::CHALLENGE_PAGE_CHALLENGE))
			pageOffset += 2;
		if (thePage == ChallengePage::CHALLENGE_PAGE_PUZZLE && (aPageIdx == ChallengePage::CHALLENGE_PAGE_LAST_STAND || thePage == ChallengePage::CHALLENGE_PAGE_PUZZLE))
			pageOffset += 4;
		aPageButton->Resize(200 + 100 * (aPageIdx - pageOffset), 540, 100, 75);

		//if (!ShowPageButtons() || aPageIdx == CHALLENGE_PAGE_SURVIVAL || aPageIdx == CHALLENGE_PAGE_PUZZLE)

		aPageButton->mVisible = false;

		if (mApp->mPlayerInfo && mApp->mPlayerInfo->mHasUsedCheatKeys)
		{
			if ((thePage == ChallengePage::CHALLENGE_PAGE_CHALLENGE && (aPageIdx == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE || aPageIdx == ChallengePage::CHALLENGE_PAGE_CHALLENGE) ||
				thePage == ChallengePage::CHALLENGE_PAGE_SURVIVAL && (aPageIdx == ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL || aPageIdx == ChallengePage::CHALLENGE_PAGE_SURVIVAL)))
				aPageButton->mVisible = true;
		}


#ifdef _HAS_EXTENDED_MINIGAMES
		if (thePage == ChallengePage::CHALLENGE_PAGE_PUZZLE && (aPageIdx == ChallengePage::CHALLENGE_PAGE_LAST_STAND || aPageIdx == ChallengePage::CHALLENGE_PAGE_PUZZLE))
			aPageButton->mVisible = true;
#endif

	}
	
	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition& aChlDef = GetChallengeDefinition(aChallengeMode);
		ButtonWidget* aChallengeButton = new ButtonWidget(ChallengeScreen::ChallengeScreen_Mode + aChallengeMode, this);
		mChallengeButtons[aChallengeMode] = aChallengeButton;
		aChallengeButton->mDoFinger = true;
		aChallengeButton->mFrameNoDraw = true;
		
		int aColumn = aChlDef.mCol;
		int aRow = aChlDef.mRow;
		if (aChlDef.mPage == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE && aChlDef.mChallengeMode >= GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
		{
			int newColumn = aChlDef.mCol - 1;
			aColumn = newColumn % 4;
			if (newColumn < 0) 
			{
				aRow--;
				aColumn = 4;
			}
		}

		if (aChlDef.mPage == CHALLENGE_PAGE_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_LIMBO_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_PUZZLE)
			aChallengeButton->Resize(38 + aColumn * 155, 93 + aRow * 119, 104, 115);
		else
			aChallengeButton->Resize(38 + aColumn * 155, 125 + aRow * 145, 104, 115);

		if (MoreTrophiesNeeded(aChallengeMode))
		{
			aChallengeButton->mDoFinger = false;
			aChallengeButton->mDisabled = true;
		}
	}

	mToolTip = new ToolTipWidget();
	mToolTip->mCenter = true;
	mToolTip->mVisible = false;

	mScrollbar = new ImageScrollbar(mApp, ScrollMode::SCROLLMODE_VERTICAL);
	mScrollbar->mOutAlpha = 0xFF;
	mScrollbar->SetView(14, 91, 771, 476);
	mScrollbar->Resize(770, 100, IMAGE_SCROLLOVERLAY->GetWidth()/2, IMAGE_SCROLLOVERLAY->GetHeight());
	mScrollbar->mThumbColor = Color(0x969696);
	mScrollbar->SetScrollRange(1000);
	mScrollbar->mBackGroundImage = Sexy::IMAGE_SCROLLBG;
	mScrollbar->mOverlayImage = Sexy::IMAGE_SCROLLOVERLAY;
	mScrollbar->mThumbPadX = 8.0f;
	mScrollbar->mThumbPadY = 8.0f;
	UpdateButtons();
	
	if (mApp->mGameMode != GAMEMODE_UPSELL || mApp->mGameScene != SCENE_LEVEL_INTRO)
		mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);

	bool aIsIZombie = false;
	if (mPageIndex == CHALLENGE_PAGE_SURVIVAL && mApp->mPlayerInfo->mHasNewSurvival)
	{
		SetUnlockChallengeIndex(mPageIndex, false);
		mApp->mPlayerInfo->mHasNewSurvival = false;
	}
	else if (mPageIndex == CHALLENGE_PAGE_CHALLENGE && mApp->mPlayerInfo->mHasNewMiniGame)
	{
		SetUnlockChallengeIndex(mPageIndex, false);
		mApp->mPlayerInfo->mHasNewMiniGame = false;
	}
	else if (mPageIndex == CHALLENGE_PAGE_PUZZLE)
	{
		if (mApp->mPlayerInfo->mHasNewScaryPotter)
		{
			SetUnlockChallengeIndex(mPageIndex, false);
			mApp->mPlayerInfo->mHasNewScaryPotter = false;
		}
		else if (mApp->mPlayerInfo->mHasNewIZombie)
		{
			SetUnlockChallengeIndex(mPageIndex, true);
			mApp->mPlayerInfo->mHasNewIZombie = false;
		}
	}

	UpdateScrollRange();
}

//0x42E280 & 0x42E2A0
ChallengeScreen::~ChallengeScreen()
{
	delete mBackButton;
	for (ButtonWidget* aPageButton : mPageButton) delete aPageButton;
	for (ButtonWidget* aChallengeButton : mChallengeButtons) delete aChallengeButton;
	delete mToolTip;
}

ChallengeDefinition& GetChallengeDefinition(int theChallengeMode)
{
	TOD_ASSERT(theChallengeMode >= 0 && theChallengeMode < NUM_CHALLENGE_MODES);

	ChallengeDefinition& aDef = gChallengeDefs[theChallengeMode];
	TOD_ASSERT(aDef.mChallengeMode == theChallengeMode + GAMEMODE_SURVIVAL_NORMAL_STAGE_1);

	return gChallengeDefs[theChallengeMode];
}

bool ChallengeScreen::IsScaryPotterLevel(GameMode theGameMode)
{
	return theGameMode >= GAMEMODE_SCARY_POTTER_1 && theGameMode <= GAMEMODE_SCARY_POTTER_ENDLESS;
}

bool ChallengeScreen::IsIZombieLevel(GameMode theGameMode)
{
	return theGameMode >= GAMEMODE_PUZZLE_I_ZOMBIE_1 && theGameMode <= GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS;
}

//0x42E3C0
void ChallengeScreen::SetUnlockChallengeIndex(ChallengePage thePage, bool theIsIZombie)
{
	mUnlockState = UNLOCK_SHAKING;
	mUnlockStateCounter = 100;
	mUnlockChallengeIndex = 0;

	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition& aDef = GetChallengeDefinition(aChallengeMode);
		if (aDef.mPage == thePage)
		{
			if (thePage != CHALLENGE_PAGE_PUZZLE || (!theIsIZombie && IsScaryPotterLevel(aDef.mChallengeMode)) || (theIsIZombie && IsIZombieLevel(aDef.mChallengeMode)))
			{
				if (AccomplishmentsNeeded(aChallengeMode) <= 0)
				{
					mUnlockChallengeIndex = aChallengeMode;
				}
			}
		}
	}
}

//0x42E440
int ChallengeScreen::MoreTrophiesNeeded(int theChallengeIndex)
{
	if (mApp->mPlayerInfo) {
		if (mApp->mPlayerInfo->mHasUsedCheatKeys) {
			return 0;
		}
	}

	ChallengeDefinition& aDef = GetChallengeDefinition(theChallengeIndex);
	if (mApp->mGameMode == GAMEMODE_UPSELL && mApp->mGameScene == SCENE_LEVEL_INTRO)
	{
		return aDef.mChallengeMode == GAMEMODE_CHALLENGE_FINAL_BOSS ? 1 : 0;
	}
	
	if (mApp->IsTrialStageLocked())
	{
		if (mPageIndex == CHALLENGE_PAGE_PUZZLE && aDef.mChallengeMode >= GAMEMODE_SCARY_POTTER_4)
		{
			return aDef.mChallengeMode == GAMEMODE_SCARY_POTTER_4 ? 1 : 2;
		}
		else if (mPageIndex == CHALLENGE_PAGE_CHALLENGE && aDef.mChallengeMode >= GAMEMODE_CHALLENGE_RAINING_SEEDS)
		{
			return aDef.mChallengeMode == GAMEMODE_CHALLENGE_RAINING_SEEDS ? 1 : 2;
		}
		else if (mPageIndex == CHALLENGE_PAGE_SURVIVAL && aDef.mChallengeMode >= GAMEMODE_SURVIVAL_NORMAL_STAGE_4)
		{
			return aDef.mChallengeMode == GAMEMODE_SURVIVAL_NORMAL_STAGE_4 ? 1 : 2;
		}
	}

	if (aDef.mPage == CHALLENGE_PAGE_PUZZLE)
	{
		if (IsScaryPotterLevel(aDef.mChallengeMode))
		{
			int aLevelsCompleted = 0;
			for (ChallengeDefinition& aSPDef : gChallengeDefs)
			{
				if (IsScaryPotterLevel(aSPDef.mChallengeMode) && mApp->HasBeatenChallenge(aSPDef.mChallengeMode))
				{
					aLevelsCompleted++;
				}
			}

			if (aDef.mChallengeMode < GAMEMODE_SCARY_POTTER_4 || mApp->HasFinishedAdventure() || aLevelsCompleted < 3)
			{
				return ClampInt(aDef.mChallengeMode - GAMEMODE_SCARY_POTTER_1 - aLevelsCompleted, 0, 9);
			}
			else
			{
				return aDef.mChallengeMode == GAMEMODE_SCARY_POTTER_4 ? 1 : 2;
			}
		}
		else if (IsIZombieLevel(aDef.mChallengeMode))
		{
			int aLevelsCompleted = 0;
			for (ChallengeDefinition& aIZDef : gChallengeDefs)
			{
				if (IsIZombieLevel(aIZDef.mChallengeMode) && mApp->HasBeatenChallenge(aIZDef.mChallengeMode))
				{
					aLevelsCompleted++;
				}
			}

			if (aDef.mChallengeMode < GAMEMODE_PUZZLE_I_ZOMBIE_4 || mApp->HasFinishedAdventure() || aLevelsCompleted < 3)
			{
				return ClampInt(aDef.mChallengeMode - GAMEMODE_PUZZLE_I_ZOMBIE_1 - aLevelsCompleted, 0, 9);
			}
			else
			{
				return aDef.mChallengeMode == GAMEMODE_PUZZLE_I_ZOMBIE_4 ? 1 : 2;
			}
		}
	}
	else
	{
		int aIdxInPage = aDef.mRow * 5 + aDef.mCol;
		if ((aDef.mPage == CHALLENGE_PAGE_CHALLENGE || aDef.mPage == CHALLENGE_PAGE_SURVIVAL || aDef.mPage == CHALLENGE_PAGE_LAST_STAND) && !mApp->HasFinishedAdventure())
		{
			return aIdxInPage < 3 ? 0 : aIdxInPage == 3 ? 1 : 2;
		}
		else
		{
			int aNumTrophies = mApp->GetNumTrophies(aDef.mPage);
			if (aDef.mPage == CHALLENGE_PAGE_LIMBO_CHALLENGE || aDef.mPage == CHALLENGE_PAGE_LIMBO_SURVIVAL)
			{
				return 0;
			}
			if (mApp->IsSurvivalEndless(aDef.mChallengeMode))
			{
				return 10 - aNumTrophies;
			}
			if (mApp->IsLastStandEndless(aDef.mChallengeMode))
			{
				return 5 - aNumTrophies;
			}
			if (aDef.mPage == CHALLENGE_PAGE_SURVIVAL || aDef.mPage == CHALLENGE_PAGE_CHALLENGE || aDef.mPage == CHALLENGE_PAGE_LAST_STAND)
			{
				aNumTrophies += 3;
			}
			else
			{
				//TOD_ASSERT();
			}

			return aIdxInPage >= aNumTrophies ? aIdxInPage - aNumTrophies + 1 : 0;
		}
	}

	return 0;
}

//0x42E6E0
bool ChallengeScreen::ShowPageButtons()
{
	return false;//mApp->mTodCheatKeys && mPageIndex != CHALLENGE_PAGE_SURVIVAL && mPageIndex != CHALLENGE_PAGE_PUZZLE;
}

//0x42E710
void ChallengeScreen::UpdateButtons()
{
	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition aDefinition = GetChallengeDefinition(aChallengeMode);
		ButtonWidget* aButton = mChallengeButtons[aChallengeMode];

		aButton->mVisible = aDefinition.mPage == mPageIndex;
		/*if (aDefinition.mChallengeMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || aDefinition.mChallengeMode == GameMode::GAMEMODE_TREE_OF_WISDOM || aDefinition.mChallengeMode >= GameMode::GAMEMODE_UPSELL && aDefinition.mChallengeMode <= GameMode::GAMEMODE_INTRO)
			aButton->mVisible = false;*/

		if (aButton->mVisible)
		{
			ChallengeDefinition& aChlDef = GetChallengeDefinition(aChallengeMode);

			int aColumn = aChlDef.mCol;
			int aRow = aChlDef.mRow;
			if (aChlDef.mPage == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE && aChlDef.mChallengeMode >= GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
			{
				int newColumn = aChlDef.mCol - 1;
				aColumn = newColumn % 4;
				if (newColumn < 0)
				{
					aRow--;
					aColumn = 4;
				}
			}

			/*if (aChlDef.mPage == CHALLENGE_PAGE_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_LIMBO_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_PUZZLE)
				aButton->Resize(38 + aColumn * 155, 93 + aRow * 119 - mScrollbar->mScrollValue, 104, 115);
			else
				aButton->Resize(38 + aColumn * 155, 125 + aRow * 145 - mScrollbar->mScrollValue, 104, 115);*/

			aButton->mDisabled = (MoreTrophiesNeeded(aChallengeMode) > 0) /*|| (aButton->mY + aButton->mHeight < mScrollbar->mY || aButton->mY > mScrollbar->mY + mScrollbar->mHeight)*/;
			aButton->mDoFinger = !aButton->mDisabled;
		}
	}
	

	for (int aPage = 0; aPage < MAX_CHALLANGE_PAGES; aPage++)
	{
		ButtonWidget* aPageButton = mPageButton[aPage];
		if (aPage == mPageIndex)
		{
			aPageButton->mColors[ButtonWidget::COLOR_LABEL] = Color(64, 64, 64);
			aPageButton->mDisabled = true;
		}
		else
		{
			aPageButton->mColors[ButtonWidget::COLOR_LABEL] = Color(255, 240, 0);
			aPageButton->mDisabled = false;
		}
	}
}

//0x42E8A0
int ChallengeScreen::AccomplishmentsNeeded(int theChallengeIndex)
{
	int aTrophiesNeeded = MoreTrophiesNeeded(theChallengeIndex);
	GameMode aGameMode = GetChallengeDefinition(theChallengeIndex).mChallengeMode;
	if (mApp->IsSurvivalEndless(aGameMode) && aTrophiesNeeded <= 3 && mApp->GetNumTrophies(CHALLENGE_PAGE_SURVIVAL) < 10 &&
		mApp->GetNumTrophies(CHALLENGE_PAGE_LAST_STAND) < 5 &&
		mApp->HasFinishedAdventure() && !mApp->IsTrialStageLocked()) aTrophiesNeeded = 1;
	return mCheatEnableChallenges ? 0 : aTrophiesNeeded;
}

//0x42E920
void ChallengeScreen::DrawButton(Graphics* g, int theChallengeIndex)
{
	g->PushState();
	ButtonWidget* aChallengeButton = mChallengeButtons[theChallengeIndex];
	g->SetClipRect(mScrollbar->mViewRect);

	if (aChallengeButton->mVisible)
	{
		ChallengeDefinition& aDef = GetChallengeDefinition(theChallengeIndex);

		int aColumn = aDef.mCol;
		int aRow = aDef.mRow;
		if (aDef.mPage == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE && aDef.mChallengeMode >= GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
		{
			int newColumn = aDef.mCol - 1;
			aColumn = newColumn % 4;
			if (newColumn < 0)
			{
				aRow--;
				aColumn = 4;
			}
		}
		
		if (aDef.mPage == CHALLENGE_PAGE_CHALLENGE || aDef.mPage == CHALLENGE_PAGE_LIMBO_CHALLENGE || aDef.mPage == CHALLENGE_PAGE_PUZZLE)
			aChallengeButton->mY = 93 + aRow * 119 - mScrollbar->mScrollValue;
		else
			aChallengeButton->mY = 125 + aRow * 145 - mScrollbar->mScrollValue;

		int aPosX = aChallengeButton->mX;
		int aPosY = aChallengeButton->mY;

		if (aChallengeButton->mY >= mScrollbar->mViewRect.mY + mScrollbar->mViewRect.mHeight ||
			aChallengeButton->mY + aChallengeButton->mHeight <= mScrollbar->mViewRect.mY ||
			mApp->mWidgetManager->mLastMouseY >= mScrollbar->mViewRect.mY + mScrollbar->mViewRect.mHeight ||
			mApp->mWidgetManager->mLastMouseY <= mScrollbar->mViewRect.mY)
		{
			aChallengeButton->mY = 600;
		}

		if (aChallengeButton->mIsDown)
		{
			aPosX++;
			aPosY++;
		}

		if (AccomplishmentsNeeded(theChallengeIndex) <= 1)
		{
			// ============================================================================================
			// ▲ 绘制按钮上的小游戏图标
			// ============================================================================================
			if (aChallengeButton->mDisabled)
			{
				g->SetColor(Color(92, 92, 92));
				g->SetColorizeImages(true);
			}
			if (theChallengeIndex == mUnlockChallengeIndex &&mApp->mPlayerInfo && !mApp->mPlayerInfo->mHasUsedCheatKeys)
			{
				if (mUnlockState == UNLOCK_SHAKING)
				{
					g->SetColor(Color(92, 92, 92));
				}
				else if (mUnlockState == UNLOCK_FADING)
				{
					int aColor = TodAnimateCurve(50, 25, mUnlockStateCounter, 92, 255, CURVE_LINEAR);
					g->SetColor(Color(aColor, aColor, aColor));
				}
				g->SetColorizeImages(true);
			}
			if (mPageIndex == CHALLENGE_PAGE_SURVIVAL || mPageIndex == CHALLENGE_PAGE_LIMBO_SURVIVAL)
			{
				g->DrawImageCel(Sexy::IMAGE_SURVIVAL_THUMBNAILS, aPosX + 13, aPosY + 4, aDef.mChallengeIconIndex);
			}
			else
			{
				g->DrawImageCel(Sexy::IMAGE_CHALLENGE_THUMBNAILS, aPosX + 13, aPosY + 4, aDef.mChallengeIconIndex);
			}

			// ============================================================================================
			// ▲ 绘制小游戏按钮边框
			// ============================================================================================
			bool aHighLight = aChallengeButton->mIsOver && theChallengeIndex != mUnlockChallengeIndex;
			g->SetColorizeImages(false);
			g->PushState();
			g->SetLinearBlend(false);
			g->DrawImage(!aHighLight ? Sexy::IMAGE_CHALLENGE_WINDOW : Sexy::IMAGE_CHALLENGE_WINDOW_HIGHLIGHT, aPosX - 6, aPosY - 2);
			g->PopState();
			// ============================================================================================
			// ▲ 绘制小游戏的名称
			// ============================================================================================
			Color aTextColor = aHighLight ? Color(250, 40, 40) : Color(42, 42, 90);

			SexyString aName = TodStringTranslate(aDef.mChallengeName);

			if (aChallengeButton->mDisabled || (theChallengeIndex == mUnlockChallengeIndex && mUnlockState == UNLOCK_SHAKING && mApp->mPlayerInfo && !mApp->mPlayerInfo->mHasUsedCheatKeys))
			{
#ifdef _DS_MINIGAMES
				if (!(aDef.mChallengeMode >= GameMode::GAMEMODE_CHALLENGE_BOMB_ALL_TOGETHER && aDef.mChallengeMode <= GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP))
#endif
				aName = _S("?");
			}

			int aNameLen = aName.size();

			if (TodDrawStringWrappedHelper(g, aName, Rect(aPosX + 6, aPosY + 76, 94, 28), Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER, false) < 28)
			{
				TodDrawString(g, aName, aPosX + 52, aPosY + 96, Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER);
			}
			else
			{
				TodDrawStringWrapped(g, aName, Rect(aPosX + 6, aPosY + 76, 94, 28), Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER_VERTICAL_MIDDLE);
			}

			/*
			if (aNameLen < 13)
			{
				TodDrawString(g, aName, aPosX + 52, aPosY + 96, Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER);
			}
			else
			{
				// 先尝试在名称字符串的后半段取空格以将字符串分隔为两行，若后半段中无空格则在整个字符串中寻找空格
				int aHalfPos = ((mPageIndex == CHALLENGE_PAGE_SURVIVAL || mPageIndex == CHALLENGE_PAGE_LIMBO_SURVIVAL) && !aChallengeButton->mDisabled) ? 7 : (aNameLen / 2 - 1);
#ifdef _USE_WIDE_STRING
				const SexyChar* aSpacedChar = aName.c_str() + aHalfPos;
#else
				const SexyChar* aSpacedChar = _S(aName.c_str() + aHalfPos, _S(' '));
#endif
				while(aSpacedChar[0]!=' ')
				{
					aHalfPos++;
#ifdef _USE_WIDE_STRING
					aSpacedChar = aName.c_str() + aHalfPos;
#else
					aSpacedChar = _S(aName.c_str() + aHalfPos, _S(' '));
#endif
					if(aSpacedChar[0]=='\0')
					{
						aHalfPos--;
#ifdef _USE_WIDE_STRING
						aSpacedChar = aName.c_str() + aHalfPos;
#else
						aSpacedChar = _S(aName.c_str() + aHalfPos, _S(' '));

#endif
						break;
					}
				}
				aHalfPos--;
#ifdef _USE_WIDE_STRING
				aSpacedChar = aName.c_str() + aHalfPos;
#else
				aSpacedChar = _S(aName.c_str() + aHalfPos, _S(' '));
#endif
				
				if (aSpacedChar == nullptr)
				{
#ifdef _USE_WIDE_STRING
					aSpacedChar = aName.c_str();
#else
					aSpacedChar = _S(aName.c_str(), _S(' '));
#endif
				}

				// 分别计算取得两行文本的长度
				int aLine1Len = aNameLen;
				int aLine2Len = 0;
				if (aSpacedChar != nullptr)
				{
					aLine1Len = aSpacedChar - aName.c_str();
					aLine2Len = aNameLen - aLine1Len - 1;
				}
				
				// 分别绘制两行文本字符串
				auto topStr=aName.substr(0, aLine1Len+1);
				auto botStr=aName.substr(aLine1Len + 1, aLine2Len);
				if(botStr.empty())
				{
					TodDrawString(g, aName, aPosX + 52, aPosY + 96, Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER);
				}
				else
				{
					TodDrawString(g, topStr, aPosX + 52, aPosY + 88, Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER);
					if (aLine2Len > 0)
					{
						TodDrawString(g, botStr, aPosX + 52, aPosY + 102, Sexy::FONT_BRIANNETOD12, aTextColor, DS_ALIGN_CENTER);
					}
				}
			
			}*/

			// ============================================================================================
			// ▲ 绘制关卡锁定或关卡完成的贴图以及关卡最高记录的文本等
			// ============================================================================================
			int aRecord = mApp->mPlayerInfo->mChallengeRecords[theChallengeIndex];
			if (theChallengeIndex == mUnlockChallengeIndex && mApp->mPlayerInfo && !mApp->mPlayerInfo->mHasUsedCheatKeys)
			{
				Image* aLockImage = Sexy::IMAGE_LOCK;
				if (mUnlockState == UNLOCK_FADING)
				{
					aLockImage = Sexy::IMAGE_LOCK_OPEN;
					g->SetColor(Color(255, 255, 255, TodAnimateCurve(25, 0, mUnlockStateCounter, 255, 0, CURVE_LINEAR)));
					g->SetColorizeImages(true);
				}
				TodDrawImageScaledF(g, aLockImage, aPosX + 24 + mLockShakeX, aPosY + 9 + mLockShakeY, 0.7f, 0.7f);
				g->SetColorizeImages(false);
			}
			else if (aRecord > 0)
			{
				if (mApp->HasBeatenChallenge(aDef.mChallengeMode))
				{
					g->DrawImage(Sexy::IMAGE_MINIGAME_TROPHY, aPosX - 6, aPosY - 2);
				}
				else if (mApp->IsEndlessScaryPotter(aDef.mChallengeMode) || mApp->IsEndlessIZombie(aDef.mChallengeMode))
				{
					SexyString aAchievement = mApp->Pluralize(aRecord, _S("[ONE_FLAG]"), _S("[COUNT_FLAGS]"));
					TodDrawString(g, aAchievement, aPosX + 48, aPosY + 48, Sexy::FONT_CONTINUUMBOLD14OUTLINE, Color::White, DS_ALIGN_CENTER);
					TodDrawString(g, aAchievement, aPosX + 48, aPosY + 48, Sexy::FONT_CONTINUUMBOLD14, Color(255, 0, 0), DS_ALIGN_CENTER);
				}
				else if (mApp->IsSurvivalEndless(aDef.mChallengeMode) || mApp->IsLastStandEndless(aDef.mChallengeMode))
				{
					SexyString aAchievement = TodReplaceNumberString(_S("[LONGEST_STREAK]"), _S("{STREAK}"), aRecord);
					Rect aRect(aPosX, aPosY + 15, 96, 200);
					// Keep the outline and fill on the same glyph atlas for this narrow wrapped label.
					for (int anOffsetY = -1; anOffsetY <= 1; anOffsetY++)
					{
						for (int anOffsetX = -1; anOffsetX <= 1; anOffsetX++)
						{
							if (anOffsetX == 0 && anOffsetY == 0)
								continue;

							Rect anOutlineRect(
								aRect.mX + anOffsetX,
								aRect.mY + anOffsetY,
								aRect.mWidth,
								aRect.mHeight
							);
							TodDrawStringWrapped(
								g,
								aAchievement,
								anOutlineRect,
								Sexy::FONT_CONTINUUMBOLD14,
								Color::White,
								DS_ALIGN_CENTER
							);
						}
					}
					TodDrawStringWrapped(g, aAchievement, aRect, Sexy::FONT_CONTINUUMBOLD14, Color(255, 0, 0), DS_ALIGN_CENTER);
				}
			}
			else if (aChallengeButton->mDisabled)
			{
				TodDrawImageScaledF(g, Sexy::IMAGE_LOCK, aPosX + 24, aPosY + 9, 0.7f, 0.7f);
			}
		}
		else
		{
			g->DrawImage(Sexy::IMAGE_CHALLENGE_BLANK, aPosX, aPosY);
		}
	}

	g->PopState();
}

//0x42F160
void ChallengeScreen::Draw(Graphics* g)
{
	g->SetLinearBlend(true);
	g->DrawImage(Sexy::IMAGE_CHALLENGE_BACKGROUND, 0, 0);

	SexyString aTitleString = _S("");
	
	switch (mPageIndex) {
		case ChallengePage::CHALLENGE_PAGE_SURVIVAL:
			aTitleString = _S("[PICK_AREA]");
			break;
		case ChallengePage::CHALLENGE_PAGE_PUZZLE:
			aTitleString = _S("[SCARY_POTTER]");
			break;
		case ChallengePage::CHALLENGE_PAGE_CHALLENGE:
			aTitleString = _S("[PICK_CHALLENGE]");
			break;
		case ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE:
		case ChallengePage::CHALLENGE_PAGE_LIMBO_SURVIVAL:
			aTitleString = _S("[LIMBO]");
			break;
		case ChallengePage::CHALLENGE_PAGE_LAST_STAND:
			aTitleString = _S("[LAST_STAND]");
			break;
		default: 
			break;
	}
	TodDrawString(g, aTitleString, 400, 58, Sexy::FONT_HOUSEOFTERROR28, Color(220, 220, 220), DS_ALIGN_CENTER);

	int aTrophiesGot = mApp->GetNumTrophies(mPageIndex);
	int aTrophiesTotal = mPageIndex == CHALLENGE_PAGE_SURVIVAL ? 10 : mPageIndex == CHALLENGE_PAGE_CHALLENGE ? 20 : mPageIndex == CHALLENGE_PAGE_PUZZLE ? 18 : mPageIndex == CHALLENGE_PAGE_LAST_STAND ? 5 : 0;
	if (aTrophiesTotal > 0)
	{
		SexyString aTrophyString = StrFormat(_S("%d/%d"), aTrophiesGot, aTrophiesTotal);
		TodDrawString(g, aTrophyString, 739, 73, Sexy::FONT_DWARVENTODCRAFT15, Color(255, 240, 0), DS_ALIGN_CENTER);
		TodDrawImageScaledF(g, Sexy::IMAGE_TROPHY, 718, 26, 0.5f, 0.5f);
	}

	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
		DrawButton(g, aChallengeMode);

	mToolTip->Draw(g);
}

//0x42F510
void ChallengeScreen::Update()
{
	Widget::Update();
	UpdateToolTip();

	if (mUnlockStateCounter > 0) mUnlockStateCounter--;

	if (mUnlockState == UNLOCK_SHAKING && mApp->mPlayerInfo && !mApp->mPlayerInfo->mHasUsedCheatKeys)
	{
		if (mUnlockStateCounter == 0)
		{
			mApp->PlayFoley(FOLEY_PAPER);
			mUnlockState = UNLOCK_FADING;
			mUnlockStateCounter = 50;
			mLockShakeX = 0;
			mLockShakeY = 0;
		}
		else
		{
			mLockShakeX = RandRangeFloat(-2, 2);
			mLockShakeY = RandRangeFloat(-2, 2);
		}
	}
	else if (mUnlockState == UNLOCK_FADING && mUnlockStateCounter == 0 && mApp->mPlayerInfo && !mApp->mPlayerInfo->mHasUsedCheatKeys)
	{
		mUnlockState = UNLOCK_OFF;
		mUnlockStateCounter = 0;
		mUnlockChallengeIndex = -1;
	}

	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition aDefinition = GetChallengeDefinition(aChallengeMode);
		ButtonWidget* aButton = mChallengeButtons[aChallengeMode];

		aButton->mVisible = aDefinition.mPage == mPageIndex;
		/*if (aDefinition.mChallengeMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || aDefinition.mChallengeMode == GameMode::GAMEMODE_TREE_OF_WISDOM || aDefinition.mChallengeMode >= GameMode::GAMEMODE_UPSELL && aDefinition.mChallengeMode <= GameMode::GAMEMODE_INTRO)
			aButton->mVisible = false;*/

		if (aButton->mVisible)
		{
			ChallengeDefinition& aChlDef = GetChallengeDefinition(aChallengeMode);

			int aColumn = aChlDef.mCol;
			int aRow = aChlDef.mRow;
			if (aChlDef.mPage == ChallengePage::CHALLENGE_PAGE_LIMBO_CHALLENGE && aChlDef.mChallengeMode >= GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
			{
				int newColumn = aChlDef.mCol - 1;
				aColumn = newColumn % 4;
				if (newColumn < 0)
				{
					aRow--;
					aColumn = 4;
				}
			}

			/*if (aChlDef.mPage == CHALLENGE_PAGE_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_LIMBO_CHALLENGE || aChlDef.mPage == CHALLENGE_PAGE_PUZZLE)
				aButton->Resize(38 + aColumn * 155, 93 + aRow * 119 - mScrollbar->mScrollValue, 104, 115);
			else
				aButton->Resize(38 + aColumn * 155, 125 + aRow * 145 - mScrollbar->mScrollValue, 104, 115);*/
		}
	}

	MarkDirty();
}

//0x42F640
void ChallengeScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	AddWidget(mBackButton);
	for (ButtonWidget* aButton : mPageButton) AddWidget(aButton);
	for (ButtonWidget* aButton : mChallengeButtons) AddWidget(aButton);
	AddWidget(mScrollbar);
}

//0x42F6B0
void ChallengeScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	RemoveWidget(mBackButton);
	for (ButtonWidget* aButton : mPageButton) RemoveWidget(aButton);
	for (ButtonWidget* aButton : mChallengeButtons) RemoveWidget(aButton);
	RemoveWidget(mScrollbar);
}

//0x42F720
void ChallengeScreen::ButtonPress(int theId)
{
	mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
}

//0x42F740
void ChallengeScreen::ButtonDepress(int theId)
{
	if (theId == ChallengeScreen::ChallengeScreen_Back)
	{
		mApp->KillChallengeScreen();
		mApp->DoBackToMain();
	}

	int aChallengeMode = theId - ChallengeScreen::ChallengeScreen_Mode;
	if (aChallengeMode >= 0 && aChallengeMode < NUM_CHALLENGE_MODES)
	{
		mApp->KillChallengeScreen();
		mApp->PreNewGame((GameMode)(aChallengeMode + 1), true);
	}

	int aPageIndex = theId - ChallengeScreen::ChallengeScreen_Page;
	if (aPageIndex >= 0 && aPageIndex < ChallengePage::MAX_CHALLANGE_PAGES)
	{
		mPageIndex = (ChallengePage)aPageIndex;
		UpdateButtons();
		UpdateScrollRange();
	}
}

//0x42F7E0
void ChallengeScreen::UpdateToolTip()
{
	if (!mApp->mWidgetManager->mMouseIn || !mApp->mActive)
	{
		mToolTip->mVisible = false;
		return;
	}

	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition& aDef = GetChallengeDefinition(aChallengeMode);
		ButtonWidget* aChallengeButton = mChallengeButtons[aChallengeMode];
		if (aChallengeButton->mVisible && aChallengeButton->mDisabled &&
			aChallengeButton->Contains(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY) &&
			AccomplishmentsNeeded(aChallengeMode) <= 1)
		{
			mToolTip->mX = aChallengeButton->mWidth / 2 + aChallengeButton->mX;
			mToolTip->mY = aChallengeButton->mY;
			if (MoreTrophiesNeeded(aChallengeMode) > 0)
			{
				SexyString aLabel;
				if (mPageIndex == CHALLENGE_PAGE_PUZZLE)
				{
					if (IsScaryPotterLevel(aDef.mChallengeMode))
					{
						if (!mApp->HasFinishedAdventure() && aDef.mChallengeMode == GAMEMODE_SCARY_POTTER_4)
						{
							aLabel = _S("[FINISH_ADVENTURE_TOOLTIP]");
						}
						else
						{
							aLabel = _S("[ONE_MORE_SCARY_POTTER_TOOLTIP]");
						}
					}
					else if (IsIZombieLevel(aDef.mChallengeMode))
					{
						if (!mApp->HasFinishedAdventure() && aDef.mChallengeMode == GAMEMODE_PUZZLE_I_ZOMBIE_4)
						{
							aLabel = _S("[FINISH_ADVENTURE_TOOLTIP]");
						}
						else
						{
							aLabel = _S("[ONE_MORE_IZOMBIE_TOOLTIP]");
						}
					}
				}
				else if (!mApp->HasFinishedAdventure() || mApp->IsTrialStageLocked())
				{
					aLabel = _S("[FINISH_ADVENTURE_TOOLTIP]");
				}
				else if (mApp->IsSurvivalEndless(aDef.mChallengeMode))
				{
					aLabel = _S("[10_SURVIVAL_TOOLTIP]");
				}
				else if (mPageIndex == CHALLENGE_PAGE_SURVIVAL)
				{
					aLabel = _S("[ONE_MORE_SURVIVAL_TOOLTIP]");
				}
				else if (mPageIndex == CHALLENGE_PAGE_CHALLENGE)
				{
					aLabel = _S("[ONE_MORE_CHALLENGE_TOOLTIP]");
				}
				else if (mApp->IsLastStandEndless(aDef.mChallengeMode))
				{
					aLabel = _S("[10_LAST_STAND_TOOLTIP]");
				}
				else if (mPageIndex == CHALLENGE_PAGE_LAST_STAND)
				{
					aLabel = _S("[ONE_MORE_LAST_STAND_TOOLTIP]");
				}
				else continue;

				mToolTip->SetLabel(aLabel);
				mToolTip->mVisible = true;
				return;
			} // end if (MoreTrophiesNeeded(aChallengeMode) > 0)
		} // end 需要显示标签的条件判断
	}

	mToolTip->mVisible = false;
}

void ChallengeScreen::MouseWheel(int theDelta)
{
	Widget::MouseWheel(theDelta);
	mScrollbar->MouseWheel(theDelta);
}

void ChallengeScreen::UpdateScrollRange()
{
	mScrollbar->mScrollValue = 0.0f;
	mScrollbar->mScrollAmount = 0.0f;

	float totalHeight = 0.0f;

	for (int aChallengeMode = 0; aChallengeMode < NUM_CHALLENGE_MODES; aChallengeMode++)
	{
		ChallengeDefinition aDefinition = GetChallengeDefinition(aChallengeMode);
		ButtonWidget* aButton = mChallengeButtons[aChallengeMode];

		aButton->mVisible = aDefinition.mPage == mPageIndex;
		/*if (aDefinition.mChallengeMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || aDefinition.mChallengeMode == GameMode::GAMEMODE_TREE_OF_WISDOM || aDefinition.mChallengeMode >= GameMode::GAMEMODE_UPSELL && aDefinition.mChallengeMode <= GameMode::GAMEMODE_INTRO)
			aButton->mVisible = false;*/

		if (aButton->mVisible)
		{
			totalHeight = max(aButton->mY + aButton->mHeight, totalHeight);
		}
	}

	mScrollbar->SetScrollRange(totalHeight - mScrollbar->mY - mScrollbar->mHeight - 5);
	mScrollbar->mDisabled = mScrollbar->mScrollRange <= 0.0f;
}
