#include "../Board.h"
#include "GameButton.h"
#include "StoreScreen.h"
#include "../ZenGarden.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "../ToolTipWidget.h"
#include "../System/SaveGame.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/ProfileMgr.h"
#include "../System/TypingCheck.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../SexyAppFramework/Font.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/TodParticle.h"
#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../GameConstants.h"

static float gFlowerCenter[3][2] = { { 765.0f, 483.0f }, { 663.0f, 455.0f }, { 701.0f, 439.0f } };  //0x665430

//0x448C80
void GameSelectorOverlay::Draw(Graphics* g)
{ 
	mParent->DrawOverlay(g);
}

GameSelectorOverlay::GameSelectorOverlay(GameSelector* theGameSelector)
{
	mParent = theGameSelector;
	mMouseVisible = false;
	mHasAlpha = true;
}

//0x448CB0
GameSelector::GameSelector(LawnApp* theApp)
{
	TodHesitationTrace("pregameselector");

	mApp = theApp;
	mLevel = 1;
	mLoading = false;
	mHasTrophy = false;
	mToolTip = new ToolTipWidget();

	mAdventureButton = MakeNewButton(
		GameSelector::GameSelector_Adventure, 
		this, 
		_S(""), 
		nullptr, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_BUTTON, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_HIGHLIGHT, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_HIGHLIGHT
	);
	mAdventureButton->Resize(0, 0, Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_BUTTON->mWidth, 125);
	mAdventureButton->mClip = false;
	mAdventureButton->mBtnNoDraw = true;
	mAdventureButton->mMouseVisible = false;
	mAdventureButton->mPolygonShape[0] = SexyVector2(7.0f, 1.0f);
	mAdventureButton->mPolygonShape[1] = SexyVector2(328.0f, 30.0f);
	mAdventureButton->mPolygonShape[2] = SexyVector2(314.0f, 125.0f);
	mAdventureButton->mPolygonShape[3] = SexyVector2(1.0f, 78.0f);
	mAdventureButton->mUsePolygonShape = true;

	mMinigameButton = MakeNewButton(
		GameSelector::GameSelector_Minigame, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_BUTTON, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT
	);
	mMinigameButton->Resize(0, 0, Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_BUTTON->mWidth, 130);
	mMinigameButton->mClip = false;
	mMinigameButton->mBtnNoDraw = true;
	mMinigameButton->mMouseVisible = false;
	mMinigameButton->mPolygonShape[0] = SexyVector2(4.0f, 2.0f);
	mMinigameButton->mPolygonShape[1] = SexyVector2(312.0f, 51.0f);
	mMinigameButton->mPolygonShape[2] = SexyVector2(296.0f, 130.0f);
	mMinigameButton->mPolygonShape[3] = SexyVector2(7.0f, 77.0f);
	mMinigameButton->mUsePolygonShape = true;

	mPuzzleButton = MakeNewButton(
		GameSelector::GameSelector_Puzzle, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_BUTTON, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT
	);
	mPuzzleButton->Resize(0, 0, Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_BUTTON->mWidth, 121);
	mPuzzleButton->mClip = false;
	mPuzzleButton->mBtnNoDraw = true;
	mPuzzleButton->mMouseVisible = false;
	mPuzzleButton->mPolygonShape[0] = SexyVector2(2.0f, 0.0f);
	mPuzzleButton->mPolygonShape[1] = SexyVector2(281.0f, 55.0f);
	mPuzzleButton->mPolygonShape[2] = SexyVector2(268.0f, 121.0f);
	mPuzzleButton->mPolygonShape[3] = SexyVector2(3.0f, 60.0f);
	mPuzzleButton->mUsePolygonShape = true;

	mSurvivalButton = MakeNewButton(
		GameSelector::GameSelector_Survival, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_BUTTON, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT, 
		Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT
	);
	mSurvivalButton->Resize(0, 0, Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_BUTTON->mWidth, 124);
	mSurvivalButton->mClip = false;
	mSurvivalButton->mBtnNoDraw = true;
	mSurvivalButton->mMouseVisible = false;
	mSurvivalButton->mPolygonShape[0] = SexyVector2(7.0f, 1.0f);
	mSurvivalButton->mPolygonShape[1] = SexyVector2(267.0f, 62.0f);
	mSurvivalButton->mPolygonShape[2] = SexyVector2(257.0f, 124.0f);
	mSurvivalButton->mPolygonShape[3] = SexyVector2(7.0f, 57.0f);
	mSurvivalButton->mUsePolygonShape = true;

#ifdef _HAS_ZOMBATAR
	// @Patoke: add these button defs
	mZombatarButton = MakeNewButton(
		GameSelector::GameSelector_Zombatar,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK
	);
	mZombatarButton->Resize(0, 0, Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS->mWidth, Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS->mHeight);
	mZombatarButton->mClip = false;
	mZombatarButton->mBtnNoDraw = true;
	mZombatarButton->mMouseVisible = false;
#endif
#ifdef _HAS_ACHIEVEMENTS
	mAchievementsButton = MakeNewButton(
		GameSelector::GameSelector_Achievements,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL,
		Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL_PRESS,
		Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL_PRESS
	);
	mAchievementsButton->Resize(0, 0, Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL->mWidth, Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL->mHeight);
	mAchievementsButton->mClip = false;
	//mAchievementsButton->mBtnNoDraw = true;
	mAchievementsButton->mMouseVisible = false;
	mAchievementsButton->mTranslateX = 0;
	mAchievementsButton->mTranslateY = 0;

	mTrophyButton = MakeNewButton(
		-1,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK
	);
	mTrophyButton->Resize(0, 0, Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL->mWidth, 200);
	mTrophyButton->mClip = false;
	mTrophyButton->mBtnNoDraw = false;
	mTrophyButton->mMouseVisible = true;
	mTrophyButton->mDoFinger = false;
	mTrophyButton->mTranslateX = 0;
	mTrophyButton->mTranslateY = 0;
#endif
#ifdef _HAS_MORESCREEN
	mQuickPlayButton = MakeNewButton(
		GameSelector::GameSelector_QuickPlay,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_SELECTOR_MORE_BUTTON,
		Sexy::IMAGE_SELECTOR_MORE_BUTTON_HIGHLIGHT,
		Sexy::IMAGE_SELECTOR_MORE_BUTTON_HIGHLIGHT
	);
	mQuickPlayButton->Resize(0, 0, Sexy::IMAGE_QUICKPLAY_BACK_BUTTON->mWidth, Sexy::IMAGE_QUICKPLAY_BACK_BUTTON->mHeight);
	mQuickPlayButton->mClip = false;
	mQuickPlayButton->mMouseVisible = false;
	mQuickPlayButton->mTranslateX = 0;
	mQuickPlayButton->mTranslateY = 0;
#endif

	mZenGardenButton = MakeNewButton(
		GameSelector::GameSelector_ZenGarden, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_ZENGARDEN, 
		Sexy::IMAGE_SELECTORSCREEN_ZENGARDENHIGHLIGHT, 
		Sexy::IMAGE_SELECTORSCREEN_ZENGARDENHIGHLIGHT
	);
	mZenGardenButton->Resize(0, 0, 130, 130);
	mZenGardenButton->mMouseVisible = false;
	mZenGardenButton->mClip = false;
	mZenGardenButton->mTranslateX = 0;
	mZenGardenButton->mTranslateY = 0;

	mOptionsButton = MakeNewButton(
		GameSelector::GameSelector_Options, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_OPTIONS1, 
		Sexy::IMAGE_SELECTORSCREEN_OPTIONS2, 
		Sexy::IMAGE_SELECTORSCREEN_OPTIONS2
	);
	mOptionsButton->Resize(0, 0, Sexy::IMAGE_SELECTORSCREEN_OPTIONS1->mWidth, Sexy::IMAGE_SELECTORSCREEN_OPTIONS1->mHeight + 23);
	mOptionsButton->mClip = false;
	mOptionsButton->mBtnNoDraw = true;
	mOptionsButton->mMouseVisible = false;
	mOptionsButton->mButtonOffsetY = 15;
	mOptionsButton->mTranslateX = 0;
	mOptionsButton->mTranslateY = 0;

	mHelpButton = MakeNewButton(
		GameSelector::GameSelector_Help, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_HELP1, 
		Sexy::IMAGE_SELECTORSCREEN_HELP2, 
		Sexy::IMAGE_SELECTORSCREEN_HELP2
	);
	mHelpButton->Resize(0, 0, Sexy::IMAGE_SELECTORSCREEN_HELP1->mWidth, Sexy::IMAGE_SELECTORSCREEN_HELP1->mHeight + 33);
	mHelpButton->mClip = false;
	mHelpButton->mBtnNoDraw = true;
	mHelpButton->mMouseVisible = false;
	mHelpButton->mButtonOffsetY = 30;
	mHelpButton->mTranslateX = 0;
	mHelpButton->mTranslateY = 0;

	mQuitButton = MakeNewButton(
		GameSelector::GameSelector_Quit, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_QUIT1, 
		Sexy::IMAGE_SELECTORSCREEN_QUIT2, 
		Sexy::IMAGE_SELECTORSCREEN_QUIT2
	);
	mQuitButton->Resize(0, 0, Sexy::IMAGE_SELECTORSCREEN_QUIT1->mWidth + 10, Sexy::IMAGE_SELECTORSCREEN_QUIT1->mHeight + 10);
	mQuitButton->mClip = false;
	mQuitButton->mBtnNoDraw = true;
	mQuitButton->mMouseVisible = false;
	mQuitButton->mButtonOffsetX = 5;
	mQuitButton->mButtonOffsetY = 5;
	mQuitButton->mTranslateX = 0;
	mQuitButton->mTranslateY = 0;

	mChangeUserButton = MakeNewButton(
		GameSelector::GameSelector_ChangeUser, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_BLANK, 
		Sexy::IMAGE_BLANK, 
		Sexy::IMAGE_BLANK
	);
	mChangeUserButton->Resize(0, 0, 250, 30);
	mChangeUserButton->mBtnNoDraw = true;
	mChangeUserButton->mMouseVisible = false;

	mOverlayWidget = new GameSelectorOverlay(this);
	mOverlayWidget->Resize(0, 0, BOARD_WIDTH, BOARD_HEIGHT);

	mStoreButton = MakeNewButton(
		GameSelector::GameSelector_Store, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_STORE, 
		Sexy::IMAGE_SELECTORSCREEN_STOREHIGHLIGHT, 
		Sexy::IMAGE_SELECTORSCREEN_STOREHIGHLIGHT
	);
	mStoreButton->mClip = false;
	mStoreButton->Resize(405, 484, Sexy::IMAGE_SELECTORSCREEN_STORE->mWidth, Sexy::IMAGE_SELECTORSCREEN_STORE->mHeight);
	mStoreButton->mMouseVisible = false;
	mStoreButton->mTranslateX = 0;
	mStoreButton->mTranslateY = 0;
	
	mAlmanacButton = MakeNewButton(
		GameSelector::GameSelector_Almanac, 
		this, 
		_S(""),
		nullptr, 
		Sexy::IMAGE_SELECTORSCREEN_ALMANAC, 
		Sexy::IMAGE_SELECTORSCREEN_ALMANACHIGHLIGHT, 
		Sexy::IMAGE_SELECTORSCREEN_ALMANACHIGHLIGHT
	);
	mAlmanacButton->mClip = false;
	mAlmanacButton->Resize(327, 428, Sexy::IMAGE_SELECTORSCREEN_ALMANAC->mWidth, Sexy::IMAGE_SELECTORSCREEN_ALMANAC->mHeight);
	mAlmanacButton->mMouseVisible = false;
	mAlmanacButton->mTranslateX = 0;
	mAlmanacButton->mTranslateY = 0;

#ifdef _HAS_UNLOCK
	mUnlockButton = MakeNewButton(
		GameSelector::GameSelector_Unlock,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_BLANK
	);
	mUnlockButton->mClip = false;
	mUnlockButton->Resize(700, 342, 60, 65);
	mUnlockButton->mMouseVisible = false;
#endif

	mApp->mMusic->MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);

	mStartingGame = false;
	mStartingGameCounter = 0;
	mMinigamesLocked = false;
	mPuzzleLocked = false;
	mSurvivalLocked = false;
	mUnlockSelectorCheat = false;
	mTrophyParticleID = ParticleSystemID::PARTICLESYSTEMID_NULL;
	mShowStartButton = false;

	Reanimation* aSelectorReanim = mApp->AddReanimation(0.5f, 0.5f, 0, ReanimationType::REANIM_SELECTOR_SCREEN);
	aSelectorReanim->PlayReanim("anim_open", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
	aSelectorReanim->AssignRenderGroupToPrefix("flower", RENDER_GROUP_HIDDEN);
	aSelectorReanim->AssignRenderGroupToPrefix("leaf", RENDER_GROUP_HIDDEN);
	aSelectorReanim->AssignRenderGroupToPrefix("woodsign", RENDER_GROUP_HIDDEN);
	aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_BG", 1);
	aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_BG_Center", 2);
	aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_BG_Left", 3);

	mSelectorReanimID = mApp->ReanimationGetID(aSelectorReanim);
	mSelectorState = SelectorAnimState::SELECTOR_OPEN;
	int aFrameStart, aFrameCount;
	aSelectorReanim->GetFramesForLayer("anim_sign", aFrameStart, aFrameCount);
	aSelectorReanim->mFrameBasePose = aFrameStart + aFrameCount - 1;

	for (int i = 0; i < 6; i++)
	{
		Reanimation* aCloudReanim = mApp->AddReanimation(0.5f, 0.5f, 0, ReanimationType::REANIM_SELECTOR_SCREEN);
		std::string aAnimName = Sexy::StrFormat("anim_cloud%d", (i > 1 ? i + 2 : i + 1));
		aCloudReanim->PlayReanim(aAnimName.c_str(), ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 0.0f);
		mCloudReanimID[i] = mApp->ReanimationGetID(aCloudReanim);
		mCloudCounter[i] = RandRangeInt(-6000, 2000);
		if (mCloudCounter[i] < 0)
		{
			aCloudReanim->mAnimTime = -mCloudCounter[i] / 6000.0f;
			aCloudReanim->mAnimRate = 0.5f;
			mCloudCounter[i] = 0;
		}
		else
			aCloudReanim->mAnimRate = 0.0f;
	}

	for (int i = 0; i < 3; i++)
	{
		Reanimation* aFlowerReanim = mApp->AddReanimation(0.5f, 0.5f, 0, ReanimationType::REANIM_SELECTOR_SCREEN);
		std::string aAnimName = Sexy::StrFormat("anim_flower%d", i + 1);
		aFlowerReanim->PlayReanim(aAnimName.c_str(), ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 0.0f);
		aFlowerReanim->mAnimRate = 0.0f;
		aFlowerReanim->AttachToAnotherReanimation(aSelectorReanim, "SelectorScreen_BG_Right");
		aFlowerReanim->mIsAttachment = false;
		mFlowerReanimID[i] = mApp->ReanimationGetID(aFlowerReanim);

		if (mApp->mFlowersPlucked[i])
			aFlowerReanim->mAnimTime = 1.0f;
	}

	Reanimation* aLeafReanim = mApp->AddReanimation(0.5f, 0.5f, 0, ReanimationType::REANIM_SELECTOR_SCREEN);
	aLeafReanim->PlayReanim("anim_grass", ReanimLoopType::REANIM_LOOP, 0, 6.0f);
	aLeafReanim->mAnimRate = 0.0f;
	mLeafReanimID = mApp->ReanimationGetID(aLeafReanim);
	mLeafCounter = 50;

	int aLeafTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
	ReanimatorTransform aLeafTransform;
	aSelectorReanim->GetCurrentTransform(aLeafTrackIndex, &aLeafTransform);
	aLeafReanim->SetPosition(aLeafTransform.mTransX - 71.0f, aLeafTransform.mTransY - 41.0f);

	Reanimation* aWoodSignReanim = mApp->AddReanimation(0.5f, 0.5f, 0, ReanimationType::REANIM_SELECTOR_SCREEN);
	aWoodSignReanim->PlayReanim("anim_open", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
	aWoodSignReanim->ShowOnlyTrack("");
	aWoodSignReanim->AssignRenderGroupToPrefix("woodsign3", RENDER_GROUP_NORMAL);
	aWoodSignReanim->AssignRenderGroupToPrefix("woodsign2", RENDER_GROUP_NORMAL);
	aWoodSignReanim->AssignRenderGroupToPrefix("woodsign1", RENDER_GROUP_NORMAL);
	mWoodSignID = mApp->ReanimationGetID(aWoodSignReanim);
	aWoodSignReanim->mFrameBasePose = aSelectorReanim->mFrameBasePose;
#ifndef _HAS_ZOMBATAR
	//TODO: replace "woodsign3" with the original 2009 sprite when Zombatar is disabled
#endif

	SyncProfile(false);

	TrackButton(mAdventureButton, mShowStartButton ? "SelectorScreen_StartAdventure_button" : "SelectorScreen_Adventure_button", 0.0f, 0.0f);
	TrackButton(mMinigameButton, "SelectorScreen_Survival_button", 0.0f, 0.0f);
	TrackButton(mPuzzleButton, "SelectorScreen_Challenges_button", 0.0f, 0.0f);
	TrackButton(mSurvivalButton, "SelectorScreen_ZenGarden_button", 0.0f, 0.0f);
	TrackButton(mZenGardenButton, "SelectorScreen_BG_Right", 100.0f, 360.0f);
	TrackButton(mOptionsButton, "SelectorScreen_BG_Right", 494.0f, 434.0f);
	TrackButton(mQuitButton, "SelectorScreen_BG_Right", 644.0f, 469.0f);
	TrackButton(mHelpButton, "SelectorScreen_BG_Right", 576.0f, 458.0f);
	TrackButton(mAlmanacButton, "SelectorScreen_BG_Right", 256.0f, 387.0f);
	TrackButton(mStoreButton, "SelectorScreen_BG_Right", 334.0f, 441.0f);
#ifdef _HAS_ACHIEVEMENTS
	TrackButton(mAchievementsButton, "SelectorScreen_BG_Left", 20.f, 480.f);
	TrackButton(mTrophyButton, "SelectorScreen_BG_Left", 20.0f, 355.0f);
#endif
#ifdef _HAS_MORESCREEN
	TrackButton(mQuickPlayButton, "SelectorScreen_BG_Right", 204, 329);
#endif
#ifdef _HAS_UNLOCK
	TrackButton(mUnlockButton, "SelectorScreen_BG_Right", 629, 301);
#endif

	mApp->PlaySample(Sexy::SOUND_ROLL_IN);

	mSlideCounter = 0;
	mStartX = 0;
	mStartY = 0;
	mDestX = 0;
	mDestY = 0;
#ifdef _HAS_ZOMBATAR
	mZombatarWidget = new ZombatarWidget(this->mApp);
	mZombatarWidget->Move(mApp->mWidth, 0);
#endif
#ifdef _HAS_ACHIEVEMENTS
	mAchievementsWidget = new AchievementsWidget(this->mApp);
	mAchievementsWidget->Move(0, mApp->mHeight);
#endif
#ifdef _HAS_MORESCREEN
	mMoreWidget = new MoreWidget(this->mApp);
	mMoreWidget->Move(-mApp->mWidth, 0);
#endif
#ifdef _HAS_LEVELSELECTOR
	mLevelSelectorWidget = new QuickplayWidget(this->mApp);
	mLevelSelectorWidget->Move(mApp->mWidth, 0);
#endif
	TodHesitationTrace("gameselectorinit");
}

//0x449D00¡¢0x449D20
GameSelector::~GameSelector()
{
	if (mAdventureButton)
		delete mAdventureButton;
	if (mMinigameButton)
		delete mMinigameButton;
	if (mPuzzleButton)
		delete mPuzzleButton;
	if (mOptionsButton)
		delete mOptionsButton;
	if (mQuitButton)
		delete mQuitButton;
	if (mHelpButton)
		delete mHelpButton;
	if (mOverlayWidget)
		delete mOverlayWidget;
	if (mStoreButton)
		delete mStoreButton;
	if (mAlmanacButton)
		delete mAlmanacButton;
	if (mZenGardenButton)
		delete mZenGardenButton;
	if (mSurvivalButton)
		delete mSurvivalButton;
	if (mChangeUserButton)
		delete mChangeUserButton;
#ifdef _HAS_ZOMBATAR
	if (mZombatarButton)
		delete mZombatarButton;
	if (mZombatarWidget)
		delete mZombatarWidget;
#endif
#ifdef _HAS_ACHIEVEMENTS
	if (mAchievementsButton)
		delete mAchievementsButton;
	if (mTrophyButton)
		delete mTrophyButton;
	if (mAchievementsWidget)
		delete mAchievementsWidget;
#endif
#ifdef _HAS_MORESCREEN
	if (mMoreWidget)
		delete mMoreWidget;
	if (mQuickPlayButton)
		delete mQuickPlayButton;
#endif
#ifdef _HAS_LEVELSELECTOR
	if (mLevelSelectorWidget)
		delete mLevelSelectorWidget;
#endif
#ifdef _HAS_UNLOCK
	if (mUnlockButton)
		delete mUnlockButton;
#endif

	delete mToolTip;
}

//0x449E60
void GameSelector::SyncButtons()
{
	bool aAlmanacAvailable = mApp->CanShowAlmanac() || mUnlockSelectorCheat;
	bool aStoreOpen = mApp->CanShowStore() || mUnlockSelectorCheat;
	bool aZenGardenOpen = mApp->CanShowZenGarden() || mUnlockSelectorCheat;

	mAlmanacButton->mDisabled = !aAlmanacAvailable;
	mAlmanacButton->mVisible = aAlmanacAvailable;
	mStoreButton->mDisabled = !aStoreOpen;
	mStoreButton->mVisible = aStoreOpen;
#ifdef _HAS_ZOMBATAR
	mZombatarButton->mDisabled = false; // @Patoke: added these
	mZombatarButton->mVisible = true;
#endif

	Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
	if (aAlmanacAvailable)
	{
		aSelectorReanim->AssignRenderGroupToPrefix("almanac_key_shadow", RENDER_GROUP_NORMAL);
		if (aStoreOpen)
			aSelectorReanim->SetImageOverride("almanac_key_shadow", nullptr);
		else
			aSelectorReanim->SetImageOverride("almanac_key_shadow", Sexy::IMAGE_REANIM_SELECTORSCREEN_ALMANAC_SHADOW);
	}
	else if (aStoreOpen)
	{
		aSelectorReanim->AssignRenderGroupToPrefix("almanac_key_shadow", RENDER_GROUP_NORMAL);
		aSelectorReanim->SetImageOverride("almanac_key_shadow", Sexy::IMAGE_REANIM_SELECTORSCREEN_KEY_SHADOW);
	}
	else
		aSelectorReanim->AssignRenderGroupToPrefix("almanac_key_shadow", RENDER_GROUP_HIDDEN);

	mZenGardenButton->mDisabled = !aZenGardenOpen;
	mZenGardenButton->mVisible = aZenGardenOpen;

	if (mMinigamesLocked)
	{
		//mMinigameButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT;
		//mMinigameButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_BUTTON;
		mMinigameButton->SetColor(ButtonWidget::COLOR_BKG, Color(128, 128, 128));
	}
	else
	{
		//mMinigameButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT;
		//mMinigameButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT;
		mMinigameButton->SetColor(ButtonWidget::COLOR_BKG, Color::White);
	}
	if (mPuzzleLocked)
	{
		//mPuzzleButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT;
		//mPuzzleButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_BUTTON;
		mPuzzleButton->SetColor(ButtonWidget::COLOR_BKG, Color(128, 128, 128));
	}
	else
	{
		//mPuzzleButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT;
		//mPuzzleButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT;
		mPuzzleButton->SetColor(ButtonWidget::COLOR_BKG, Color::White);
	}
	if (mSurvivalLocked)
	{
		//mSurvivalButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT;
		//mSurvivalButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_BUTTON;
		mSurvivalButton->SetColor(ButtonWidget::COLOR_BKG, Color(128, 128, 128));
	}
	else
	{
		//mSurvivalButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT;
		//mSurvivalButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT;
		mSurvivalButton->SetColor(ButtonWidget::COLOR_BKG, Color::White);
	}

	ReanimatorTrackInstance* aMinigameTrack = aSelectorReanim->GetTrackInstanceByName("SelectorScreen_Survival_button");
	ReanimatorTrackInstance* aPuzzleTrack = aSelectorReanim->GetTrackInstanceByName("SelectorScreen_Challenges_button");
	ReanimatorTrackInstance* aSurvivalTrack = aSelectorReanim->GetTrackInstanceByName("SelectorScreen_ZenGarden_button");
	aMinigameTrack->mTrackColor = mMinigameButton->GetColor(ButtonWidget::COLOR_BKG);
	aPuzzleTrack->mTrackColor = mPuzzleButton->GetColor(ButtonWidget::COLOR_BKG);
	aSurvivalTrack->mTrackColor = mSurvivalButton->GetColor(ButtonWidget::COLOR_BKG);

	if (mShowStartButton)
	{
		aSelectorReanim->AssignRenderGroupToPrefix("SelectorScreen_Adventure_", RENDER_GROUP_HIDDEN);
#ifdef DO_FIX_BUGS
		aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_StartAdventure_shadow", RENDER_GROUP_NORMAL);
#endif
		mAdventureButton->mButtonImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_STARTADVENTURE_BUTTON;
		mAdventureButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_STARTADVENTURE_HIGHLIGHT;
		mAdventureButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_STARTADVENTURE_HIGHLIGHT;
	}
	else
	{
		aSelectorReanim->AssignRenderGroupToPrefix("SelectorScreen_StartAdventure_", RENDER_GROUP_HIDDEN);
#ifdef DO_FIX_BUGS
		aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_Adventure_shadow", RENDER_GROUP_NORMAL);
#endif
		mAdventureButton->mButtonImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_BUTTON;
		mAdventureButton->mOverImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_HIGHLIGHT;
		mAdventureButton->mDownImage = Sexy::IMAGE_REANIM_SELECTORSCREEN_ADVENTURE_HIGHLIGHT;
	}
}

//0x44A2E0
void GameSelector::AddTrophySparkle()
{
	TOD_ASSERT(mTrophyParticleID == PARTICLESYSTEMID_NULL);
	float theY = 380.0f;
#ifdef _HAS_ACHIEVEMENTS
	theY -= 45;
#endif
	TodParticleSystem* aTrophyParticle = mApp->AddTodParticle(85.0f, theY, RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_TROPHY_SPARKLE);
	mTrophyParticleID = mApp->ParticleGetID(aTrophyParticle);
}

//0x44A320
void GameSelector::SyncProfile(bool theShowLoading)
{
	if (theShowLoading)
	{
		mLoading = true;

		mApp->DoDialog(Dialogs::DIALOG_MESSAGE, true, _S("[LOADING_LINES]"), _S(""), _S(""), Dialog::BUTTONS_NONE);
		mApp->DrawDirtyStuff();
		mApp->PreloadForUser();
		mApp->KillDialog(Dialogs::DIALOG_MESSAGE);

		mLoading = false;
	}

	TodParticleSystem* aTrophyParticle = mApp->ParticleTryToGet(mTrophyParticleID);
	if (aTrophyParticle)
	{
		aTrophyParticle->ParticleSystemDie();
		mTrophyParticleID = ParticleSystemID::PARTICLESYSTEMID_NULL;
	}

	mLevel = 1;
	if (mApp->mPlayerInfo)
		mLevel = mApp->mPlayerInfo->GetLevel();
	mApp->mPlayerLevelRef = -1;
	mShowStartButton = true;
	mMinigamesLocked = true;
	mPuzzleLocked = true;
	mSurvivalLocked = true;
	if (mApp->mPlayerInfo && !mApp->IsIceDemo())
	{
		// @Inliothixi: implemented
		if (mApp->SaveFileExists() || mApp->HasFinishedAdventure() || mApp->mPlayerInfo->mLevel > 1)
			mShowStartButton = false;

		if (mApp->HasFinishedAdventure())
		{
			mMinigamesLocked = false;
			mSurvivalLocked = false;
			mPuzzleLocked = false;
			mShowStartButton = false;
		}

		if (mApp->mPlayerInfo->mHasUnlockedMinigames)
			mMinigamesLocked = false;
		if (mApp->mPlayerInfo->mHasUnlockedPuzzleMode)
			mPuzzleLocked = false;
		if (mApp->mPlayerInfo->mHasUnlockedSurvivalMode)
			mSurvivalLocked = false;

		if (mApp->IsTrialStageLocked())
		{
			mPuzzleLocked = true;
			mSurvivalLocked = true;
		}
	}

	if (mApp->HasFinishedAdventure() && !mApp->IsTrialStageLocked())
		mHasTrophy = true;
	else
		mHasTrophy = false;
	if (mHasTrophy && mSelectorState != SelectorAnimState::SELECTOR_OPEN)
		AddTrophySparkle();

	SyncButtons();
	AlmanacInitForPlayer();
	BoardInitForPlayer();
	ReportAchievement::AchievementInitForPlayer(mApp);
}

//0x44A650
void GameSelector::Draw(Graphics* g)
{
	if (mApp->GetDialog(Dialogs::DIALOG_STORE) || mApp->GetDialog(Dialogs::DIALOG_ALMANAC))
		return;

	g->PushState();
	g->ClearClipRect();
	g->SetLinearBlend(true);
	Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);

	aSelectorReanim->DrawRenderGroup(g, 1);  // "SelectorScreen_BG"

	for (int i = 0; i < 6; i++)
	{
		Reanimation* aCloudReanim = mApp->ReanimationTryToGet(mCloudReanimID[i]);
		if (aCloudReanim)
			aCloudReanim->Draw(g);
	}

	aSelectorReanim->DrawRenderGroup(g, 2);  // "SelectorScreen_BG_Center"
	aSelectorReanim->DrawRenderGroup(g, 3); // "SelectorScreen_BG_Left"
	//{
	//	Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
	//	int aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Left");
	//	ReanimatorTransform aTransform;
	//	aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);

	//	int drawPosX = (int)(aTransform.mTransX + 75);
	//	int drawPosY = (int)(aTransform.mTransY + 460);

	//	g->DrawImageF(IMAGE_SELECTOR_FENCE, drawPosX, drawPosY);
	//}

#ifdef _HAS_MORESCREEN
	int aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
	ReanimatorTransform aTransform;
	aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);

	int drawPosX = (int)(aTransform.mTransX + 179);
	int drawPosY = (int)(aTransform.mTransY + 314);

	g->DrawImageF(IMAGE_SELECTOR_MORE_SIGN, drawPosX, drawPosY);
#endif

	aSelectorReanim->DrawRenderGroup(g, RENDER_GROUP_NORMAL);

	Reanimation* aWoodSignReanim = mApp->ReanimationGet(mWoodSignID);
	aWoodSignReanim->DrawRenderGroup(g, RENDER_GROUP_NORMAL);

	if (mSelectorState == SelectorAnimState::SELECTOR_OPEN)
	{
		int aBGIdx = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
		ReanimatorTransform aTransform;
		aSelectorReanim->GetCurrentTransform(aBGIdx, &aTransform);
		float aFractionalOffsetX = fmod(aTransform.mTransX, 1.0f);
		float aFractionalOffsetY = fmod(aTransform.mTransY, 1.0f);
		g->DrawImageF(
			mOptionsButton->mButtonImage,
			mOptionsButton->mX + mOptionsButton->mButtonOffsetX + aFractionalOffsetX,
			mOptionsButton->mY + mOptionsButton->mButtonOffsetY + aFractionalOffsetY
		);
		g->DrawImageF(
			mQuitButton->mButtonImage,
			mQuitButton->mX + mQuitButton->mButtonOffsetX + aFractionalOffsetX,
			mQuitButton->mY + mQuitButton->mButtonOffsetY + aFractionalOffsetY
		);
		g->DrawImageF(
			mHelpButton->mButtonImage,
			mHelpButton->mX + mHelpButton->mButtonOffsetX + aFractionalOffsetX,
			mHelpButton->mY + mHelpButton->mButtonOffsetY + aFractionalOffsetY
		);
	}

	if (mApp->mPlayerInfo && mApp->mPlayerInfo->mName.size() &&
		mSelectorState != SelectorAnimState::SELECTOR_OPEN && mSelectorState != SelectorAnimState::SELECTOR_NEW_USER)
	{
		SexyString aWelcomeStr = mApp->mPlayerInfo->mName + _S('!');

		int aSignIdx = aWoodSignReanim->FindTrackIndex("woodsign1");
		SexyTransform2D aOverlayMatrix;
		aWoodSignReanim->GetAttachmentOverlayMatrix(aSignIdx, aOverlayMatrix);
		float aStringWidth = Sexy::FONT_BRIANNETOD16->StringWidth(aWelcomeStr);
		SexyTransform2D aOffsetMatrix;
		// @Patoke: add position so it moves when sliding to position
		aOffsetMatrix.Translate(170.5f - (int)(aStringWidth * 0.5f) + mX, 102.5f + mY);
		TodDrawStringMatrix(g, Sexy::FONT_BRIANNETOD16, aOverlayMatrix * aOffsetMatrix, aWelcomeStr, Color(255, 245, 200));
	}
	g->PopState();
}

//0x44AB50
void GameSelector::DrawOverlay(Graphics* g)
{
	g->SetLinearBlend(true);

#ifdef _HAS_UNLOCK
	{
		g->PushState();

		Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
		int aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
		ReanimatorTransform aTransform;
		aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);
		g->TranslateF(691.5f, aTransform.mTransY - 41.0f + 329);
		bool alreadyPressed = mApp->mPlayerInfo && mApp->mPlayerInfo->mHasUsedCheatKeys;
		Sexy::Image* buttonImg = alreadyPressed ? IMAGE_UNLOCK_BUTTON_PRESSED : mUnlockButton->mIsOver ? Sexy::IMAGE_UNLOCK_BUTTON_HIGHLIGHT : Sexy::IMAGE_UNLOCK_BUTTON;
		if (!alreadyPressed && mUnlockButton->mIsDown)
		{
			g->DrawImageF(buttonImg, 0, 0);
			g->TranslateF(-1, 0);
		}
		g->DrawImageF(buttonImg, 0, 0);
		g->PopState();
	}
#endif

	for (int i = 0; i < 3; i++)
		mApp->ReanimationGet(mFlowerReanimID[i])->Draw(g);

	if (mApp->mPlayerInfo != nullptr)
	{
		if (!mApp->IsIceDemo() && !mShowStartButton)
		{
			int aOffsetX, aOffsetY;
			if (mAdventureButton->mIsDown && mAdventureButton->mIsOver)
			{
				aOffsetX = 1;
				aOffsetY = 1;
			}
			else
			{
				aOffsetX = 0;
				aOffsetY = 0;
			}

			Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
			int aRightIdx = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
			ReanimatorTransform aTransform;
			aSelectorReanim->GetCurrentTransform(aRightIdx, &aTransform);
			float aTransAreaX = aTransform.mTransX + aOffsetX;
			float aTransAreaY = aTransform.mTransY + aOffsetY;
			float aTransSubX = aTransAreaX;
			float aTransSubY = aTransAreaY;

			int aStage;
			if (mLevel >= 0) {
				aStage = ClampInt((mLevel - 1) / 10 + 1, 1, 6);  // 大关
			}
			else {
				aStage = ClampInt((mLevel - 1) / 10 + 1, -6, -1);
			}

			int aSub;
			if (aStage == 6) {
				aSub = abs(mLevel) - (6 - 1) * 10;
			}
			else {
				aSub = abs(mLevel) - (abs(aStage) - 1) * 10;
			}
			//int aSub = mLevel - (aStage - 1) * 10;  // 小关
			if (mApp->IsTrialStageLocked() && (mLevel >= 25 || mApp->HasFinishedAdventure()))
			{
				aStage = 3;
				aSub = 4;
			}
			else
			{
				if (aStage == 1)
				{
					aTransAreaY += 1.0f;
				}
				else if (aStage == 4)
				{
					aTransAreaX -= 1.0f;
				}
				if (aSub == 3)
				{
					aTransSubX -= 1.0f;
				}
			}

			g->SetColorizeImages(true);
			g->SetColor(mAdventureButton->mColors[ButtonWidget::COLOR_BKG]);
#ifdef _GOTY
			// @Patoke: changed positions for GOTY adventure icon
			TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransAreaX + 486.0f, aTransAreaY + 47.f, abs(aStage), 0);  // 绘制大关数
			if (aSub < 10)
			{
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 509.f, aTransSubY + 50.f, abs(aSub), 0);
			}
			/*else if (aSub >= 10)
			{
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 509.f, aTransSubY + 50.f, 1, 0);
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 518.f, aTransSubY + 51.f, 0, 0);
			}*/
			else
			{
				int count = 0;
				int tempSub = abs(aSub);

				while (tempSub > 0) {
					tempSub /= 10;
					count++;
				}

				if (aSub == 0) {
					count = 1;
				}

				for (int _i = 0; _i < count; _i++) {
					int digit = (aSub / (int)pow(10, count - _i - 1)) % 10;

					TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 509.f + 9 * _i, aTransSubY + 50.f + _i, digit, 0);
				}
			}
#else
			TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransAreaX + 486.0f, aTransAreaY + 125.0f, aStage, 0);  // ���ƴ����
			if (aSub < 10)
			{
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 504.0f, aTransSubY + 128.0f, aSub, 0);
			}
			/*else if (aSub == 10)
			{
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 504.0f, aTransSubY + 128.0f, 1, 0);
				TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 513.0f, aTransSubY + 129.0f, 0, 0);
			}*/
			else
			{
				int count = 0;
				int tempSub = abs(aSub);

				while (tempSub > 0) {
					tempSub /= 10;
					count++;
				}

				if (aSub == 0) {
					count = 1;
				}

				for (int _i = 0; _i < count; _i++) {
					int digit = (aSub / (int)pow(10, count - _i - 1)) % 10;

					TodDrawImageCelF(g, Sexy::IMAGE_SELECTORSCREEN_LEVELNUMBERS, aTransSubX + 504.0f + 9 * _i, aTransSubY + 128.0f + _i, digit, 0);
				}
			}
#endif
			g->SetColorizeImages(false);
		}


		if (mZenGardenButton->mVisible && mApp->mZenGarden->PlantsNeedWater())
		{
			g->DrawImage(Sexy::IMAGE_PLANTSPEECHBUBBLE, mZenGardenButton->mX + 106, mZenGardenButton->mY + 36);
			g->DrawImage(Sexy::IMAGE_WATERDROP, mZenGardenButton->mX + 123, mZenGardenButton->mY + 45);
		}
	}


	Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
	int aLeftIdx = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Left");
	ReanimatorTransform aTransformLeft;
	aSelectorReanim->GetCurrentTransform(aLeftIdx, &aTransformLeft);

	if (mHasTrophy)
	{
		float theX = aTransformLeft.mTransX + 10.0f;
		float theY = aTransformLeft.mTransY + 390.0f;

#ifdef _HAS_ACHIEVEMENTS
		theX += 2.0f;
		theY -= 45;
#endif
		if (mApp->EarnedGoldTrophy())
			TodDrawImageCelF(g, Sexy::IMAGE_SUNFLOWER_TROPHY, theX, theY, 1, 0);
		else
			TodDrawImageCelF(g, Sexy::IMAGE_SUNFLOWER_TROPHY, theX, theY, 0, 0);

		TodParticleSystem* aTrophyParticle = mApp->ParticleTryToGet(mTrophyParticleID);
		if (aTrophyParticle)
			aTrophyParticle->Draw(g);
	}

	if (mWidgetManager->mFocusWidget == this)
		mToolTip->Draw(g);

	Reanimation* aHandReanim = mApp->ReanimationTryToGet(mHandReanimID);
	if (aHandReanim)
	{
		g->PushState();
		g->SetClipRect(0, 0, BOARD_WIDTH, BOARD_HEIGHT - 40);
		aHandReanim->Draw(g);
		g->PopState();
	}

	mApp->ReanimationGet(mLeafReanimID)->Draw(g);

#ifndef _DEBUG 
	if (mApp->mBetaValidate)
#endif
	{
		g->SetFont(Sexy::FONT_BRIANNETOD16);
		g->SetColor(Color(200, 200, 200, 255));

		Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
		int aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
		ReanimatorTransform aTransform;
		aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);

		int posX = (int)(aTransform.mTransX);
		int posY = (int)(aTransform.mTransY);

		if (gIsPartnerBuild)
			g->DrawString(TodStringTranslate(_S("[PREVIEW_BUILD]")), posX + 27 - 71, posY + 594 - 41);
		else
			g->DrawString(TodStringTranslate(_S("[BETA_BUILD]")), posX + 27 - 71, posY + 594 - 41);
	}

	Reanimation* aSpotLightReanim = mApp->ReanimationTryToGet(mSpotLightID);
	if (aSpotLightReanim)
	{
		if (aSpotLightReanim->mLoopCount == 0)
		{
			g->PushState();
			g->SetClipRect(0, 0, BOARD_WIDTH, BOARD_HEIGHT);
			aSpotLightReanim->Draw(g);
			g->PopState();
		}
		else
		{
			g->PushState();
			g->SetColorizeImages(true);
			g->SetColor(Color::Black);
			g->FillRect(Rect(0, 0, BOARD_WIDTH, BOARD_HEIGHT));
			g->PopState();
		}
	} 
}

//0x44B0D0
void GameSelector::UpdateTooltip()
{
	if (!mApp->HasFinishedAdventure() || mApp->GetDialog(Dialogs::DIALOG_MESSAGE) || mWidgetManager->mFocusWidget != this)
		return;

	if (mHasTrophy)
	{
		int aMouseX = mX + mApp->mWidgetManager->mLastMouseX;
		int aMouseY = mY + mApp->mWidgetManager->mLastMouseY;
		if (aMouseX >= mX + 50 && aMouseX < mX + 135 && aMouseY >= mY + 280 && aMouseY <= mY + 505
#ifdef _HAS_ACHIEVEMENTS 
			|| mTrophyButton && mTrophyButton->mIsOver
#endif 
		)
		{
			if (mApp->EarnedGoldTrophy())
			{
				mToolTip->SetLabel(LawnApp::Pluralize(mApp->mPlayerInfo->mFinishedAdventure, _S("[GOLD_SUNFLOWER_TOOLTIP]"), _S("[GOLD_SUNFLOWER_TOOLTIP_PLURAL]")));
				mToolTip->mX = mX + 32;
#ifdef _HAS_ACHIEVEMENTS 
				mToolTip->mY = mY + 465;
#else
				mToolTip->mY = mY + 510;
#endif
				mToolTip->mVisible = true;
			}
			else
			{
				mToolTip->SetLabel(_S("[SILVER_SUNFLOWER_TOOLTIP]"));
				mToolTip->mX = mX + 20;
#ifdef _HAS_ACHIEVEMENTS 
				mToolTip->mY = mY + 450;
#else
				mToolTip->mY = mY + 495;
#endif
				mToolTip->mVisible = true;
			}
			return;
		}
	}

	mToolTip->mVisible = false;
	mToolTip->Update();
}

//0x44B2A0
void GameSelector::Update()
{
	Widget::Update();
	MarkDirty();
	UpdateTooltip();
	// @Patoke: implemented this
	if (mSlideCounter > 0) {
		int aNewX = TodAnimateCurve(75, 0, mSlideCounter, mStartX, mDestX, TodCurves::CURVE_EASE_IN_OUT);
		int aNewY = TodAnimateCurve(75, 0, mSlideCounter, mStartY, mDestY, TodCurves::CURVE_EASE_IN_OUT);
		Move(aNewX, aNewY);

		// @Patoke: not from the original binaries but fixes bugs
		mOverlayWidget->Move(aNewX, aNewY);
#ifdef _HAS_ZOMBATAR
		mZombatarWidget->Move(aNewX + mApp->mWidth, aNewY);
#endif
#ifdef _HAS_ACHIEVEMENTS
		mAchievementsWidget->Move(aNewX, aNewY + mApp->mHeight);
#endif
#ifdef _HAS_MORESCREEN
		mMoreWidget->Move(aNewX - mApp->mWidth, aNewY);
#endif
		mAdventureButton->SetOffset(aNewX, aNewY);
		mMinigameButton->SetOffset(aNewX, aNewY);
		mPuzzleButton->SetOffset(aNewX, aNewY);
		mOptionsButton->SetOffset(aNewX, aNewY + 15);
		mQuitButton->SetOffset(aNewX + 5, aNewY + 5);
		mHelpButton->SetOffset(aNewX, aNewY + 30);
		mStoreButton->SetOffset(aNewX, aNewY);
		mAlmanacButton->SetOffset(aNewX, aNewY);
		mZenGardenButton->SetOffset(aNewX, aNewY);
		mSurvivalButton->SetOffset(aNewX, aNewY);
		mChangeUserButton->SetOffset(aNewX, aNewY);
#ifdef _HAS_ZOMBATAR
		mZombatarButton->SetOffset(aNewX, aNewY);
#endif
#ifdef _HAS_ACHIEVEMENTS
		mAchievementsButton->SetOffset(aNewX, aNewY);
		mTrophyButton->SetOffset(aNewX, aNewY);
#endif
#ifdef _HAS_MORESCREEN
		mQuickPlayButton->SetOffset(aNewX, aNewY);
#endif
#ifdef _HAS_UNLOCK
		mUnlockButton->SetOffset(aNewX, aNewY);
#endif

#ifdef _HAS_LEVELSELECTOR
		mLevelSelectorWidget->Move(aNewX + mApp->mWidth, aNewY);
		// @Patoke: make sure these are drawn even outside of bounds (force redraw)
#endif
#ifdef _HAS_ACHIEVEMENTS
		mAchievementsButton->MarkDirty();
		mTrophyButton->MarkDirty();
#endif
#ifdef _HAS_ZOMBATAR
		mZombatarButton->MarkDirty();
#endif
#ifdef _HAS_MORESCREEN
		mQuickPlayButton->MarkDirty();
#endif
		mOptionsButton->MarkDirty();
		mHelpButton->MarkDirty();
		mQuitButton->MarkDirty();
		mStoreButton->MarkDirty();
		mZenGardenButton->MarkDirty();

		mSlideCounter--;
	}
#ifdef _HAS_ACHIEVEMENTS
	mAchievementsWidget->MarkDirty();
#endif
#ifdef _HAS_ZOMBATAR
	mZombatarWidget->MarkDirty();
#endif
#ifdef _HAS_MORESCREEN
	mMoreWidget->MarkDirty();
#endif
#ifdef _HAS_LEVELSELECTOR
	mLevelSelectorWidget->MarkDirty();
#endif
	mApp->mZenGarden->UpdatePlantNeeds();

	TodParticleSystem* aParticle = mApp->ParticleTryToGet(mTrophyParticleID);
	if (aParticle)
		aParticle->Update();

	if (mStartingGame)
	{
		mStartingGameCounter++;
		if (mStartingGameCounter > 450)
		{
			mApp->KillGameSelector();

			if (mApp->IsIceDemo())
			{
				mApp->PreNewGame(GameMode::GAMEMODE_CHALLENGE_ICE, false);
				return;
			}
			if (mApp->IsFirstTimeAdventureMode() && mLevel == 1 && !mApp->SaveFileExists())
			{
				mApp->PreNewGame(GameMode::GAMEMODE_INTRO, false);
				return;
			}
			if (mApp->mPlayerInfo->mNeedsMagicTacoReward && mLevel == 35)
			{
				StoreScreen* aStore = mApp->ShowStoreScreen();
				aStore->SetupForIntro(601);
				aStore->WaitForResult(true);
				mApp->PreNewGame(GameMode::GAMEMODE_ADVENTURE, false);
				return;
			}
			if (ShouldDoZenTuturialBeforeAdventure())
			{
				mApp->PreNewGame(GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN, false);
				mApp->mZenGarden->SetupForZenTutorial();
				return;
			}
			mApp->PreNewGame(GameMode::GAMEMODE_ADVENTURE, true);
			
			return;
		}
		mAdventureButton->SetColor(ButtonWidget::COLOR_BKG, mStartingGameCounter % 20 < 10 ? Color(80, 80, 80) : Color::White);
		if (mStartingGameCounter == 125)
			mApp->PlaySample(Sexy::SOUND_EVILLAUGH);
	}

	Reanimation* aSelectorReanim = mApp->ReanimationTryToGet(mSelectorReanimID);

	if (aSelectorReanim)
	{
		int aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
		ReanimatorTransform aTransform;
		aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);
		aTransform.mTransY += mOverlayWidget->mY;
		aTransform.mTransY -= 41.0f;
#ifdef _HAS_ZOMBATAR
		mZombatarWidget->mY = aTransform.mTransY;
#endif
#ifdef _HAS_LEVELSELECTOR
		mLevelSelectorWidget->mY = aTransform.mTransY;
		mAchievementsWidget->mY = aTransform.mTransY + BOARD_HEIGHT;
#endif
		
		aTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Left");
		aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);
		aTransform.mTransY += mOverlayWidget->mY;
#ifdef _HAS_MORESCREEN
		mMoreWidget->mY = aTransform.mTransY + 80.0f;
#endif
	}

	Reanimation* aWoodSignReanim = mApp->ReanimationGet(mWoodSignID);

	switch (mSelectorState)
	{
	case SelectorAnimState::SELECTOR_OPEN:
		if (mWidgetManager)
			mWidgetManager->RehupMouse();
#ifdef DO_FIX_BUGS
		aSelectorReanim->AssignRenderGroupToPrefix("SelectorScreen_Adventure_button", RENDER_GROUP_HIDDEN);
		aSelectorReanim->AssignRenderGroupToPrefix("SelectorScreen_StartAdventure_button", RENDER_GROUP_HIDDEN);
		mAdventureButton->mBtnNoDraw = false;
#endif
		if (aSelectorReanim->mLoopCount > 0)
		{
			aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_Adventure_button", RENDER_GROUP_HIDDEN);
			aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_StartAdventure_button", RENDER_GROUP_HIDDEN);
			aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_Survival_button", RENDER_GROUP_HIDDEN);
			aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_Challenges_button", RENDER_GROUP_HIDDEN);
			aSelectorReanim->AssignRenderGroupToTrack("SelectorScreen_ZenGarden_button", RENDER_GROUP_HIDDEN);
			mAdventureButton->mBtnNoDraw = false;
			mMinigameButton->mBtnNoDraw = false;
			mPuzzleButton->mBtnNoDraw = false;
			mSurvivalButton->mBtnNoDraw = false;
			mZenGardenButton->mBtnNoDraw = false;
			mHelpButton->mBtnNoDraw = false;
			mOptionsButton->mBtnNoDraw = false;
			mQuitButton->mBtnNoDraw = false;
#ifdef _HAS_ZOMBATAR
			mZombatarButton->mBtnNoDraw = false; // @Patoke: new widgets
#endif
#ifdef _HAS_ACHIEVEMENTS
			mAchievementsButton->mBtnNoDraw = false;
			mTrophyButton->mBtnNoDraw = false;
#endif
#ifdef _HAS_MORESCREEN
			mQuickPlayButton->mBtnNoDraw = false;
#endif
			mAdventureButton->mMouseVisible = true;
			mMinigameButton->mMouseVisible = true;
			mPuzzleButton->mMouseVisible = true;
			mSurvivalButton->mMouseVisible = true;
			mZenGardenButton->mMouseVisible = true;
			mHelpButton->mMouseVisible = true;
			mOptionsButton->mMouseVisible = true;
			mQuitButton->mMouseVisible = true;
			mStoreButton->mMouseVisible = true;
			mAlmanacButton->mMouseVisible = true;
			mChangeUserButton->mMouseVisible = true;
#ifdef _HAS_ZOMBATAR
			mZombatarButton->mMouseVisible = true; // @Patoke: new widgets
#endif
#ifdef _HAS_ACHIEVEMENTS
			mAchievementsButton->mMouseVisible = true;
			mTrophyButton->mMouseVisible = true;
#endif
#ifdef _HAS_MORESCREEN
			mQuickPlayButton->mMouseVisible = true;
#endif
#ifdef _HAS_UNLOCK
			mUnlockButton->mMouseVisible = true;
#endif

			
			if (mApp->mPlayerInfo == nullptr)
			{
				mApp->DoCreateUserDialog();
				if (gIsPartnerBuild)
					AddPreviewProfiles();

				mSelectorState = SelectorAnimState::SELECTOR_NEW_USER;
			}
			else
			{
				aSelectorReanim->PlayReanim("anim_sign", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
				aWoodSignReanim->PlayReanim("anim_sign", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
				mSelectorState = SelectorAnimState::SELECTOR_IDLE;
			}

			if (mHasTrophy)
				AddTrophySparkle();

			if (mApp->mPlayerInfo && mApp->mPlayerInfo->mNeedsMessageOnGameSelector)
			{
				mApp->mPlayerInfo->mNeedsMessageOnGameSelector = 0;
				mApp->WriteCurrentUserConfig();
				mApp->LawnMessageBox(
					Dialogs::DIALOG_MESSAGE, 
					_S("[ADVENTURE_COMPLETE_HEADER]"), 
					_S("[ADVENTURE_COMPLETE_BODY]"), 
					_S("[DIALOG_BUTTON_OK]"), 
					_S(""), 
					Dialog::BUTTONS_FOOTER
				);
			}
		}
		break;
	case SelectorAnimState::SELECTOR_NEW_USER:
		if (mApp->GetDialog(Dialogs::DIALOG_CREATEUSER) == nullptr)
		{
			aSelectorReanim->PlayReanim("anim_sign", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
			mSelectorState = SelectorAnimState::SELECTOR_SHOW_SIGN;
			aWoodSignReanim->PlayReanim("anim_sign", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 30.0f);
		}
		break;
	case SelectorAnimState::SELECTOR_SHOW_SIGN:
		if (aSelectorReanim->mLoopCount > 0)
		{
			mSelectorState = SelectorAnimState::SELECTOR_IDLE;
		}

		break;
#ifdef _HAS_ANIMATED_WOOD_SIGN
	case SelectorAnimState::SELECTOR_IDLE:
		if (aWoodSignReanim->mLoopCount > 0)
		{
			aWoodSignReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 10, 6.0f);
		}
		break;
#endif
	}

	for (int i = 0; i < 6; i++)
	{
		Reanimation* aCloudReanim = mApp->ReanimationGet(mCloudReanimID[i]);
		if (mCloudCounter[i] > 0)
		{
			if (--mCloudCounter[i] == 0)
			{
				aCloudReanim->mLoopCount = 0;
				aCloudReanim->mAnimTime = 0.0f;
				aCloudReanim->mAnimRate = 0.5f;
			}
		}
		else if (aCloudReanim->mLoopCount > 0)
			mCloudCounter[i] = RandRangeInt(2000, 4000);
	}
	aSelectorReanim->Update();
	aWoodSignReanim->Update();

	Reanimation* aLeafReanim = mApp->ReanimationGet(mLeafReanimID);
	aLeafReanim->Update();
	int aLeafTrackIndex = aSelectorReanim->FindTrackIndex("SelectorScreen_BG_Right");
	ReanimatorTransform aLeafTransform;
	aSelectorReanim->GetCurrentTransform(aLeafTrackIndex, &aLeafTransform);
	aLeafReanim->SetPosition(aLeafTransform.mTransX - 71.0f, aLeafTransform.mTransY - 41.0f);
	if (--mLeafCounter == 0)
	{
		float aRate = RandRangeFloat(3.0f, 12.0f);
		aLeafReanim->PlayReanim("anim_grass", ReanimLoopType::REANIM_LOOP, 20, aRate);
		mLeafCounter = RandRangeInt(200, 400);
	}

	for (int i = 0; i < 6; i++)
		mApp->ReanimationGet(mCloudReanimID[i])->Update();

	Reanimation* aHandReanim = mApp->ReanimationTryToGet(mHandReanimID);
	if (aHandReanim)
	{
		aHandReanim->Update();
	}

	Reanimation* aSpotLight = mApp->ReanimationTryToGet(mSpotLightID);
	if (aSpotLight)
	{
		aSpotLight->Update();
	}

	TrackButton(mAdventureButton, mShowStartButton ? "SelectorScreen_StartAdventure_button" : "SelectorScreen_Adventure_button", 0.0f, 0.0f);
	TrackButton(mMinigameButton, "SelectorScreen_Survival_button", 0.0f, 0.0f);
	TrackButton(mPuzzleButton, "SelectorScreen_Challenges_button", 0.0f, 0.0f);
	TrackButton(mSurvivalButton, "SelectorScreen_ZenGarden_button", 0.0f, 0.0f);
	TrackButton(mZenGardenButton, "SelectorScreen_BG_Right", 100.0f, 360.0f);
	TrackButton(mOptionsButton, "SelectorScreen_BG_Right", 494.0f, 434.0f);
	TrackButton(mQuitButton, "SelectorScreen_BG_Right", 644.0f, 469.0f);
	TrackButton(mHelpButton, "SelectorScreen_BG_Right", 576.0f, 458.0f);
	TrackButton(mAlmanacButton, "SelectorScreen_BG_Right", 256.0f, 387.0f);
	TrackButton(mStoreButton, "SelectorScreen_BG_Right", 334.0f, 441.0f);
#ifdef _HAS_UNLOCK
	TrackButton(mUnlockButton, "SelectorScreen_BG_Right", 629, 301);
#endif
	//TrackButton(mChangeUserButton, "woodsign2", 24.0f, 10.0f);
	//TrackButton(mZombatarButton, "woodsign3", 0.f, 0.f); // @Patoke: add shart here

	{
		Reanimation* aWoodSignReanim = mApp->ReanimationGet(mWoodSignID);
		int aTrackIndex = aWoodSignReanim->FindTrackIndex("woodsign2");
		ReanimatorTransform aTransform;
		aWoodSignReanim->GetCurrentTransform(aTrackIndex, &aTransform);

		mChangeUserButton->mX = (int)(aTransform.mTransX + 24.0f);
		mChangeUserButton->mY = (int)(aTransform.mTransY + 10.0f);
	}

	{
		Reanimation* aWoodSignReanim = mApp->ReanimationGet(mWoodSignID);
		int aTrackIndex = aWoodSignReanim->FindTrackIndex("woodsign3");
		ReanimatorTransform aTransform;
		aWoodSignReanim->GetCurrentTransform(aTrackIndex, &aTransform);

#ifdef _HAS_ZOMBATAR
		
			mZombatarButton->mX = (int)(aTransform.mTransX);
			mZombatarButton->mY = (int)(aTransform.mTransY);
#endif
	}
#ifdef _HAS_ACHIEVEMENTS
	TrackButton(mAchievementsButton, "SelectorScreen_BG_Left", 20.f, 480.f);
	TrackButton(mTrophyButton, "SelectorScreen_BG_Left", 20.0f, 355.0f);
#endif
#ifdef _HAS_MORESCREEN
	TrackButton(mQuickPlayButton, "SelectorScreen_BG_Right", 204, 329);
#endif
	aWoodSignReanim->SetImageOverride("woodsign2", (mChangeUserButton->mIsOver || mChangeUserButton->mIsDown) ? Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN2_PRESS : nullptr);
#ifdef _HAS_ZOMBATAR
	aWoodSignReanim->SetImageOverride("woodsign3", (mZombatarButton->mIsOver || mZombatarButton->mIsDown) ? Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS : nullptr);
#endif

#ifdef _HAS_ACHIEVEMENTS
	if (mWidgetManager->mFocusWidget != this)
	{
		mAchievementsButton->SetDisabled(true);
	}
	else
	{
		const bool isTrophyOver = Rect(mTrophyButton->mX, mTrophyButton->mY, mTrophyButton->mWidth, mTrophyButton->mHeight).Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY);
		mAchievementsButton->mMouseVisible = !isTrophyOver;
		mAchievementsButton->mIsOver = isTrophyOver ? false : mAchievementsButton->mIsOver;
		mAchievementsButton->SetDisabled(isTrophyOver);
	}
#endif
	
}

//0x44BB20
void GameSelector::TrackButton(DialogButton* theButton, const char* theTrackName, float theOffsetX, float theOffsetY)
{
	Reanimation* aSelectorReanim = mApp->ReanimationGet(mSelectorReanimID);
	int aTrackIndex = aSelectorReanim->FindTrackIndex(theTrackName);
	ReanimatorTransform aTransform;
	aSelectorReanim->GetCurrentTransform(aTrackIndex, &aTransform);
	
	theButton->mX = (int)(aTransform.mTransX + theOffsetX);
	theButton->mY = (int)(aTransform.mTransY + theOffsetY);
}

//0x44BBC0
void GameSelector::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);

	theWidgetManager->AddWidget(mAdventureButton);
	theWidgetManager->AddWidget(mMinigameButton);
	theWidgetManager->AddWidget(mPuzzleButton);
	theWidgetManager->AddWidget(mOptionsButton);
	theWidgetManager->AddWidget(mQuitButton);
	theWidgetManager->AddWidget(mHelpButton);
	theWidgetManager->AddWidget(mStoreButton);
	theWidgetManager->AddWidget(mAlmanacButton);
	theWidgetManager->AddWidget(mSurvivalButton);
	theWidgetManager->AddWidget(mZenGardenButton);
	theWidgetManager->AddWidget(mChangeUserButton);
	theWidgetManager->AddWidget(mOverlayWidget);
#ifdef _HAS_ZOMBATAR
	theWidgetManager->AddWidget(mZombatarButton); // @Patoke: add new widgets
	theWidgetManager->AddWidget(mZombatarWidget);
#endif
#ifdef _HAS_ACHIEVEMENTS
	theWidgetManager->AddWidget(mAchievementsButton);
	theWidgetManager->AddWidget(mTrophyButton);
	theWidgetManager->AddWidget(mAchievementsWidget);
#endif
#ifdef _HAS_MORESCREEN
	theWidgetManager->AddWidget(mQuickPlayButton);
	theWidgetManager->AddWidget(mMoreWidget);
#endif
#ifdef _HAS_LEVELSELECTOR
	theWidgetManager->AddWidget(mLevelSelectorWidget);
#endif
#ifdef _HAS_UNLOCK
	theWidgetManager->AddWidget(mUnlockButton);
#endif
}

//0x44BCA0
void GameSelector::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	theWidgetManager->RemoveWidget(mAdventureButton);
	theWidgetManager->RemoveWidget(mMinigameButton);
	theWidgetManager->RemoveWidget(mPuzzleButton);
	theWidgetManager->RemoveWidget(mOptionsButton);
	theWidgetManager->RemoveWidget(mQuitButton);
	theWidgetManager->RemoveWidget(mHelpButton);
	theWidgetManager->RemoveWidget(mStoreButton);
	theWidgetManager->RemoveWidget(mAlmanacButton);
	theWidgetManager->RemoveWidget(mSurvivalButton);
	theWidgetManager->RemoveWidget(mZenGardenButton);
	theWidgetManager->RemoveWidget(mChangeUserButton);
	theWidgetManager->RemoveWidget(mOverlayWidget);
#ifdef _HAS_ZOMBATAR
	theWidgetManager->RemoveWidget(mZombatarButton); // @Patoke: new widgets
	theWidgetManager->RemoveWidget(mZombatarWidget);
#endif
#ifdef _HAS_ACHIEVEMENTS
	theWidgetManager->RemoveWidget(mAchievementsButton);
	theWidgetManager->RemoveWidget(mTrophyButton);
	theWidgetManager->RemoveWidget(mAchievementsWidget);
#endif
#ifdef _HAS_MORESCREEN
	theWidgetManager->RemoveWidget(mQuickPlayButton);
	theWidgetManager->RemoveWidget(mMoreWidget);
#endif
#ifdef _HAS_LEVELSELECTOR
	theWidgetManager->RemoveWidget(mLevelSelectorWidget);
#endif
#ifdef _HAS_UNLOCK
	theWidgetManager->RemoveWidget(mUnlockButton);
#endif
}

//0x44BD80
void GameSelector::OrderInManagerChanged()
{
	mWidgetManager->PutInfront(mOverlayWidget, this);
	mWidgetManager->PutInfront(mAlmanacButton, this);
	mWidgetManager->PutInfront(mStoreButton, this);
	mWidgetManager->PutInfront(mHelpButton, this);
	mWidgetManager->PutInfront(mQuitButton, this);
	mWidgetManager->PutInfront(mOptionsButton, this);
	mWidgetManager->PutInfront(mAdventureButton, this);
	mWidgetManager->PutInfront(mMinigameButton, this);
	mWidgetManager->PutInfront(mPuzzleButton, this);
	mWidgetManager->PutInfront(mZenGardenButton, this);
	mWidgetManager->PutInfront(mSurvivalButton, this);
	mWidgetManager->PutInfront(mChangeUserButton, this);
#ifdef _HAS_ZOMBATAR
	mWidgetManager->PutInfront(mZombatarButton, this); // @Patoke: new widgets
	mWidgetManager->PutInfront(mZombatarWidget, this);
#endif 
#ifdef _HAS_ACHIEVEMENTS
	mWidgetManager->PutInfront(mAchievementsButton, this);
	mWidgetManager->PutInfront(mTrophyButton, this);
	mWidgetManager->PutInfront(mAchievementsWidget, this);
#endif
#ifdef _HAS_MORESCREEN
	mWidgetManager->PutInfront(mQuickPlayButton, this);
	mWidgetManager->PutInfront(mMoreWidget, this);
#endif
#ifdef _HAS_LEVELSELECTOR
	mWidgetManager->PutInfront(mLevelSelectorWidget, this);
#endif
#ifdef _HAS_UNLOCK
	mWidgetManager->PutInfront(mUnlockButton, this);
#endif
}

//0x44BE60
void GameSelector::KeyDown(KeyCode theKey)
{
	if (mApp->mKonamiCheck->Check(theKey))
	{
		mApp->mTodCheatKeys = !mApp->mTodCheatKeys;
		mApp->PlayFoley(FoleyType::FOLEY_DROP);
		return;
	}
	if (mApp->mMustacheCheck->Check(theKey) || mApp->mMoustacheCheck->Check(theKey))
	{
		mApp->PlayFoley(FoleyType::FOLEY_POLEVAULT);
		mApp->mMustacheMode = !mApp->mMustacheMode;
		ReportAchievement::GiveAchievement(mApp, AchievementId::MustacheMode, false);
		return;
	}
	if (mApp->mSuperMowerCheck->Check(theKey) || mApp->mSuperMowerCheck2->Check(theKey))
	{
		mApp->PlayFoley(FoleyType::FOLEY_ZAMBONI);
		mApp->mSuperMowerMode = !mApp->mSuperMowerMode;
		return;
	}
	if (mApp->mFutureCheck->Check(theKey))
	{
		mApp->PlaySample(Sexy::SOUND_BOING);
		mApp->mFutureMode = !mApp->mFutureMode;
		return;
	}
	if (mApp->mPinataCheck->Check(theKey))
	{
		if (mApp->CanDoPinataMode())
		{
			mApp->PlayFoley(FoleyType::FOLEY_JUICY);
			mApp->mPinataMode = !mApp->mPinataMode;
			return;
		}
		else
		{
			mApp->PlaySample(Sexy::SOUND_BUZZER);
			return;
		}
	}
	if (mApp->mDanceCheck->Check(theKey))
	{
		if (mApp->CanDoDanceMode())
		{
			mApp->PlayFoley(FoleyType::FOLEY_DANCER);
			mApp->mDanceMode = !mApp->mDanceMode;
			return;
		}
		else
		{
			mApp->PlaySample(Sexy::SOUND_BUZZER);
			return;
		}
	}
	if (mApp->mDaisyCheck->Check(theKey))
	{
		if (mApp->CanDoDaisyMode())
		{
			mApp->PlaySample(Sexy::SOUND_LOADINGBAR_FLOWER);
			mApp->mDaisyMode = !mApp->mDaisyMode;
			return;
		}
		else
		{
			mApp->PlaySample(Sexy::SOUND_BUZZER);
			return;
		}
	}
	if (mApp->mSukhbirCheck->Check(theKey))
	{
		mApp->PlaySample(Sexy::SOUND_SUKHBIR);
		mApp->mSukhbirMode = !mApp->mSukhbirMode;
		return;
	}
}

//0x44C200
void GameSelector::KeyChar(char theChar)
{
	if (mStartingGame)
		return;

#ifndef _DEBUG
	return;
#endif

	if ((gIsPartnerBuild || mApp->mDebugKeysEnabled) && theChar == 'u' && mApp->mPlayerInfo)
	{
		TodTraceAndLog("Selector cheat key '%c'", theChar);

		mApp->mPlayerInfo->SetLevel(1);
		mApp->mPlayerInfo->mFinishedAdventure = 2;
		mApp->mPlayerInfo->AddCoins(50000);
		mApp->mPlayerInfo->mHasUsedCheatKeys = true;
		mApp->mPlayerInfo->mHasUnlockedMinigames = true;
		mApp->mPlayerInfo->mHasUnlockedPuzzleMode = true;
		mApp->mPlayerInfo->mHasUnlockedSurvivalMode = true;

		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FERTILIZER] = PURCHASE_COUNT_OFFSET + 5;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_BUG_SPRAY] = PURCHASE_COUNT_OFFSET + 5;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_CHOCOLATE] = PURCHASE_COUNT_OFFSET + 5;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_FOOD] = PURCHASE_COUNT_OFFSET + 5;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GATLINGPEA] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_CATTAIL] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_WINTERMELON] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_SPIKEROCK] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_COBCANNON] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_IMITATER] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 4;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_ROOF_CLEANER] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PHONOGRAPH] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_GARDENING_GLOVE] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_MUSHROOM_GARDEN] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_WHEEL_BARROW] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_AQUARIUM_GARDEN] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_OF_WISDOM] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FIRSTAID] = 1;

		/*for (int i = 1; i < 100; i++)
			if (i != (int)GameMode::GAMEMODE_TREE_OF_WISDOM && i != (int)GameMode::GAMEMODE_SCARY_POTTER_ENDLESS &&
				i != (int)GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS && (i < (int)GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_1 || i >(int)GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_5))
				mApp->mPlayerInfo->mChallengeRecords[i - 1] = 20;*/
		SyncProfile(true);

		mApp->EraseFile(GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mApp->mPlayerInfo->mId));
	}
	
	if (mApp->mDebugKeysEnabled)
	{
		TodTraceAndLog("Selector cheat key '%c'", theChar);
		if (theChar == 'c' || theChar == 'C')
		{
			mMinigamesLocked = false;
			mPuzzleLocked = false;
			mSurvivalLocked = false;
			SyncButtons();
		}
	}
}

//0x44C360
void GameSelector::MouseDown(int x, int y, int theClickCount)
{
	if (mSlideCounter > 0)
		return;

	for (int i = 0; i < 3; i++)
	{
		Reanimation* aFlowerReanim = mApp->ReanimationGet(mFlowerReanimID[i]);
		if (!mApp->mFlowersPlucked[i] && aFlowerReanim->mAnimRate <= 0.0f && Distance2D(x, y, gFlowerCenter[i][0], gFlowerCenter[i][1]) < 20.0f)
		{
			mApp->mFlowersPlucked[i] = true;
			aFlowerReanim->mAnimRate = 24.0f;
			mApp->PlayFoley(FoleyType::FOLEY_LIMBS_POP);
		}
	}

	if (mApp->mTodCheatKeys && mStartingGame && mStartingGameCounter < 450)
		mStartingGameCounter = 450;
}

//0x44C4C0
void GameSelector::ButtonMouseEnter(int theId)
{
	if (mSlideCounter > 0)
		return;

	if ((theId == GameSelector::GameSelector_Minigame && mMinigamesLocked) ||
		(theId == GameSelector::GameSelector_Puzzle && mPuzzleLocked) ||
		(theId == GameSelector::GameSelector_Survival && mSurvivalLocked) ||
		mSlideCounter > 0 || mSelectorState != SelectorAnimState::SELECTOR_IDLE || theId == -1)
		return;

	mApp->PlayFoley(FoleyType::FOLEY_BLEEP);
}

//0x44C540
void GameSelector::ButtonPress(int theId, int theClickCount)
{
	if (mSlideCounter > 0)
		return;

	if (theId == -1)
		return;

	if (theId == GameSelector::GameSelector_Adventure || theId == GameSelector::GameSelector_Minigame ||
		theId == GameSelector::GameSelector_Puzzle || theId == GameSelector::GameSelector_Survival  
#ifdef _HAS_ZOMBATAR
		|| theId == GameSelector::GameSelector_Zombatar
#endif
#ifdef _HAS_UNLOCK
		|| theId == GameSelector::GameSelector_Unlock
#endif
		)
		mApp->PlaySample(Sexy::SOUND_GRAVEBUTTON);
	else
		mApp->PlaySample(Sexy::SOUND_TAP);
}

//0x44C590
void GameSelector::ClickedAdventure()
{
	if (mApp->IsTrialStageLocked() && (mLevel >= 25 || mApp->HasFinishedAdventure()))
	{
		if (mApp->LawnMessageBox(
			Dialogs::DIALOG_MESSAGE,
			_S("[REPLAY_LEVEL_HEADER]"),
			_S("[REPLAY_LEVEL_BODY]"),
			_S("[DIALOG_BUTTON_YES]"),
			_S("[DIALOG_BUTTON_NO]"),
			Dialog::BUTTONS_YES_NO) == Dialog::ID_NO)
			return;

		mApp->mPlayerInfo->mLevel = 24;
		mApp->mPlayerInfo->mFinishedAdventure = 0;
		mApp->EraseFile(GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mApp->mPlayerInfo->mId));
	}

	/*if (mApp->mPlayerInfo->mLevel > 1)
	{
		ShowQuickplayScreen();
		return;
	}*/

	mApp->mMusic->StopAllMusic();
	mApp->PlaySample(Sexy::SOUND_LOSEMUSIC);
	mStartingGame = true;
	
	DisableButtons(true);

	Reanimation* aHandReanim = mApp->AddReanimation(-70.0f, 10.0f, 0, ReanimationType::REANIM_ZOMBIE_HAND);
	aHandReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
	mHandReanimID = mApp->ReanimationGetID(aHandReanim);
	mApp->PlayFoley(FoleyType::FOLEY_DIRT_RISE);
	for (int i = 0; i < aHandReanim->mDefinition->mTracks.count; i++)
		if (!strnicmp(aHandReanim->mDefinition->mTracks.tracks[i].mName, "rock", 4))
			aHandReanim->mTrackInstances[i].mIgnoreClipRect = true;

#ifdef _HAS_GAMESELECTOR_SPOTLIGHT
	Reanimation* aSplotLightReanim = mApp->AddReanimation(-70.0f, -10.0f, 0, ReanimationType::REANIM_SELECTORSCREEN_SPOTLIGHT);
	aSplotLightReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
	mSpotLightID = mApp->ReanimationGetID(aSplotLightReanim);
#endif
}

//0x44C890
bool GameSelector::ShouldDoZenTuturialBeforeAdventure()
{
	return !mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() == 45 && mApp->mPlayerInfo->mNumPottedPlants == 0;
}

//0x44C8C0
void GameSelector::ButtonDepress(int theId)
{
	if (mSlideCounter > 0 || mStartingGame)
		return;

	if (theId == GameSelector::GameSelector_Minigame && mMinigamesLocked)
	{
		mApp->LawnMessageBox(Dialogs::DIALOG_MESSAGE, _S("[MODE_LOCKED]"), _S("[MINIGAME_LOCKED_MESSAGE]"), _S("[DIALOG_BUTTON_OK]"), _S(""), Dialog::BUTTONS_FOOTER);
		return;
	}
	if (theId == GameSelector::GameSelector_Puzzle && mPuzzleLocked)
	{
		mApp->LawnMessageBox(Dialogs::DIALOG_MESSAGE, _S("[MODE_LOCKED]"), _S("[PUZZLE_LOCKED_MESSAGE]"), _S("[DIALOG_BUTTON_OK]"), _S(""), Dialog::BUTTONS_FOOTER);
		return;
	}
	if (theId == GameSelector::GameSelector_Survival && mSurvivalLocked)
	{
		mApp->LawnMessageBox(Dialogs::DIALOG_MESSAGE, _S("[MODE_LOCKED]"), _S("[SURVIVAL_LOCKED_MESSAGE]"), _S("[DIALOG_BUTTON_OK]"), _S(""), Dialog::BUTTONS_FOOTER);
		return;
	}

	switch (theId)
	{
	case GameSelector::GameSelector_Adventure:
		ClickedAdventure();
		break;
	case GameSelector::GameSelector_Minigame:
		mApp->KillGameSelector();
		mApp->ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
		break;
	case GameSelector::GameSelector_Puzzle:
		mApp->KillGameSelector();
		mApp->ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_PUZZLE);
		break;
	case GameSelector::GameSelector_Survival:
		mApp->KillGameSelector();
		mApp->ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_SURVIVAL);
		break;
	case GameSelector::GameSelector_Quit:
		mApp->ConfirmQuit();
		break;
	case GameSelector::GameSelector_Help:
		mApp->KillGameSelector();
		mApp->ShowAwardScreen(AwardType::AWARD_HELP_ZOMBIENOTE, false);
		break;
	case GameSelector::GameSelector_Options:
		mApp->DoNewOptions(true);
		break;
	case GameSelector::GameSelector_ChangeUser:
		mApp->DoUserDialog();
		break;
	case GameSelector::GameSelector_Store:
	{
		StoreScreen* aStore = mApp->ShowStoreScreen();
		aStore->WaitForResult(true);
		if (aStore->mGoToTreeNow)
		{
			mApp->KillGameSelector();
			mApp->PreNewGame(GameMode::GAMEMODE_TREE_OF_WISDOM, false);
		}
		else
			mApp->mMusic->MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);

		break;
	}
	case GameSelector::GameSelector_Almanac:
		mApp->DoAlmanacDialog()->WaitForResult(true);
		mApp->mMusic->MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);
		break;
	case GameSelector::GameSelector_ZenGarden:
		mApp->KillGameSelector();
		mApp->PreNewGame(GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN, false);
		if (ShouldDoZenTuturialBeforeAdventure())
			mApp->mZenGarden->SetupForZenTutorial();
		break;
#ifdef _HAS_ZOMBATAR
	case GameSelector::GameSelector_Zombatar:
	{
		if (mApp->mPlayerInfo->mAckZombatarTOS)
		{
			ShowZombatarScreen();
		}
		else
		{
			mApp->ShowZombatarTOS();
		}
		break;
	}
#endif
#ifdef _HAS_ACHIEVEMENTS
	case GameSelector::GameSelector_AchievementsBack: // @Patoke: seems to be unused
		//SlideTo(0, 0);
		break;
	case GameSelector::GameSelector_Achievements:
		ShowAchievementsScreen();
		break;
#endif
#ifdef _HAS_MORESCREEN
	case GameSelector::GameSelector_QuickPlay:
		ShowMoreScreen();
		// GameSelector::ShowQuickPlayScreen();
		break;
#endif
#ifdef _HAS_UNLOCK
	case GameSelector::GameSelector_Unlock:
		if (mApp->mPlayerInfo)
		{
			if (mApp->mPlayerInfo->mHasUsedCheatKeys)
				mApp->DoDialog(Dialogs::DIALOG_ALREADY_UNLOCK, true, _S("[ALREADY_UNLOCKED_DIALOG_HEADER]"), _S("[ALREADY_UNLOCKED_DIALOG_BODY]"), _S("OK"), Dialog::BUTTONS_FOOTER);
			else
				mApp->DoDialog(Dialogs::DIALOG_UNLOCK, true, _S("[UNLOCK_DIALOG_HEADER]"), _S("[UNLOCK_DIALOG_BODY]"), _S(""), Dialog::BUTTONS_YES_NO);
		}
		break;
#endif
	}
}

//0x44CB00
void GameSelector::AddPreviewProfiles()
{
	PlayerInfo* aProfile;

	aProfile = mApp->mProfileMgr->AddProfile(_S("2 Night"));
	if (aProfile)
	{
		aProfile->mLevel = 11;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("3 Pool"));
	if (aProfile)
	{
		aProfile->mLevel = 21;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mCoins = 400;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 1;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("4 Fog"));
	if (aProfile)
	{
		aProfile->mLevel = 31;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mHasUnlockedSurvivalMode = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 2;
		aProfile->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		aProfile->mCoins = 400;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("5 Roof"));
	if (aProfile)
	{
		aProfile->mLevel = 41;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mHasUnlockedPuzzleMode = 1;
		aProfile->mHasUnlockedSurvivalMode = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 2;
		aProfile->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		aProfile->mCoins = 500;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("6 Highground"));
	if (aProfile)
	{
		aProfile->mLevel = 51;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mHasUnlockedPuzzleMode = 1;
		aProfile->mHasUnlockedSurvivalMode = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 2;
		aProfile->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		aProfile->mCoins = 600;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("Complete"));
	if (aProfile)
	{
		aProfile->mLevel = 1;
		aProfile->mFinishedAdventure = 1;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mHasUnlockedPuzzleMode = 1;
		aProfile->mHasUnlockedSurvivalMode = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 2;
		aProfile->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_ROOF_CLEANER] = 1;
		aProfile->mCoins = 1000;
		aProfile->SaveDetails();
	}

	aProfile = mApp->mProfileMgr->AddProfile(_S("Full Unlock"));
	if (aProfile)
	{
		aProfile->mLevel = 99;
		aProfile->mFinishedAdventure = 2;
		aProfile->AddCoins(50000);
		aProfile->mPurchases[StoreItem::STORE_ITEM_FERTILIZER] = PURCHASE_COUNT_OFFSET + 5;
		aProfile->mPurchases[StoreItem::STORE_ITEM_BUG_SPRAY] = PURCHASE_COUNT_OFFSET + 5;
		aProfile->mPurchases[StoreItem::STORE_ITEM_CHOCOLATE] = PURCHASE_COUNT_OFFSET + 5;
		aProfile->mPurchases[StoreItem::STORE_ITEM_TREE_FOOD] = PURCHASE_COUNT_OFFSET + 5;
		aProfile->mHasUnlockedMinigames = 1;
		aProfile->mHasUnlockedPuzzleMode = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_GATLINGPEA] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_CATTAIL] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_WINTERMELON] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_SPIKEROCK] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_COBCANNON] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PLANT_IMITATER] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 4;
		aProfile->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_ROOF_CLEANER] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_PHONOGRAPH] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_GARDENING_GLOVE] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_MUSHROOM_GARDEN] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_WHEEL_BARROW] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_AQUARIUM_GARDEN] = 1;
		aProfile->mPurchases[StoreItem::STORE_ITEM_TREE_OF_WISDOM] = 1;
		mApp->mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FIRSTAID] = 1;

		aProfile->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - 1] = 1;
		for (int i = 1; i < 100; i++)
			if (i != (int)GameMode::GAMEMODE_TREE_OF_WISDOM && i != (int)GameMode::GAMEMODE_SCARY_POTTER_ENDLESS &&
				i != (int)GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS && (i < (int)GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_1 || i >(int)GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_5))
				aProfile->mChallengeRecords[i - 1] = 20;

		aProfile->SaveDetails();
	}
}
// @Patoke: implemented functions
// GOTY @Patoke: 0x450140
void GameSelector::SlideTo(int theX, int theY) {
	mSlideCounter = 75;
	mDestX = theX;
	mDestY = theY;
	mStartX = mX;
	mStartY = mY;
}
#ifdef _HAS_ACHIEVEMENTS
// GOTY @Patoke: 0x450200
void GameSelector::ShowAchievementsScreen() {
	SlideTo(0, -mApp->mHeight);

	mWidgetManager->SetFocus(mAchievementsWidget);

#ifdef _HAS_MORESCREEN
	mMoreWidget->SetDisabled(true);
#endif
	DisableButtons(true);
}
#endif
#ifdef _HAS_ZOMBATAR
void GameSelector::ShowZombatarScreen() {
	SlideTo(-mApp->mWidth, 0);
	mWidgetManager->SetFocus(mZombatarWidget);
#if defined(_HAS_LEVELSELECTOR) && defined(_HAS_ZOMBATAR)
	mWidgetManager->PutBehind(mLevelSelectorWidget, mZombatarWidget);
#endif
	mZombatarWidget->mBackButton->SetDisabled(false);
	mZombatarWidget->mBackButton->mButtonImage = Sexy::IMAGE_BLANK;
	mZombatarWidget->mBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mZombatarWidget->mBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mZombatarWidget->ResetZombatar();
#ifdef _HAS_MORESCREEN
	mMoreWidget->DisableButtons(true);
#endif
	DisableButtons(true);
}
#endif
#ifdef _HAS_MORESCREEN
void GameSelector::ShowMoreScreen() {
	SlideTo(mApp->mWidth, 0);

	mWidgetManager->SetFocus(mMoreWidget);
	mMoreWidget->DisableButtons(false);
	DisableButtons(true);
}
#endif
#ifdef _HAS_LEVELSELECTOR
void GameSelector::ShowQuickplayScreen() {
	SlideTo(-mApp->mWidth, 0);

	mWidgetManager->SetFocus(mLevelSelectorWidget);
	mWidgetManager->PutBehind(mZombatarWidget, mLevelSelectorWidget);
	//int currentStage = min((int)(mApp->mPlayerInfo->mLevel / 10.0f), 4) + 1;
	//currentStage = 1;
	//mLevelSelectorWidget->theCurrentId = currentStage;
	mLevelSelectorWidget->SelectStage(1, false);
	mLevelSelectorWidget->mHasFinishedSliding = false;
	mLevelSelectorWidget->mIsSlidingOut = false;
	mMoreWidget->DisableButtons(true);
	DisableButtons(true);
}
#endif

void GameSelector::ShowGameSelectorScreen() {
	SlideTo(0, 0);
	mWidgetManager->SetFocus(this);
#ifdef _HAS_MORESCREEN
	mMoreWidget->DisableButtons(true);
#endif
	DisableButtons(false);
}

void GameSelector::DisableButtons(bool isDisabled){
	mAdventureButton->SetDisabled(isDisabled);
	mMinigameButton->SetDisabled(isDisabled);
	mPuzzleButton->SetDisabled(isDisabled);
	mOptionsButton->SetDisabled(isDisabled);
	mQuitButton->SetDisabled(isDisabled);
	mHelpButton->SetDisabled(isDisabled);
	mChangeUserButton->SetDisabled(isDisabled);
	mStoreButton->SetDisabled(isDisabled);
	mAlmanacButton->SetDisabled(isDisabled);
	mSurvivalButton->SetDisabled(isDisabled);
	mZenGardenButton->SetDisabled(isDisabled);
#ifdef _HAS_ZOMBATAR
	mZombatarButton->SetDisabled(isDisabled);
#endif
#ifdef _HAS_ACHIEVEMENTS
	mAchievementsButton->SetDisabled(isDisabled);
	mTrophyButton->SetDisabled(isDisabled);
#endif
#ifdef _HAS_MORESCREEN
	mQuickPlayButton->SetDisabled(isDisabled);
#endif
#ifdef _HAS_UNLOCK
	mUnlockButton->SetDisabled(isDisabled);
#endif
}
