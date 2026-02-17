#ifndef __GAMESELECTOR_H__
#define __GAMESELECTOR_H__

#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "AchievementsScreen.h"
#include "ZombatarScreen.h"
#include "MoreScreen.h"
#include "QuickplayScreen.h"

class LawnApp;
class ToolTipWidget;
namespace Sexy
{
    class DialogButton;
}

using namespace Sexy;

enum SelectorAnimState
{
    SELECTOR_OPEN,
    SELECTOR_NEW_USER,
    SELECTOR_SHOW_SIGN,
    SELECTOR_IDLE
};

class GameSelector : public Widget, public ButtonListener
{
private:
    enum
    {
        GameSelector_Adventure = 100,
        GameSelector_Minigame,
        GameSelector_Puzzle,
        GameSelector_Options,
        GameSelector_Help,
        GameSelector_Quit,
        GameSelector_ChangeUser,
        GameSelector_Store,
        GameSelector_Almanac,
        GameSelector_ZenGarden,
        GameSelector_Survival,
#ifdef _HAS_ZOMBATAR
        GameSelector_Zombatar, // @Patoke
#endif
#ifdef _HAS_ACHIEVEMENTS
        GameSelector_AchievementsBack,
        GameSelector_Achievements,
#endif 
#ifdef _HAS_MORESCREEN
        GameSelector_QuickPlay,
#endif 
#ifdef _HAS_UNLOCK
        GameSelector_Unlock,
#endif
    };

public:
    LawnApp*                    mApp;                       //+0x8C
    NewLawnButton*              mAdventureButton;           //+0x90
    NewLawnButton*              mMinigameButton;            //+0x94
    NewLawnButton*              mPuzzleButton;              //+0x98
    NewLawnButton*              mOptionsButton;             //+0x9C
    NewLawnButton*              mQuitButton;                //+0xA0
    NewLawnButton*              mHelpButton;                //+0xA4
    NewLawnButton*              mStoreButton;               //+0xA8
    NewLawnButton*              mAlmanacButton;             //+0xAC
    NewLawnButton*              mZenGardenButton;           //+0xB0
    NewLawnButton*              mSurvivalButton;            //+0xB4
    NewLawnButton*              mChangeUserButton;          //+0xB8
#ifdef _HAS_ZOMBATAR
    NewLawnButton*              mZombatarButton;             //+GOTY @Patoke: 0xC0
#endif
#ifdef _HAS_ACHIEVEMENTS
    NewLawnButton*              mAchievementsButton;        //+GOTY @Patoke: 0xC4
#endif
    NewLawnButton*              mTrophyButton;
#ifdef _HAS_MORESCREEN
    NewLawnButton*              mQuickPlayButton;           //+GOTY @Patoke: 0xC8
#endif
#ifdef _HAS_UNLOCK
    NewLawnButton*              mUnlockButton;
#endif
    Widget*                     mOverlayWidget;             //+0xBC
    bool                        mStartingGame;              //+0xC0
    int                         mStartingGameCounter;       //+0xC4
    bool                        mMinigamesLocked;           //+0xC8
    bool                        mPuzzleLocked;              //+0xC9
    bool                        mSurvivalLocked;            //+0xCA
    bool                        mShowStartButton;           //+0xCB
    ParticleSystemID            mTrophyParticleID;          //+0xCC
    ReanimationID               mSelectorReanimID;          //+0xD0
    ReanimationID               mCloudReanimID[6];          //+0xD4
    int                         mCloudCounter[6];           //+0xEC
    ReanimationID               mFlowerReanimID[3];         //+0x104
    ReanimationID               mLeafReanimID;              //+0x110
    ReanimationID               mHandReanimID;              //+0x114
    int                         mLeafCounter;               //+0x118
    SelectorAnimState           mSelectorState;             //+0x11C
    int                         mLevel;                     //+0x120
    bool                        mLoading;                   //+0x124
    ToolTipWidget*              mToolTip;                   //+0x128
    bool                        mHasTrophy;                 //+0x12C
    bool                        mUnlockSelectorCheat;       //+0x12D
    int                         mSlideCounter;              //+GOTY @Patoke: 0x154
    int                         mStartX;                    //+GOTY @Patoke: 0x158
    int                         mStartY;                    //+GOTY @Patoke: 0x15C
    int                         mDestX;                     //+GOTY @Patoke: 0x160
    int                         mDestY;                     //+GOTY @Patoke: 0x164
#ifdef _HAS_ZOMBATAR
    ZombatarWidget*             mZombatarWidget;            //+GOTY @Patoke: 0x168
#endif
#ifdef _HAS_ACHIEVEMENTS
    AchievementsWidget*         mAchievementsWidget;        //+GOTY @Patoke: 0x16C
#endif
#ifdef _HAS_MORESCREEN
    MoreWidget*                 mMoreWidget;
#endif
#ifdef _HAS_LEVELSELECTOR
    QuickplayWidget*            mLevelSelectorWidget;
#endif
    ReanimationID               mWoodSignID;            
    ReanimationID               mSpotLightID;
    int                         mScrollOffset;

public:
    GameSelector(LawnApp* theApp);
    virtual ~GameSelector();

    void                        SyncProfile(bool theShowLoading);
    virtual void                Draw(Graphics* g);
    virtual void                DrawOverlay(Graphics* g);
    virtual void                Update();
    virtual void                AddedToManager(WidgetManager* theWidgetManager);
    virtual void                RemovedFromManager(WidgetManager* theWidgetManager);
    virtual void                OrderInManagerChanged();
    virtual void                ButtonMouseEnter(int theId);
    virtual void                ButtonPress(int theId, int theClickCount);
    virtual void                ButtonDepress(int theId);
    virtual void                KeyDown(KeyCode theKey);
    virtual void                KeyChar(char theChar);
    virtual void                MouseDown(int x, int y, int theClickCount);
    void                        TrackButton(DialogButton* theButton, const char* theTrackName, float theOffsetX, float theOffsetY);
    void                        SyncButtons();
    void                        AddTrophySparkle();
    void                        ClickedAdventure();
    void                        UpdateTooltip();
    /*inline*/ bool             ShouldDoZenTuturialBeforeAdventure();
    void                        AddPreviewProfiles();
    // @Patoke: implement functions
    /*inline*/ void             SlideTo(int theX, int theY);
#ifdef _HAS_ACHIEVEMENTS
    void                        ShowAchievementsScreen(); // @Patoke: unofficial name
#endif
#ifdef _HAS_ZOMBATAR
    void                        ShowZombatarScreen();
#endif
#ifdef _HAS_MORESCREEN
    void                        ShowMoreScreen();
#endif
#ifdef _HAS_LEVELSELECTOR
    void                        ShowQuickplayScreen();
#endif
    void                        ShowGameSelectorScreen();
    void                        DisableButtons(bool isDisabled);
};

class GameSelectorOverlay : public Widget
{
public:
    GameSelector*               mParent;                    //+0x88

public:
    GameSelectorOverlay(GameSelector* theGameSelector);
    virtual ~GameSelectorOverlay() { }

    virtual void Draw(Graphics* g);
};

#endif
