#ifndef __ACHIEVEMENTSSCREEN_H__
#define __ACHIEVEMENTSSCREEN_H__
// @Patoke: implement file

#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../GameConstants.h"

class LawnApp;

using namespace Sexy;

enum AchievementId {
	HomeSecurity,
	NovelPeasPrize,
	BetterOffDead,
	ChinaShop,
	Spudow,
	Explodonator,
	Morticulturalist,
	DontPea,
	RollSomeHeads,
	Grounded,
	Zombologist,
	PennyPincher,
	SunnyDays,
	PopcornParty,
	GoodMorning,
	NoFungusAmongUs,
	BeyondTheGrave,
	Immortal,
	ToweringWisdom,
	MustacheMode,
	DiscoIsUndead,
#ifdef _HAS_UNUSED_ACHIEVEMENTS
	SultanOfSpin,
	DisrespectTheDead,
	HeavyWeapons,
	ZenProfit,
	MayNotContainNuts,
	EvenMorticulturalist,
	BeatIt,
	GoldFarmer,
	FaceToFace,
#endif
    MAX_ACHIEVEMENTS
};

class AchievementItem {
public:
	SexyString name;
	SexyString description;
};

extern AchievementItem gAchievementList[MAX_ACHIEVEMENTS];

class AchievementsWidget : public Widget {
public:
	LawnApp*	mApp;                       //+GOTY @Patoke: 0xA8
	int			mScrollDirection;			//+GOTY @Patoke: 0xAC
	Rect		mMoreRockRect;				//+GOTY @Patoke: 0xC0
	int			mScrollValue;				//+GOTY @Patoke: 0xB0
	int			mScrollDecay;				//+GOTY @Patoke: 0xB4
	int			mDefaultScrollValue;		//+GOTY @Patoke: 0xB8
	bool		mDidPressMoreButton;		//+GOTY @Patoke: 0xBC

	AchievementsWidget(LawnApp* theApp);
	virtual ~AchievementsWidget();

	virtual void                Update();
	virtual void                Draw(Graphics* g);
	virtual void                KeyDown(KeyCode theKey);
	virtual void                MouseDown(int x, int y, int theClickCount);
	virtual void                MouseUp(int x, int y, int theClickCount);
	virtual void				MouseWheel(int theDelta);
};

class ReportAchievement {
public:
	static void GiveAchievement(LawnApp* theApp, int theAchievement, bool theForceGive);
	static void AchievementInitForPlayer(LawnApp* theApp);
};

#endif