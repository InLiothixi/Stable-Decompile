#include "../Board.h"
#include "../Plant.h"
#include "../Zombie.h"
#include "GameButton.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/PoolEffect.h"
#include "../System/ReanimationLawn.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../SexyAppFramework/Font.h"

bool gZombieDefeated[NUM_ZOMBIE_TYPES] = { false };

//0x401010
AlmanacDialog::AlmanacDialog(LawnApp* theApp) : LawnDialog(theApp, DIALOG_ALMANAC, true, _S("Almanac"), _S(""), _S(""), BUTTONS_NONE)
{
	mApp = (LawnApp*)gSexyAppBase;
	mOpenPage = ALMANAC_PAGE_INDEX;
	mSelectedSeed = SEED_PEASHOOTER;
	mSelectedZombie = ZOMBIE_NORMAL;
	mZombie = nullptr;
	mPlant = nullptr;
	mDrawStandardBack = false;
	mScrollAmount = 0;
	TodLoadResources("DelayLoad_Almanac");
	for (int i = 0; i < LENGTH(mZombiePerfTest); i++) mZombiePerfTest[i] = nullptr;
	LawnDialog::Resize(0, 0, BOARD_WIDTH, BOARD_HEIGHT);

	mCloseButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_CLOSE);
	mCloseButton->SetLabel(_S("[CLOSE_BUTTON]"));
	mCloseButton->mButtonImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTON;
	mCloseButton->mOverImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTONHIGHLIGHT;
	mCloseButton->mDownImage = nullptr;
	mCloseButton->SetFont(Sexy::FONT_BRIANNETOD12);
	Color aColor = Color(42, 42, 90);
	mCloseButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
	mCloseButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
	mCloseButton->Resize(676, 567, 89, 26);
	mCloseButton->mParentWidget = this;
	mCloseButton->mTextOffsetX = -8;
	mCloseButton->mTextOffsetY = 1;

	mIndexButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_INDEX);
	mIndexButton->SetLabel(_S("[ALMANAC_INDEX]"));
	mIndexButton->mButtonImage = Sexy::IMAGE_ALMANAC_INDEXBUTTON;
	mIndexButton->mOverImage = Sexy::IMAGE_ALMANAC_INDEXBUTTONHIGHLIGHT;
	mIndexButton->mDownImage = nullptr;
	mIndexButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mIndexButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
	mIndexButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
	mIndexButton->Resize(32, 567, 164, 26);
	mIndexButton->mParentWidget = this;
	mIndexButton->mTextOffsetX = 8;
	mIndexButton->mTextOffsetY = 1;

	mPlantButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_PLANT);
	mPlantButton->SetLabel(_S("[VIEW_PLANTS]"));
	mPlantButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON;
	mPlantButton->mOverImage = nullptr;
	mPlantButton->mDownImage = nullptr;
	mPlantButton->mDisabledImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_DISABLED;
	mPlantButton->mOverOverlayImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_GLOW;
	mPlantButton->SetFont(Sexy::FONT_DWARVENTODCRAFT18YELLOW);
	mPlantButton->mColors[ButtonWidget::COLOR_LABEL] = Color::White;
	mPlantButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color::White;
	mPlantButton->Resize(130, 345, 156, 42);
	mPlantButton->mTextOffsetY = -1;
	mPlantButton->mParentWidget = this;

	mZombieButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_ZOMBIE);
	mZombieButton->SetLabel(_S("[VIEW_ZOMBIES]"));
	mZombieButton->Resize(487, 345, 210, 48);
	mZombieButton->mDrawStoneButton = true;
	mZombieButton->mParentWidget = this;

	mScrollbar = new ScrollbarWidget(mApp, ScrollbarMode::VERTICAL);
	mScrollbar->SetDisabled(true);

	SetPage(ALMANAC_PAGE_INDEX);
	if (!mApp->mBoard || !mApp->mBoard->mPaused)
		mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);
}

//0x401880 & 0x4018A0
AlmanacDialog::~AlmanacDialog()
{
	if (mCloseButton)	delete mCloseButton;
	if (mIndexButton)	delete mIndexButton;
	if (mPlantButton)	delete mPlantButton;
	if (mZombieButton)	delete mZombieButton;
	if (mScrollbar)		delete mScrollbar;
	ClearPlantsAndZombies();
	mFollowersList.clear();

}

//0x401970
void AlmanacDialog::ClearPlantsAndZombies()
{
	if (mPlant)
	{
		mPlant->Die();
		delete mPlant;
		mPlant = nullptr;
	}
	if (mZombie)
	{
		mZombie->DieNoLoot();
		delete mZombie;
		mZombie = nullptr;
	}
	for (Zombie* &aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->DieNoLoot();
			delete aZombie;
		}

		aZombie = nullptr;
	}

	for (size_t i = 0; i < mFollowersList.size();)
	{
		Zombie* aZombie = mFollowersList[i];
		if (aZombie)
		{
			aZombie->DieNoLoot();
			delete aZombie;
		}
		mFollowersList.erase(mFollowersList.begin() + i);
	}

}

//0x401A10
void AlmanacDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	this->RemoveWidget(mScrollbar);
	ClearPlantsAndZombies();
}

//0x401A30
void AlmanacDialog::SetupPlant()
{
	ClearPlantsAndZombies();

	float aPosX = ALMANAC_PLANT_POSITION_X;
	float aPosY = ALMANAC_PLANT_POSITION_Y;
	if (mSelectedSeed == SEED_TALLNUT)				aPosY += 18;
	else if (mSelectedSeed == SEED_COBCANNON)		aPosX -= 40;
	else if (mSelectedSeed == SEED_FLOWERPOT)		aPosY -= 20;
	else if (mSelectedSeed == SEED_INSTANT_COFFEE)	aPosY += 20;
	else if (mSelectedSeed == SEED_GRAVEBUSTER)		aPosY += 55;

	mPlant = new Plant();
	mPlant->mBoard = nullptr;
	mPlant->mIsOnBoard = false;
	mPlant->PlantInitialize(0, 0, mSelectedSeed, SEED_NONE);
	mPlant->mX = aPosX;
	mPlant->mY = aPosY;
}

//0x401B70
void AlmanacDialog::SetupZombie()
{
	ClearPlantsAndZombies();

	mZombie = new Zombie();
	mZombie->mBoard = nullptr;
	mZombie->ZombieInitialize(0, mSelectedZombie, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
	mZombie->mX = mZombie->mPosX = ALMANAC_ZOMBIE_POSITION_X;
	mZombie->mY = mZombie->mPosY = ALMANAC_ZOMBIE_POSITION_Y;

	if (mSelectedZombie == ZombieType::ZOMBIE_BOBSLED)
	{
		for (int _i = 0; _i < 4; _i++)
		{
			Zombie* aFollower = new Zombie();
			aFollower->mBoard = nullptr;
			aFollower->ZombieInitialize(0, ZombieType::ZOMBIE_BOBSLED, false, mZombie, Zombie::ZOMBIE_WAVE_UI);
			aFollower->mX = aFollower->mPosX = mZombie->mPosX + 50 + 50 * _i;
			aFollower->mY = aFollower->mPosY = mZombie->mPosY;

			aFollower->PlayZombieReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aFollower->mApp->ReanimationGet(aFollower->mBodyReanimID)->mAnimTime = 1.0f;
			aFollower->mAltitude = 18.0f;
			aFollower->mZombiePhase = ZombiePhase::PHASE_BOBSLED_BOARDING;
			
			mFollowersList.push_back(aFollower);
		}
	}

	mScrollbar->mScrollValue = 0;
	mScrollbar->mScrollRange = 0;
	mScrollbar->Resize(0, 0, 0, 0);
	mScrollbar->mViewport = Rect();
	mScrollAmount = 0;
}

//0x401BE0
void AlmanacDialog::SetPage(AlmanacPage thePage)
{
	mOpenPage = thePage;
	ClearPlantsAndZombies();

	if (mOpenPage == AlmanacPage::ALMANAC_PAGE_INDEX)
	{
		mPlant = new Plant();
		mPlant->mBoard = nullptr;
		mPlant->mIsOnBoard = false;
		mPlant->PlantInitialize(0, 0, SeedType::SEED_SUNFLOWER, SeedType::SEED_NONE);
		mPlant->mX = ALMANAC_INDEXPLANT_POSITION_X;
		mPlant->mY = ALMANAC_INDEXPLANT_POSITION_Y;

		mZombie = new Zombie();
		mZombie->mBoard = nullptr;
		mZombie->ZombieInitialize(0, ZombieType::ZOMBIE_NORMAL, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
		mZombie->mPosX = ALMANAC_INDEXZOMBIE_POSITION_X;
		mZombie->mPosY = ALMANAC_INDEXZOMBIE_POSITION_Y;

		mIndexButton->mBtnNoDraw = true;
		mPlantButton->mBtnNoDraw = false;
		mZombieButton->mBtnNoDraw = false;
		mScrollbar->SetDisabled(true);
	}
	else
	{
		if (mOpenPage == AlmanacPage::ALMANAC_PAGE_PLANTS)
			SetupPlant();
		else if (mOpenPage == AlmanacPage::ALMANAC_PAGE_ZOMBIES)
			SetupZombie();
		else return;

		mIndexButton->mBtnNoDraw = false;
		mPlantButton->mBtnNoDraw = true;
		mZombieButton->mBtnNoDraw = true;
	}
}

void AlmanacDialog::ShowPlant(SeedType theSeedType)
{
	mSelectedSeed = theSeedType;
	SetPage(ALMANAC_PAGE_PLANTS);
}

void AlmanacDialog::ShowZombie(ZombieType theZombieType)
{
	mSelectedZombie = theZombieType;
	SetPage(ALMANAC_PAGE_ZOMBIES);
}

//0x401D30
void AlmanacDialog::Update()
{
	mCloseButton->Update();
	mIndexButton->Update();
	mPlantButton->Update();
	mZombieButton->Update();
	if (mPlant) mPlant->Update();
	if (mZombie) mZombie->Update();
	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->Update();
		}
	}

	for (Zombie* aZombie : mFollowersList)
	{
		if (aZombie) aZombie->Update();
	}
	mScrollbar->Update();

	if (mApp->mWidgetManager->mOverWidget)
	{
		int aMouseX = mApp->mWidgetManager->mLastMouseX;
		int aMouseY = mApp->mWidgetManager->mLastMouseY;
		if (SeedHitTest(aMouseX, aMouseY) != SeedType::SEED_NONE || ZombieHitTest(aMouseX, aMouseY) != ZombieType::ZOMBIE_INVALID ||
			mCloseButton->IsMouseOver() || mIndexButton->IsMouseOver() || mPlantButton->IsMouseOver() || mZombieButton->IsMouseOver() || mScrollbar->isThumbOver())
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}

	Widget::Update();

	if (!mScrollbar->mDisabled)
	{
		float aScrollSpeed = 1 + abs(mScrollAmount) * 0.1f;
		mScrollbar->mScrollValue = ClampFloat(mScrollbar->mScrollValue += mScrollAmount * aScrollSpeed, 0, mScrollbar->mScrollRange);
		mScrollAmount *= 0.9f;
	}
	
	mApp->mPoolEffect->PoolEffectUpdate();
	MarkDirty();
}

ZombieType AlmanacDialog::GetZombieType(int theIndex)
{
	return theIndex < NUM_ZOMBIE_TYPES ? (ZombieType)theIndex : ZOMBIE_INVALID;
}

//0x401E70
void AlmanacDialog::DrawIndex(Graphics* g)
{
	g->DrawImage(Sexy::IMAGE_ALMANAC_INDEXBACK, 0, 0);
	TodDrawString(g, _S("[SUBURBAN_ALMANAC_INDEX]"), BOARD_WIDTH / 2, 60, Sexy::FONT_HOUSEOFTERROR28, Color(220, 220, 220), DrawStringJustification::DS_ALIGN_CENTER);
	
	if (mPlant)
	{
		g->PushState();
		mPlant->BeginDraw(g);
		mPlant->Draw(g);
		g->PopState();
	}
	if (mZombie)
	{
		g->PushState();
		mZombie->BeginDraw(g);
		mZombie->DrawShadow(g);
		mZombie->Draw(g);
		g->PopState();
	}
}

//0x402060
void AlmanacDialog::DrawPlants(Graphics* g)
{
	g->DrawImage(Sexy::IMAGE_ALMANAC_PLANTBACK, 0, 0);
	TodDrawString(g, _S("[SUBURBAN_ALMANAC_PLANTS]"), BOARD_WIDTH / 2, 48, Sexy::FONT_HOUSEOFTERROR20, Color(213, 159, 43), DrawStringJustification::DS_ALIGN_CENTER);

	SeedType aSeedMouseOn = SeedHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY);
	for (SeedType aSeedType = SeedType::SEED_PEASHOOTER; aSeedType < NUM_ALMANAC_SEEDS; aSeedType = (SeedType)(aSeedType + 1))
	{
		int aPosX, aPosY;
		GetSeedPosition(aSeedType, aPosX, aPosY);
		if (mApp->SeedTypeAvailable(aSeedType))
		{
			if (aSeedType == SeedType::SEED_IMITATER)
			{
				if (aSeedType == aSeedMouseOn)
					g->DrawImage(Sexy::IMAGE_ALMANAC_IMITATER, aPosX, aPosY);
				g->DrawImage(Sexy::IMAGE_ALMANAC_IMITATER, aPosX, aPosY);
			}
			else
			{
				DrawSeedPacket(g, aPosX, aPosY, aSeedType, SeedType::SEED_NONE, 0, 255, true, false);
				if (aSeedType == aSeedMouseOn)
					g->DrawImage(Sexy::IMAGE_SEEDPACKETFLASH, aPosX, aPosY);
			}
		}
	}

	if (mSelectedSeed == SeedType::SEED_LILYPAD || mSelectedSeed == SeedType::SEED_TANGLEKELP || 
		mSelectedSeed == SeedType::SEED_CATTAIL || mSelectedSeed == SeedType::SEED_SEASHROOM)
	{
		bool aNight = mSelectedSeed == SeedType::SEED_SEASHROOM;
		g->DrawImage(aNight ? Sexy::IMAGE_ALMANAC_GROUNDNIGHTPOOL : Sexy::IMAGE_ALMANAC_GROUNDPOOL, 521, 107);

		if (mApp->Is3DAccelerated())
		{
			g->SetClipRect(475, 0, 397, 500);
			g->mTransY -= 145;
			mApp->mPoolEffect->PoolEffectDraw(g, aNight);
			g->mTransY += 145;
			g->ClearClipRect();
		}
	}
	else
	{
		g->DrawImage(
			Plant::IsNocturnal(mSelectedSeed) || mSelectedSeed == SeedType::SEED_GRAVEBUSTER || mSelectedSeed == SeedType::SEED_PLANTERN ? Sexy::IMAGE_ALMANAC_GROUNDNIGHT :
			mSelectedSeed == SeedType::SEED_FLOWERPOT ? Sexy::IMAGE_ALMANAC_GROUNDROOF : Sexy::IMAGE_ALMANAC_GROUNDDAY,
			521, 107
		);
	}
	
	if (mPlant)
	{
		g->PushState();
		mPlant->BeginDraw(g);
		mPlant->Draw(g);
		g->PopState();
	}

	g->DrawImage(Sexy::IMAGE_ALMANAC_PLANTCARD, 459, 86);
	PlantDefinition& aPlantDef = GetPlantDefinition(mSelectedSeed);
	SexyString aName = Plant::GetNameString(mSelectedSeed, SEED_NONE);
	SexyString aDescriptionName = StrFormat(_S("[%s_DESCRIPTION]"), aPlantDef.mPlantName);
	TodDrawString(g, aName, 617, 288, Sexy::FONT_DWARVENTODCRAFT18YELLOW, Color::White, DS_ALIGN_CENTER);
	TodDrawStringWrapped(g, aDescriptionName, Rect(485, 309, 258, 230), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), DS_ALIGN_LEFT);

	if (mSelectedSeed != SeedType::SEED_IMITATER)
	{
		SexyString aCostStr = TodReplaceString(StrFormat(_S("{KEYWORD}{COST}:{STAT} %d"), aPlantDef.mSeedCost), _S("{COST}"), _S("[COST]"));
		TodDrawStringWrapped(g, aCostStr, Rect(485, 520, 134, 50), Sexy::FONT_BRIANNETOD12, Color::White, DS_ALIGN_LEFT);

		SexyString aRechargeStr = TodReplaceString(
			_S("{KEYWORD}{WAIT_TIME}: {STAT}{WAIT_TIME_LENGTH}"), 
			_S("{WAIT_TIME_LENGTH}"),
			aPlantDef.mRefreshTime == 750 ? _S("[WAIT_TIME_SHORT]") : aPlantDef.mRefreshTime == 3000 ? _S("[WAIT_TIME_LONG]") : _S("[WAIT_TIME_VERY_LONG]")
		);
		aRechargeStr = TodReplaceString(aRechargeStr, _S("{WAIT_TIME}"), _S("[WAIT_TIME]"));
		TodDrawStringWrapped(g, aRechargeStr, Rect(600, 520, 139, 50), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), DS_ALIGN_RIGHT);
	}
}

//0x402C00
void AlmanacDialog::DrawZombies(Graphics* g)
{
	g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEBACK, 0, 0);
	TodDrawString(g, _S("[SUBURBAN_ALMANAC_ZOMBIES]"), BOARD_WIDTH / 2, 54, Sexy::FONT_DWARVENTODCRAFT24, Color(0, 196, 0), DS_ALIGN_CENTER);

	ZombieType aZombieMouseOn = ZombieHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY);
	for (int i = 0; i < NUM_ALMANAC_ZOMBIES; i++)
	{
		ZombieType aZombieType = GetZombieType(i);
		int aPosX, aPosY;
		GetZombiePosition(aZombieType, aPosX, aPosY);
		if (aZombieType != ZombieType::ZOMBIE_INVALID)
		{
			if (!ZombieIsShown(aZombieType))
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEBLANK, aPosX, aPosY);
			else
			{
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
				if (aZombieType == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}

				ZombieType aZombieTypeToDraw = aZombieType;
				g->PushState();
				g->SetClipRect(aPosX + 2, aPosY + 2, 72, 72);
				g->Translate(aPosX + 1, aPosY - 6);
				g->mScaleX = 0.5f;
				g->mScaleY = 0.5f;
				switch (aZombieType)
				{
				case ZombieType::ZOMBIE_POLEVAULTER:
				{
					g->TranslateF(-24.25f, -3);
					aZombieTypeToDraw = ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE;
				}
				break;
				case ZombieType::ZOMBIE_FLAG:			g->TranslateF(2, 10);		break;
				case ZombieType::ZOMBIE_TRAFFIC_CONE:	g->TranslateF(0, 12);		break;
				case ZombieType::ZOMBIE_PAIL:			g->TranslateF(0, 9);		break;
				case ZombieType::ZOMBIE_FOOTBALL:		g->TranslateF(-15, -1);	break;
				case ZombieType::ZOMBIE_ZAMBONI:		g->TranslateF(-25, 3);		break;
				case ZombieType::ZOMBIE_DOLPHIN_RIDER:	g->TranslateF(-2, -10);	break;
				case ZombieType::ZOMBIE_POGO:			g->TranslateF(0, -3);		break;
				case ZombieType::ZOMBIE_GARGANTUAR:		g->TranslateF(15, 17);		break;
				case ZombieType::ZOMBIE_IMP:			g->TranslateF(-8, -7);		break;
				case ZombieType::ZOMBIE_BUNGEE:			g->TranslateF(-4, 3);		break;
#ifdef _HAS_NEW_DANCER
				case ZombieType::ZOMBIE_BACKUP_DANCER:	g->TranslateF(-2, 5);		break;
#else
				case ZombieType::ZOMBIE_BACKUP_DANCER:	g->TranslateF(-8, 5);		break;
#endif
				case ZombieType::ZOMBIE_SNORKEL:		g->TranslateF(-10, 0);		break;
				case ZombieType::ZOMBIE_YETI:			g->TranslateF(0, 4);		break;
				case ZombieType::ZOMBIE_CATAPULT:		g->TranslateF(-24, -1);	break;
				case ZombieType::ZOMBIE_BOBSLED:		g->TranslateF(0, -8);		break;
				case ZombieType::ZOMBIE_LADDER:			g->TranslateF(0, -3);		break;
				default: break;
				}
				if (ZombieHasSilhouette(aZombieType))
				{
					g->SetColor(Color(0, 0, 0, 40));
					g->SetColorizeImages(true);
				}
				if (aZombieType == ZombieType::ZOMBIE_BUNGEE)
				{
					g->mTransY -= 300;
				}
				mApp->mReanimatorCache->DrawCachedZombie(g, 0, 0, aZombieTypeToDraw);
				g->PopState();

				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
				if (aZombieType == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}
			}
		}
	}

	g->DrawImage(mZombie->mZombieType == ZombieType::ZOMBIE_ZAMBONI || mZombie->mZombieType == ZombieType::ZOMBIE_BOBSLED ?
		Sexy::IMAGE_ALMANAC_GROUNDICE : Sexy::IMAGE_ALMANAC_GROUNDDAY, 518, 110);
	if (mZombie && !ZombieHasSilhouette(mZombie->mZombieType))
	{
		int followerID = 0;
		for (Zombie* aFolllower : mFollowersList) {
			if (aFolllower)
			{
				followerID++;
				
				g->PushState();
				aFolllower->BeginDraw(g);
				g->SetClipRect(-42 - 50 * followerID, -51, 197, 187);
				if (aFolllower->mZombieType != ZombieType::ZOMBIE_BUNGEE && aFolllower->mZombieType != ZombieType::ZOMBIE_BOSS &&
					aFolllower->mZombieType != ZombieType::ZOMBIE_ZAMBONI && aFolllower->mZombieType != ZombieType::ZOMBIE_CATAPULT)
					aFolllower->DrawShadow(g);
				if (followerID == 1) {
					ZombieDrawPosition aDrawPos;
					aFolllower->GetDrawPos(aDrawPos);
					g->DrawImage(IMAGE_ZOMBIE_BOBSLED_INSIDE, aFolllower->mPosX + aDrawPos.mImageOffsetX - aFolllower->mPosX - 76.0f - 50, 15.0f);
				}
				aFolllower->Draw(g);
				g->PopState();
			}
		}

		g->PushState();
		mZombie->BeginDraw(g);
		g->SetClipRect(-42, -51, 197, 187);
		switch (mZombie->mZombieType)
		{
		case ZombieType::ZOMBIE_ZAMBONI:		g->TranslateF(-30, 5);		break;
		case ZombieType::ZOMBIE_GARGANTUAR:		g->TranslateF(0, 40);		break;
		case ZombieType::ZOMBIE_FOOTBALL:		g->TranslateF(-10, 0);		break;
		case ZombieType::ZOMBIE_BALLOON:		g->TranslateF(0, -20);		break;
		case ZombieType::ZOMBIE_BUNGEE:			g->TranslateF(15, 0);		break;
		case ZombieType::ZOMBIE_CATAPULT:		g->TranslateF(-10, 0);		break;
		case ZombieType::ZOMBIE_BOSS:			g->TranslateF(-540, -175);	break;
		default: break;
		}
		if (mZombie->mZombieType != ZombieType::ZOMBIE_BUNGEE && mZombie->mZombieType != ZombieType::ZOMBIE_BOSS &&
			mZombie->mZombieType != ZombieType::ZOMBIE_ZAMBONI && mZombie->mZombieType != ZombieType::ZOMBIE_CATAPULT)
			mZombie->DrawShadow(g);
		mZombie->Draw(g);

		if (mZombie->mZombieType == ZombieType::ZOMBIE_BOBSLED) {
			ZombieDrawPosition aDrawPos;
			mZombie->GetDrawPos(aDrawPos);
			g->SetClipRect(-42, -51, 197, 187);
			g->DrawImage(IMAGE_ZOMBIE_BOBSLED1, mZombie->mPosX + aDrawPos.mImageOffsetX - mZombie->mPosX - 76.0f, 15.0f);
		}
		g->PopState();
	}
	g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIECARD, 455, 78);

	ZombieDefinition& aZombieDef = GetZombieDefinition(mSelectedZombie);
	SexyString aName = ZombieHasSilhouette(mSelectedZombie) ? _S("???") : StrFormat(_S("[%s]"), aZombieDef.mZombieName);
	TodDrawString(g, aName, 613, 362, Sexy::FONT_DWARVENTODCRAFT18GREENINSET, Color(190, 255, 235, 255), DS_ALIGN_CENTER);

	SexyString aHeader;
	SexyString aDescription;

	DrawStringJustification aAlign;
	if (ZombieHasDescription(mSelectedZombie))
	{
		aHeader = TodStringTranslate(StrFormat(_S("[%s_HEADER]"), aZombieDef.mZombieName));
		aDescription = TodStringTranslate(StrFormat(_S("[%s_DESCRIPTION]"), aZombieDef.mZombieName));
		aAlign = DS_ALIGN_LEFT;
	}
	else
	{
		aHeader = _S("");
		aDescription = _S("[NOT_ENCOUNTERED_YET]");
		aAlign = DS_ALIGN_CENTER_VERTICAL_MIDDLE;
	}
	for (TodStringListFormat& aFormat : gLawnStringFormats)
	{
		if (TestBit(aFormat.mFormatFlags, TodStringFormatFlag::TOD_FORMAT_HIDE_UNTIL_MAGNETSHROOM))
		{
			if (mApp->HasSeedType(SeedType::SEED_MAGNETSHROOM))
			{
				aFormat.mNewColor.mAlpha = 255;
				aFormat.mLineSpacingOffset = 0;
			}
			else
			{
				aFormat.mNewColor.mAlpha = 0;
				aFormat.mLineSpacingOffset = -17;
			}
		}
	}

	int aHeaderHeight = 0;
	if (ZombieHasDescription(mSelectedZombie)) aHeaderHeight = TodDrawStringWrappedHelper(g, aHeader, Rect(484, 377, 258, 170), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), aAlign, false);
	int aBoxHeight = 142 - aHeaderHeight + Sexy::FONT_BRIANNETOD12->GetHeight();
	int aDescriptionHeight = TodDrawStringWrappedHelper(g, aDescription, Rect(484, 377 + aHeaderHeight, 258, 170), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), aAlign, false);
	mScrollbar->mScrollRange = aDescriptionHeight - 142 + aHeaderHeight;
	mScrollbar->SetDisabled(mScrollbar->mScrollRange <= 34);
	int aClipWidthOffset = mScrollbar->mDisabled ? 10 : 0;

	g->PushState();
	g->SetClipRect(484, 377 , 258, aHeaderHeight);
	TodDrawStringWrapped(g, aHeader, Rect(484, 377, 258, 170), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), aAlign, true);
	g->PopState();

	g->PushState();
	g->mTransY -= mScrollbar->mScrollValue;
	g->SetClipRect(484, 377 + aHeaderHeight + mScrollbar->mScrollValue, 248 + aClipWidthOffset, aBoxHeight);

	mScrollbar->Resize(738, 377 + aHeaderHeight, 8, aBoxHeight);
	mScrollbar->mViewport = Rect(484, 377 + aHeaderHeight, 248 + aClipWidthOffset, aBoxHeight);
	
	TodDrawStringWrapped(g, aDescription, Rect(484, 377 + aHeaderHeight, 248 + aClipWidthOffset, 170), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), aAlign, true);
	g->PopState();
}
//0x403810
void AlmanacDialog::Draw(Graphics* g)
{
	g->SetLinearBlend(true);
	switch (mOpenPage)
	{
	case AlmanacPage::ALMANAC_PAGE_INDEX:	DrawIndex(g);	break;
	case AlmanacPage::ALMANAC_PAGE_PLANTS:	DrawPlants(g);	break;
	case AlmanacPage::ALMANAC_PAGE_ZOMBIES:	DrawZombies(g);	break;
	}

	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			g->PushState();
			aZombie->Draw(g);
			g->PopState();
		}
	}

	mCloseButton->Draw(g);
	mIndexButton->Draw(g);
	mPlantButton->Draw(g);
	mZombieButton->Draw(g);

	if (!mScrollbar->mDisabled)
	{
		mScrollbar->mButtonNoDraw = false;
		g->PushState();
		g->mTransX = g->mTransX + mScrollbar->mX;
		g->mTransY = g->mTransY + mScrollbar->mY;
		mScrollbar->Draw(g);
		mScrollbar->mButtonNoDraw = true;
		g->PopState();
	}
}

void AlmanacDialog::GetSeedPosition(SeedType theSeedType, int& x, int& y)
{
	if (theSeedType == SeedType::SEED_IMITATER)
		x = 20, y = 23;
	else
	{
		x = theSeedType % 8 * 52 + 26;
		y = theSeedType / 8 * 78 + 92;

		if (theSeedType > SeedType::SEED_IMITATER)
			x -= 52;
	}
}

//0x403940
SeedType AlmanacDialog::SeedHitTest(int x, int y)
{
	if (mMouseVisible && mOpenPage == AlmanacPage::ALMANAC_PAGE_PLANTS)
	{
		for (SeedType aSeedType = SeedType::SEED_PEASHOOTER; aSeedType < NUM_ALMANAC_SEEDS; aSeedType = (SeedType)(aSeedType + 1))
		{
			if (mApp->SeedTypeAvailable(aSeedType))
			{
				int aSeedX, aSeedY;
				GetSeedPosition(aSeedType, aSeedX, aSeedY);
				Rect aSeedRect = aSeedType == SeedType::SEED_IMITATER ? Rect(aSeedX, aSeedY, 34, 46) : Rect(aSeedX, aSeedY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
				if (aSeedRect.Contains(x, y)) return aSeedType;
			}
		}
	}
	return SeedType::SEED_NONE;
}

bool AlmanacDialog::ZombieHasSilhouette(ZombieType theZombieType)
{
	// 除雪人僵尸以外的其他僵尸，或者雪人僵尸已经可以刷出（已经到达或完成冒险模式二周目 4-10 关卡），则不会显示为剪影
	if (theZombieType != ZombieType::ZOMBIE_YETI || mApp->CanSpawnYetis())
		return false;

	// 排除上述情况后，若已完成雪人僵尸出现的关卡（冒险模式一周目 4-10 关卡），则雪人僵尸显示为剪影
	return mApp->HasFinishedAdventure() || mApp->mPlayerInfo->GetLevel() > GetZombieDefinition(ZombieType::ZOMBIE_YETI).mStartingLevel;
}

//0x403A10
bool AlmanacDialog::ZombieIsShown(ZombieType theZombieType)
{
	// 试玩模式下，仅展示潜水僵尸及其之前出现的僵尸
	if (mApp->IsTrialStageLocked() && theZombieType > ZombieType::ZOMBIE_SNORKEL)
		return false;

	// 对于雪人僵尸，要求其可以在刷怪中出现（已经到达或完成冒险模式二周目 4-10 关卡），
	// 或已得知其存在但未解锁其形象（已经完成冒险模式一周目 4-10 关卡，但未到达二周目 4-10 关卡）
	if (theZombieType == ZombieType::ZOMBIE_YETI)
		return mApp->CanSpawnYetis() || ZombieHasSilhouette(ZombieType::ZOMBIE_YETI);

	// 对于冒险模式中出现的僵尸
	//if (theZombieType <= ZombieType::ZOMBIE_BOSS)
	{
		// 冒险模式一周目完成后，图鉴展示所有僵尸
		if (mApp->HasFinishedAdventure())
			return true;

		int aLevel = mApp->mPlayerInfo->GetLevel();
		int aStart = GetZombieDefinition(theZombieType).mStartingLevel;
		// 要求已经达到僵尸首次出现的关卡
		// 对于不能通过自然刷怪出现的僵尸（小鬼僵尸、雪橇僵尸小队、伴舞僵尸），额外要求已通过其首次出现的关卡或已击败过该僵尸
		return aStart <= aLevel && (aStart != aLevel || !Board::IsZombieTypeSpawnedOnly(theZombieType) || gZombieDefeated[theZombieType]);
	}

	return false;
}

//0x403B30
bool AlmanacDialog::ZombieHasDescription(ZombieType theZombieType)
{
	int aLevel = mApp->mPlayerInfo->GetLevel();
	int aStart = GetZombieDefinition(theZombieType).mStartingLevel;

	// 对于雪人僵尸
	if (theZombieType == ZombieType::ZOMBIE_YETI)
	{
		// 当雪人僵尸不可在刷怪中出现时（冒险模式二周目 4-10 关卡之前），不显示僵尸描述
		if (!mApp->CanSpawnYetis())
			return false;
		// 从第三周目开始，总是显示雪人僵尸的描述
		if (mApp->mPlayerInfo->mFinishedAdventure >= 2)
			return true;
	}
	// 对于雪人僵尸外的其他僵尸，当冒险模式已完成时，总是显示僵尸的描述
	else if (mApp->HasFinishedAdventure())
		return true;

	// 雪人僵尸在二周目 4-10 关卡至三周目之间，或其他僵尸在冒险模式一周目中的情况，
	// 要求已经达到僵尸首次出现的关卡，且已通过其首次出现的关卡或已击败过该僵尸
	return aStart <= aLevel && (aStart != aLevel || gZombieDefeated[theZombieType]);
}

void AlmanacDialog::GetZombiePosition(ZombieType theZombieType, int& x, int& y)
{
	if (theZombieType == ZombieType::ZOMBIE_BOSS)
		x = 192, y = 486;
	else
	{
		int theIndex = (int) theZombieType;

		if (theZombieType >= ZombieType::ZOMBIE_BOSS)
			--theIndex;

		x = theIndex % 5 * 85 + 22;
		y = theIndex / 5 * 80 + 86;
	}
}

//0x403BB0
ZombieType AlmanacDialog::ZombieHitTest(int x, int y)
{
	if (mMouseVisible && mOpenPage == AlmanacPage::ALMANAC_PAGE_ZOMBIES)
	{
		for (int i = 0; i < NUM_ALMANAC_ZOMBIES; i++)
		{
			ZombieType aZombieType = GetZombieType(i);
			if (aZombieType != ZombieType::ZOMBIE_INVALID && ZombieIsShown(aZombieType))
			{
				int aZombieX, aZombieY;
				GetZombiePosition(aZombieType, aZombieX, aZombieY);
				if (Rect(aZombieX, aZombieY, 76, 76).Contains(x, y))
					return aZombieType;
			}
		}
	}
	return ZombieType::ZOMBIE_INVALID;
}

//0x403C60
void AlmanacDialog::MouseUp(int x, int y, int theClickCount)
{
	if (mPlantButton->IsMouseOver())		SetPage(ALMANAC_PAGE_PLANTS);
	else if (mZombieButton->IsMouseOver())	SetPage(ALMANAC_PAGE_ZOMBIES);
	else if (mCloseButton->IsMouseOver())	mApp->KillAlmanacDialog();
	else if (mIndexButton->IsMouseOver())	SetPage(ALMANAC_PAGE_INDEX);
}

//0x403D00
void AlmanacDialog::MouseDown(int x, int y, int theClickCount)
{
	if (mPlantButton->IsMouseOver() || mCloseButton->IsMouseOver() || mIndexButton->IsMouseOver())
		mApp->PlaySample(Sexy::SOUND_TAP);
	if (mZombieButton->IsMouseOver())
		mApp->PlaySample(Sexy::SOUND_GRAVEBUTTON);

	SeedType aSeedType = SeedHitTest(x, y);
	if (aSeedType != SeedType::SEED_NONE && aSeedType != mSelectedSeed)
	{
		mSelectedSeed = aSeedType;
		SetupPlant();
		mApp->PlaySample(Sexy::SOUND_TAP);
	}
	ZombieType aZombieType = ZombieHitTest(x, y);
	if (aZombieType != ZombieType::ZOMBIE_INVALID && aZombieType != mSelectedZombie)
	{
		mSelectedZombie = aZombieType;
		SetupZombie();
		mApp->PlaySample(Sexy::SOUND_TAP);
	}

	if(mScrollbar->isThumbOver())
	{
		mScrollbar->MouseDown(x, y, theClickCount);
	}
}

void AlmanacInitForPlayer()
{
	for (int i = 0; i < ZombieType::NUM_ZOMBIE_TYPES; i++)
		gZombieDefeated[i] = false;
}

void AlmanacPlayerDefeatedZombie(ZombieType theZombieType)
{
	gZombieDefeated[(int)theZombieType] = true;
}

void AlmanacDialog::MouseWheel(int theDelta)
{
	Widget::MouseWheel(theDelta);

	if (mScrollbar->mDisabled) return;

	mScrollAmount -= theDelta;
	mScrollAmount -= mScrollAmount * 0.1f;
}

void AlmanacDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	this->AddWidget(mScrollbar);
}

void AlmanacDialog::OrderInManagerChanged()
{
}