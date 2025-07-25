#include "../Plant.h"
#include "../Board.h"
#include "GameButton.h"
#include "StoreScreen.h"
#include "AwardScreen.h"
#include "../ZenGarden.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "AchievementsScreen.h"
#include "../../SexyAppFramework/WidgetManager.h"

//0x405780
// GOTY @Patoke: 0x4063E0
AwardScreen::AwardScreen(LawnApp* theApp, AwardType theAwardType, bool theShowingAchievements)
{
	mApp = theApp;
	mClip = false;
	mFadeInCounter = 180;
	mAwardType = theAwardType;
	TodLoadResources("DelayLoad_AwardScreen");
	// @Patoke: implemented and Inliothixi
	mShowingAchievements = false;
	
	if (theShowingAchievements) {
		mAchievementAnimTime = 0;

		for (int i = 0; i < (int)MAX_ACHIEVEMENTS; i++) {
			if (mApp->mPlayerInfo->mEarnedAchievements[i] && !mApp->mPlayerInfo->mShownAchievements[i]) {
				mShowingAchievements = mApp->mPlayerInfo->mShownAchievements[i] = true;

				AchievementScreenItem aAchievementItem{};
				aAchievementItem.mId = i;
				aAchievementItem.mStartAnimTime = 100 * mAchievementItems.size() + 150;
				aAchievementItem.mEndAnimTime = aAchievementItem.mStartAnimTime + 100;
				aAchievementItem.mStartY = 750;
				aAchievementItem.mY = aAchievementItem.mStartY;
				mAchievementItems.push_back(aAchievementItem);
			}
		}

		if (mShowingAchievements)
		{
			TodLoadResources("DelayLoad_ChallengeScreen");
		}

		if (!mAchievementItems.empty()) {
			//int aDestY = 284 - ((76 * mAchievementItems.size()) >> 1);
			int aDestY = 284 - ((76 * mAchievementItems.size()) / 2);

			for (size_t j = 0; j < mAchievementItems.size(); j++) {
				mAchievementItems[j].mDestY = aDestY;
				aDestY += 76;
			}
		}

		mApp->WriteCurrentUserConfig();
	}

	int aLevel = mApp->mPlayerInfo->GetLevel();
	if (mAwardType == AWARD_CREDITS_ZOMBIENOTE)
	{
		TodLoadResources("DelayLoad_Background6");
		TodLoadResources("DelayLoad_ZombieNote");
		TodLoadResources("DelayLoad_Credits");
	}
	else if (mAwardType == AWARD_HELP_ZOMBIENOTE)
	{
		TodLoadResources("DelayLoad_Background1");
		TodLoadResources("DelayLoad_ZombieNote");
		TodLoadResources("DelayLoad_ZombieNoteHelp");
	}
	else if (mApp->IsAdventureMode())
	{
		if (aLevel == 10)
		{
			TodLoadResources("DelayLoad_Background1");
			TodLoadResources("DelayLoad_ZombieNote");
			TodLoadResources("DelayLoad_ZombieNote1");
		}
		else if (aLevel == 20)
		{
			TodLoadResources("DelayLoad_Background2");
			TodLoadResources("DelayLoad_ZombieNote");
			TodLoadResources("DelayLoad_ZombieNote2");
		}
		else if (aLevel == 30)
		{
			TodLoadResources("DelayLoad_Background1");
			TodLoadResources("DelayLoad_ZombieNote");
			TodLoadResources("DelayLoad_ZombieNote3");
		}
		else if (aLevel == 40)
		{
			TodLoadResources("DelayLoad_Background2");
			TodLoadResources("DelayLoad_ZombieNote");
			TodLoadResources("DelayLoad_ZombieNote4");
		}
		else if (aLevel == 50)
		{
			TodLoadResources("DelayLoad_Background1");
			TodLoadResources("DelayLoad_ZombieNote");
			TodLoadResources("DelayLoad_ZombieFinalNote");
		}
	}

	mStartButton = new GameButton(AwardScreen::AwardScreen_Start);
	mStartButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON;
	mStartButton->mOverImage = nullptr;
	mStartButton->mDownImage = nullptr;
	mStartButton->mDisabledImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_DISABLED;
	mStartButton->mOverOverlayImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_GLOW;
	mStartButton->SetFont(Sexy::FONT_DWARVENTODCRAFT15);
	mStartButton->mColors[ButtonWidget::COLOR_LABEL] = Color(213, 159, 43);
	mStartButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(213, 159, 43);
	mStartButton->Resize(324, 500, 156, 42);
	mStartButton->mTextOffsetY = -1;

	// @Patoke: implemented
	mContinueButton = new GameButton(AwardScreen::AwardScreen_Start);
	mContinueButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON;
	mContinueButton->mOverImage = nullptr;
	mContinueButton->mDownImage = nullptr;
	mContinueButton->mDisabledImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_DISABLED;
	mContinueButton->mOverOverlayImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_GLOW;
	mContinueButton->SetFont(Sexy::FONT_DWARVENTODCRAFT15);
	mContinueButton->mColors[ButtonWidget::COLOR_LABEL] = Color(213, 159, 43);
	mContinueButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(213, 159, 43);
	mContinueButton->Resize(324, 515, 156, 42);
	mContinueButton->mParentWidget = this;
	mContinueButton->mTextOffsetY = -1;
	mContinueButton->mBtnNoDraw = true;
	mContinueButton->mDisabled = true;

	mMenuButton = new GameButton(AwardScreen::AwardScreen_Menu);
	mMenuButton->SetLabel(_S("[AWARD_MAIN_MENU_BUTTON]"));
	mMenuButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2;
	mMenuButton->mOverImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW;
	mMenuButton->mDownImage = nullptr;
	mMenuButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mMenuButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
	mMenuButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
	mMenuButton->mParentWidget = this;
	mMenuButton->Resize(677, 16, 111, 26);
	mMenuButton->mTextOffsetY = 1;
	if (!mApp->HasFinishedAdventure() && aLevel <= 3)
	{
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
	}

	if (mAwardType == AWARD_CREDITS_ZOMBIENOTE)
	{
		mStartButton->SetLabel(_S("[ROLL_CREDITS]"));
		mStartButton->mButtonImage = Sexy::IMAGE_CREDITS_PLAYBUTTON;
		mStartButton->mOverImage = nullptr;
		mStartButton->mDownImage = nullptr;
		mStartButton->mDisabledImage = nullptr;
		mStartButton->mOverOverlayImage = nullptr;
		mStartButton->SetFont(Sexy::FONT_HOUSEOFTERROR20);
		mStartButton->mColors[ButtonWidget::COLOR_LABEL] = Color(255, 255, 255);
		mStartButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(213, 159, 43);
		mStartButton->Resize(325, 505, 190, 73);
		mStartButton->mTextOffsetX = 33;
		mStartButton->mTextOffsetY = -2;
		mStartButton->mButtonOffsetX = -2;
		mStartButton->mButtonOffsetY = 8;
		mStartButton->mParentWidget = this;
	}
	else if (mAwardType == AWARD_HELP_ZOMBIENOTE)
	{
		mStartButton->SetLabel(_S("[MAIN_MENU_BUTTON]"));
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
	}
	else if (!mApp->IsAdventureMode())
	{
		mStartButton->SetLabel(_S("[MAIN_MENU_BUTTON]"));
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
	}
	else if (mApp->HasFinishedAdventure()) // @Patoke: change case
	{
		ReportAchievement::GiveAchievement(mApp, AchievementId::HomeSecurity, false); // @Patoke: add achievement
		mStartButton->SetLabel(_S("[CONTINUE_BUTTON]"));
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
	}
	else if (aLevel == 15)
		mStartButton->SetLabel(_S("[VIEW_ALMANAC_BUTTON]"));
	else if (aLevel == 25 || aLevel == 35 || aLevel == 45)
		mStartButton->SetLabel(_S("[CONTINUE_BUTTON]"));
	else
		mStartButton->SetLabel(_S("[NEXT_LEVEL_BUTTON]"));

	if (mApp->EarnedGoldTrophy()) 
		ReportAchievement::GiveAchievement(mApp, AchievementId::NovelPeasPrize, false);

	if (mApp->IsFirstTimeAdventureMode() && aLevel == 25 && mApp->IsTrialStageLocked() && !mApp->mPlayerInfo->mHasSeenUpsell)
	{
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
	}

	// @Patoke: implemented
	if (mShowingAchievements) {
		mShowStartButtonAfterAchievements = !mStartButton->mBtnNoDraw;
		mShowMenuButtonAfterAchievements = !mMenuButton->mBtnNoDraw;
		mStartButton->mBtnNoDraw = true;
		mStartButton->mDisabled = true;
		mMenuButton->mBtnNoDraw = true;
		mMenuButton->mDisabled = true;
		mContinueButton->SetLabel(_S("[CONTINUE_BUTTON]"));
	}

	if (IsPaperNote())
	{
		mApp->mMusic->StopAllMusic();
		mStartButton->mY += 20;
		mApp->PlayFoley(FOLEY_PAPER);
	}
	else
		mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_ZEN_GARDEN);
}

//0x406420 & 0x406440
AwardScreen::~AwardScreen()
{
	if (mStartButton) delete mStartButton;
	if (mContinueButton) delete mContinueButton; // @Patoke: add new button
	if (mMenuButton) delete mMenuButton;
}

bool AwardScreen::IsPaperNote()
{
	if (mAwardType == AWARD_CREDITS_ZOMBIENOTE || mAwardType == AWARD_HELP_ZOMBIENOTE)
		return true;

	int aLevel = mApp->mPlayerInfo->GetLevel();
	return mApp->IsAdventureMode() && (aLevel == 10 || aLevel == 20 || aLevel == 30 || aLevel == 40 || aLevel == 50);
}

//0x4064D0
void AwardScreen::DrawBottom(Graphics* g, const SexyString& theTitle, const SexyString& theAward, const SexyString& theMessage)
{
	g->DrawImage(Sexy::IMAGE_AWARDSCREEN_BACK, 0, 0);
	TodDrawString(g, theTitle, BOARD_WIDTH / 2, 58, Sexy::FONT_DWARVENTODCRAFT24, Color(213, 159, 43), DS_ALIGN_CENTER);
	TodDrawString(g, theAward, BOARD_WIDTH / 2, 326, Sexy::FONT_DWARVENTODCRAFT18YELLOW, Color::White, DS_ALIGN_CENTER);
	TodDrawStringWrapped(g, theMessage, Rect(285, 360, 230, 90), Sexy::FONT_BRIANNETOD16, Color(40, 50, 90), DS_ALIGN_CENTER_VERTICAL_MIDDLE);
}

//0x4066A0
void AwardScreen::DrawAwardSeed(Graphics* g)
{
	SeedType aSeedType = mApp->GetAwardSeedForLevel(mApp->mPlayerInfo->GetLevel() - 1);
	SexyString aAward = Plant::GetNameString(aSeedType, SEED_NONE);
	SexyString aMessage;
	if (mApp->IsTrialStageLocked() && aSeedType >= SEED_SQUASH && aSeedType != SEED_TANGLEKELP)
		aMessage = _S("[AVAILABLE_IN_FULL_VERSION]");
	else
		aMessage = Plant::GetToolTip(aSeedType);
	DrawBottom(g, _S("[NEW_PLANT]"), aAward, aMessage);

	g->SetScale(2, 2, 350, 129);
	DrawSeedPacket(g, 350, 129, aSeedType, SEED_NONE, 0, 255, true, false);
	g->SetScale(1, 1, 0, 0);
}

//0x4068D0
// GOTY @Patoke: 0x4081C0
void AwardScreen::Draw(Graphics* g)
{
	g->SetLinearBlend(true);

	int aLevel = mApp->mPlayerInfo->GetLevel();
	if (mShowingAchievements) // @Patoke: add call
		DrawAchievements(g);
	else if (mAwardType == AWARD_CREDITS_ZOMBIENOTE)
	{
		g->SetColor(Color(125, 200, 255, 255));
		g->SetColorizeImages(true);
		g->DrawImage(Sexy::IMAGE_BACKGROUND6BOSS, -900, -400, 2800, 1200);
		g->SetColorizeImages(false);
		g->SetColor(Color(0, 0, 0, 64));
		g->FillRect(-gSexyAppBase->mDDInterface->mWideScreenOffsetX, 525, gSexyAppBase->mDDInterface->GetScreenImage()->GetWidth(), gSexyAppBase->mDDInterface->GetScreenImage()->GetHeight());
		g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 75, 60);
		g->DrawImage(Sexy::IMAGE_CREDITS_ZOMBIENOTE, 149, 103, 475, 325);
	}
	else if (mAwardType == AWARD_HELP_ZOMBIENOTE)
	{
		g->DrawImage(Sexy::IMAGE_BACKGROUND1, -700, -300, 2800, 1200);
		g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
		g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE_HELP, 131, 132);
	}
	else if (mAwardType != AWARD_ACHIEVEMENTONLY) // @Patoke: add check
	{
		if (!mApp->IsAdventureMode())
		{
			if (mApp->EarnedGoldTrophy())
			{
				DrawBottom(g, _S("[BEAT_GAME_MESSAGE1]"), _S("[GOLD_SUNFLOWER_TROPHY]"), _S("[BEAT_GAME_MESSAGE2]"));
				TodDrawImageCelCenterScaledF(g, Sexy::IMAGE_SUNFLOWER_TROPHY, 330, 80, 1, 0.7f, 0.7f);
			}
			else
			{
				SexyString aMsgChar;
				if (mApp->IsSurvivalMode())
				{
					int aNumTrophies = mApp->GetNumTrophies(CHALLENGE_PAGE_SURVIVAL);
					aMsgChar =
						aNumTrophies <= 7 ? _S("[YOU_UNLOCKED_A_SURVIVAL]") :
						aNumTrophies == 10 ? _S("[YOU_UNLOCKED_ENDLESS_SURVIVAL]") : _S("[EARN_MORE_TROPHIES_FOR_ENDLESS_SURVIVAL]");

				}
				else if (mApp->IsScaryPotterLevel())
					aMsgChar = _S("[UNLOCKED_VASEBREAKER_LEVEL]");
				else if (mApp->IsPuzzleMode())
					aMsgChar = _S("[UNLOCKED_I_ZOMBIE_LEVEL]");
				else
					aMsgChar = mApp->GetNumTrophies(CHALLENGE_PAGE_CHALLENGE) <= 17 ? _S("[CHALLENGE_UNLOCKED]") : _S("[GET_MORE_TROPHIES]");

				DrawBottom(g, _S("[GOT_TROPHY]"), _S("[TROPHY]"), aMsgChar);
				g->DrawImage(Sexy::IMAGE_TROPHY_HI_RES, BOARD_WIDTH / 2 - Sexy::IMAGE_TROPHY_HI_RES->mWidth / 2, 137);
			}
		}
		else if (aLevel == 5)
		{
			DrawBottom(g, _S("[GOT_SHOVEL]"), _S("[SHOVEL]"), _S("[SHOVEL_DESCRIPTION]"));
			g->DrawImage(Sexy::IMAGE_SHOVEL_HI_RES, BOARD_WIDTH / 2 - Sexy::IMAGE_SHOVEL_HI_RES->mWidth / 2, 137);
		}
		else if (aLevel == 10)
		{
			g->DrawImage(Sexy::IMAGE_BACKGROUND1, -700, -300, 2800, 1200);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE1, 131, 132);
			TodDrawString(g, _S("[FOUND_NOTE]"), BOARD_WIDTH / 2, 70, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 200, 0, 255), DS_ALIGN_CENTER);
		}
		else if (aLevel == 15)
		{
			DrawBottom(g, _S("[FOUND_SUBURBAN_ALMANAC]"), _S("[SUBURBAN_ALMANAC]"), _S("[SUBURBAN_ALMANAC_DESCRIPTION]"));
			g->DrawImage(Sexy::IMAGE_ALMANAC, BOARD_WIDTH / 2 - Sexy::IMAGE_ALMANAC->mWidth / 2, 160);
		}
		else if (aLevel == 20)
		{
			g->DrawImage(Sexy::IMAGE_BACKGROUND2, -700, -300, 2800, 1200);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE2, 133, 127);
			TodDrawString(g, _S("[FOUND_NOTE]"), BOARD_WIDTH / 2, 70, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 200, 0, 255), DS_ALIGN_CENTER);
		}
		else if (aLevel == 25)
		{
			DrawBottom(g, _S("[FOUND_KEYS]"), _S("[KEYS]"), _S("[KEYS_DESCRIPTION]"));
			g->DrawImage(Sexy::IMAGE_CARKEYS, BOARD_WIDTH / 2 - Sexy::IMAGE_CARKEYS->mWidth / 2, 160);
		}
		else if (aLevel == 30)
		{
			g->DrawImage(Sexy::IMAGE_BACKGROUND1, -700, -300, 2800, 1200);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE3, 120, 117);
			TodDrawString(g, _S("[FOUND_NOTE]"), BOARD_WIDTH / 2, 70, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 200, 0, 255), DS_ALIGN_CENTER);
		}
		else if (aLevel == 35)
		{
			DrawBottom(g, _S("[FOUND_TACO]"), _S("[TACO]"), _S("[TACO_DESCRIPTION]"));
			g->DrawImage(Sexy::IMAGE_TACO, BOARD_WIDTH / 2 - Sexy::IMAGE_TACO->mWidth / 2, 160);
		}
		else if (aLevel == 40)
		{
			g->DrawImage(Sexy::IMAGE_BACKGROUND2, -700, -300, 2800, 1200);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE4, 102, 117);
			TodDrawString(g, _S("[FOUND_NOTE]"), BOARD_WIDTH / 2, 70, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 200, 0, 255), DS_ALIGN_CENTER);
		}
		else if (aLevel == 45)
		{
			DrawBottom(g, _S("[FOUND_WATERING_CAN]"), _S("[WATERING_CAN]"), _S("[WATERING_CAN_DESCRIPTION]"));
			g->DrawImage(Sexy::IMAGE_WATERINGCAN, BOARD_WIDTH / 2 - Sexy::IMAGE_WATERINGCAN->mWidth / 2, 160);
		}
		else if (aLevel == 50)
		{
			g->DrawImage(Sexy::IMAGE_BACKGROUND1, -700, -300, 2800, 1200);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_NOTE, 80, 80);
			g->DrawImage(Sexy::IMAGE_ZOMBIE_FINAL_NOTE, 114, 138);
			TodDrawString(g, _S("[FOUND_NOTE]"), BOARD_WIDTH / 2, 70, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 200, 0, 255), DS_ALIGN_CENTER);
		}
		else if (aLevel == 51 && mApp->mPlayerInfo->mFinishedAdventure == 1) // aLevel == 1 // mApp->HasFinishedAdventure()
		{
			DrawBottom(g, _S("[WIN_MESSAGE1]"), _S("[SILVER_SUNFLOWER_TROPHY]"), _S("[WIN_MESSAGE2]"));
			TodDrawImageCelCenterScaledF(g, Sexy::IMAGE_SUNFLOWER_TROPHY, 325, 65, 0, 0.7f, 0.7f);
		}
		else
		{
			DrawAwardSeed(g);
		}
	}

	mStartButton->Draw(g);
	mMenuButton->Draw(g);
	mContinueButton->Draw(g); // @Patoke: add call

	int aFadeInAlpha = TodAnimateCurve(180, 0, mFadeInCounter, 255, 0, CURVE_LINEAR);
	g->SetColor(IsPaperNote() ? Color(0, 0, 0, aFadeInAlpha) : Color(255, 255, 255, aFadeInAlpha));
	g->FillRect(-gSexyAppBase->mDDInterface->mWideScreenOffsetX, 0, gSexyAppBase->mDDInterface->GetScreenImage()->GetWidth(), gSexyAppBase->mDDInterface->GetScreenImage()->GetHeight());
}

//0x4076A0
// GOTY @Patoke: 0x408FE0
void AwardScreen::Update()
{
	Widget::Update();
	// @Patoke: implemented
	if (mShowingAchievements) 
	{
		mAchievementAnimTime++;

		for (size_t i = 0; i < mAchievementItems.size(); i++) {
			if (mAchievementAnimTime >= mAchievementItems[i].mStartAnimTime) {
				mAchievementItems[i].mY = TodAnimateCurve(mAchievementItems[i].mStartAnimTime, mAchievementItems[i].mEndAnimTime, mAchievementAnimTime, mAchievementItems[i].mStartY, mAchievementItems[i].mDestY, CURVE_EASE_IN_OUT);
			}

			if (mAchievementItems[mAchievementItems.size() - 1].mY == mAchievementItems[mAchievementItems.size() - 1].mDestY) {
				mContinueButton->mBtnNoDraw = false;
				mContinueButton->mDisabled = false;
			}
		}
	}

	if (mApp->GetDialogCount() > 0) return;
	mStartButton->Update();
	mMenuButton->Update();
	mContinueButton->Update(); // @Patoke: add call
	if (mApp->mWidgetManager->mOverWidget)
		mApp->SetCursor(mStartButton->IsMouseOver() || mMenuButton->IsMouseOver() ? CURSOR_HAND : CURSOR_POINTER);
	MarkDirty();
	if (mFadeInCounter > 0) mFadeInCounter--;
}

//0x407760
void AwardScreen::KeyChar(char theChar)
{
	if (theChar == ' ' || theChar == '\r' || theChar == '\u001B')
		StartButtonPressed();
}

//0x407780
// GOTY @Patoke: 0x409530
void AwardScreen::StartButtonPressed()
{
	if (mApp->GetDialog(DIALOG_STORE))
		return;

	if (mAwardType == AWARD_CREDITS_ZOMBIENOTE)
	{
		mApp->KillAwardScreen();
		mApp->ShowCreditScreen();
	}
	else if (mAwardType == AWARD_HELP_ZOMBIENOTE)
	{
		mApp->KillAwardScreen();
		mApp->ShowGameSelector();
	}
	else if (mApp->IsSurvivalMode())
	{
		mApp->KillAwardScreen();
		mApp->ShowChallengeScreen(CHALLENGE_PAGE_SURVIVAL);
	}
	else if (mApp->IsPuzzleMode())
	{
		mApp->KillAwardScreen();
		mApp->ShowChallengeScreen(CHALLENGE_PAGE_PUZZLE);
	}
	else if (mApp->IsChallengeMode())
	{
		mApp->KillAwardScreen();
		mApp->ShowChallengeScreen(CHALLENGE_PAGE_CHALLENGE);
	}
	else
	{
		int aLevel = mApp->mPlayerInfo->GetLevel();
		if (aLevel == 1)
		{
			mApp->KillAwardScreen();
			if (mApp->HasFinishedAdventure())
			{
				mApp->ShowAwardScreen(AWARD_CREDITS_ZOMBIENOTE, false);
			}
			else
			{
				mApp->PreNewGame(GAMEMODE_ADVENTURE, false);
			}
		}
		else
		{
			if (aLevel == 15)
			{
				mApp->DoAlmanacDialog()->WaitForResult();
			}
			else if (aLevel == 25)
			{
				StoreScreen* aStore = mApp->ShowStoreScreen();
				aStore->SetupForIntro(301);
				aStore->WaitForResult(true);

				if (aStore->mPurchasedFullVersion)
				{
					mApp->KillAwardScreen();
					mApp->ShowGameSelector();
					return;
				}
				if (mApp->IsTrialStageLocked())
				{
					mApp->KillAwardScreen();
					mApp->PreNewGame(GAMEMODE_UPSELL, false);
					if (!mApp->mPlayerInfo->mHasSeenUpsell)
					{
						mApp->mBoard->mStoreButton->mBtnNoDraw = true;
						mApp->mPlayerInfo->mHasSeenUpsell = true;
					}
					return;
				}
			}
			else if (aLevel == 35)
			{
				StoreScreen* aStore = mApp->ShowStoreScreen();
				aStore->SetupForIntro(601);
				aStore->WaitForResult(true);
			}
			else if (aLevel == 42)
			{
				StoreScreen* aStore = mApp->ShowStoreScreen();
				aStore->SetupForIntro(3100);
				aStore->WaitForResult(true);
			}
			else if (aLevel == 45)
			{
				mApp->KillAwardScreen();
				mApp->PreNewGame(GAMEMODE_CHALLENGE_ZEN_GARDEN, false);
				mApp->mZenGarden->SetupForZenTutorial();
				return;
			}

			mApp->KillAwardScreen();
			mApp->PreNewGame(GAMEMODE_ADVENTURE, false);
		}
	}
}

//0x4079F0
// GOTY @Patoke: 0x4097A0
void AwardScreen::MouseDown(int x, int y, int theClickCount)
{
	(void)x;(void)y;
	if (theClickCount == 1) {
		mStartButton->Update(); // @Patoke: implemented
		mMenuButton->Update();
		mContinueButton->Update();
		if (mStartButton->IsMouseOver() || mMenuButton->IsMouseOver())
			mApp->PlaySample(Sexy::SOUND_TAP);
	}
}

//0x407A70
// GOTY @Patoke: 0x409840
void AwardScreen::MouseUp(int x, int y, int theClickCount)
{
	(void)x;(void)y;
	if (theClickCount == 1)
	{
		if (mStartButton->IsMouseOver())
			StartButtonPressed();
		if (mContinueButton->IsMouseOver()) // @Patoke: add call
			AchievementsContinuePressed();
		if (mMenuButton->IsMouseOver())
		{
			mApp->KillAwardScreen();
			mApp->ShowGameSelector();
		}
	}
}

// @Patoke: implement functions
// GOTY @Patoke: 0x407C20
void AwardScreen::DrawAchievements(Graphics* g) {
	g->DrawImage(IMAGE_CHALLENGE_BACKGROUND, 0, 0);

	TodDrawString(g, _S("ACHIEVEMENTS"), BOARD_WIDTH / 2, 58, FONT_HOUSEOFTERROR28, Color(220, 220, 220), DS_ALIGN_CENTER);

	for (int i = 0; i < (int)mAchievementItems.size(); i++) {
		SexyString aAchievementName = TodStringTranslate(gAchievementList[mAchievementItems[i].mId].name);
		SexyString aAchievementDesc = gAchievementList[mAchievementItems[i].mId].description;

		aAchievementName.append(_S(" Earned!"));
#ifdef _HAS_UNUSED_ACHIEVEMENTS
		if (i < (int)AchievementId::SultanOfSpin)
#endif
		{
			int spaceOffset = 0;

			if (mAchievementItems[i].mId % 7 == 1 && mAchievementItems[i].mId / 7 < 2 || mAchievementItems[i].mId % 7 < 2 && mAchievementItems[i].mId / 7 == 2)
				spaceOffset = 2;
			else if (mAchievementItems[i].mId % 7 > 0 || mAchievementItems[i].mId / 7 == 2)
				spaceOffset = 1;

			Rect aSrcRect = Rect(70 * (mAchievementItems[i].mId % 7) + spaceOffset, 70 * (mAchievementItems[i].mId / 7), 70, 70);
			Rect aDestRect = Rect(240, mAchievementItems[i].mY + 10, 70, 70);

			g->DrawImage(IMAGE_ACHEESEMENTS_ICONS, aDestRect, aSrcRect);
		}

		TodDrawString(g, aAchievementName, BOARD_WIDTH / 2 + 50, mAchievementItems[i].mY + 25, FONT_DWARVENTODCRAFT15, Color(224, 187, 98), DS_ALIGN_CENTER);

		Rect aTextRect = Rect(300, mAchievementItems[i].mY + 20, 300, 60);
		TodDrawStringWrapped(g, aAchievementDesc, aTextRect, FONT_DWARVENTODCRAFT12, Color(255, 255, 255), DS_ALIGN_CENTER_VERTICAL_MIDDLE);
	}
}

// GOTY @Patoke: 0x409400
void AwardScreen::AchievementsContinuePressed() {
	// @Patoke: implemented
	if (mAwardType == AWARD_ACHIEVEMENTONLY) {
		mApp->KillAwardScreen();
		mApp->PreNewGame(GAMEMODE_ADVENTURE, false);
	}
	else {
		mStartButton->mBtnNoDraw = !mShowStartButtonAfterAchievements;
		mStartButton->mDisabled = !mShowStartButtonAfterAchievements;
		mMenuButton->mBtnNoDraw = !mShowMenuButtonAfterAchievements;
		mMenuButton->mDisabled = !mShowMenuButtonAfterAchievements;
		mContinueButton->mDisabled = true;
		mContinueButton->mBtnNoDraw = true;
		mShowingAchievements = false;
		int level = mApp->mPlayerInfo->GetLevel();
		if (level == 1 && mApp->HasFinishedAdventure()) {
			mApp->KillAwardScreen();
			mApp->ShowAwardScreen(AWARD_CREDITS_ZOMBIENOTE, false);
		}
	}
}