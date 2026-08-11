#include "../Board.h"
#include "GameButton.h"
#include "../Cutscene.h"
#include "AlmanacDialog.h"
#include "../LawnCommon.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "NewOptionsDialog.h"
#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

//0x45C050
NewOptionsDialog::NewOptionsDialog(LawnApp* theApp, bool theFromGameSelector) : 
	Dialog(nullptr, nullptr, Dialogs::DIALOG_NEWOPTIONS, true, _S("[OPTIONS]"), _S(""), _S(""), Dialog::BUTTONS_NONE)
{
    mApp = theApp;
    mFromGameSelector = theFromGameSelector;
    SetColor(Dialog::COLOR_BUTTON_TEXT, Color(255, 255, 100));
    mAlmanacButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Almanac, this, _S("[VIEW_ALMANAC_BUTTON]"));
    mRestartButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Restart, this, _S("[RESTART_LEVEL]"));
    mBackToMainButton = MakeButton(NewOptionsDialog::NewOptionsDialog_MainMenu, this, _S("[MAIN_MENU_BUTTON]"));

   /* mGameplayButton = MakeButton(NewOptionsDialog::NewOptionsDialog_VideoGraphics, this, _S("[GAMEPLAY_SETTINGS_BUTTON]"));
    mControllerButton = MakeButton(NewOptionsDialog::NewOptionsDialog_SoundSystem, this, _S("[CONTROLLER_BUTTON]"));
    mLanguageButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Language, this, _S("[LANGUAGE_BUTTON]"));*/

    mBackToGameButton = MakeNewButton(
        Dialog::ID_OK, 
        this, 
        _S("[BACK_TO_GAME]"), 
        nullptr, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON2
    );
    mBackToGameButton->mTranslateX = 0;
    mBackToGameButton->mTranslateY = 0;
    mBackToGameButton->mTextOffsetX = -2;
    mBackToGameButton->mTextOffsetY = -5;
    mBackToGameButton->mTextDownOffsetX = 0;
    mBackToGameButton->mTextDownOffsetY = 1;
    mBackToGameButton->SetFont(FONT_DWARVENTODCRAFT36GREENINSET);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL, Color::White);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color::White);
    mBackToGameButton->mHiliteFont = FONT_DWARVENTODCRAFT36BRIGHTGREENINSET;
    
    mMusicVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_MusicVolume, this);
    double aMusicVolume = theApp->GetMusicVolume();
    aMusicVolume = max(0.0, min(1.0, aMusicVolume));
    mMusicVolumeSlider->SetValue(aMusicVolume);

    mSfxVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_SoundVolume, this);
    mSfxVolumeSlider->SetValue(theApp->GetSfxVolume() / 0.65);

    mFullscreenCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_Fullscreen, this, !theApp->mIsWindowed);
    mHardwareAccelerationCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_HardwareAcceleration, this, theApp->mEnableVsync);
    mNativeHDRCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_NativeHDR, this, theApp->mEnableNativeHDR);

    if (mFromGameSelector)
    {
        mRestartButton->SetVisible(false);
        mBackToGameButton->SetLabel(_S("[DIALOG_BUTTON_OK]"));
        if (mApp->HasFinishedAdventure() && !mApp->IsTrialStageLocked())
        {
            mBackToMainButton->SetLabel(_S("[CREDITS]"));
        }
        else
        {
            mBackToMainButton->SetVisible(false);
        }
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ICE || 
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || 
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
    {
        mRestartButton->SetVisible(false);
    }
    if (mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO && !mApp->mBoard->mCutScene->IsSurvivalRepick())
    {
        mRestartButton->SetVisible(false);
    }
    if (!mApp->CanShowAlmanac() || 
        mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO || 
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN ||
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM || 
        mFromGameSelector)
    {
        mAlmanacButton->SetVisible(false);
    }

    //if (mFromGameSelector)
    //{
    //    mMusicVolumeSlider->SetVisible(false);
    //    mSfxVolumeSlider->SetVisible(false);
    //    mHardwareAccelerationCheckbox->SetVisible(false);
    //    mFullscreenCheckbox->SetVisible(false);
    //}
    //else
    //{
    //    /*mGameplayButton->SetVisible(false);
    //    mControllerButton->SetVisible(false);
    //    mLanguageButton->SetVisible(false);*/
    //}
}

//0x45C760, 0x45C780
NewOptionsDialog::~NewOptionsDialog()
{
    delete mMusicVolumeSlider;
    delete mSfxVolumeSlider;
    delete mFullscreenCheckbox;
    delete mHardwareAccelerationCheckbox;
    delete mNativeHDRCheckbox;
    delete mAlmanacButton;
    delete mRestartButton;
    delete mBackToMainButton;
    delete mBackToGameButton;
    /*delete mGameplayButton;
    delete mControllerButton;
    delete mLanguageButton;*/
}

//0x45C880
int NewOptionsDialog::GetPreferredHeight(int theWidth)
{
    return IMAGE_OPTIONS_MENUBACK->mWidth;
}

//0x45C890
void NewOptionsDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::AddedToManager(theWidgetManager);
    /*AddWidget(mGameplayButton);
    AddWidget(mControllerButton);
    AddWidget(mLanguageButton);*/
    AddWidget(mAlmanacButton);
    AddWidget(mRestartButton);
    AddWidget(mBackToMainButton);
    AddWidget(mMusicVolumeSlider);
    AddWidget(mSfxVolumeSlider);
    AddWidget(mHardwareAccelerationCheckbox);
    AddWidget(mFullscreenCheckbox);
    AddWidget(mNativeHDRCheckbox);
    AddWidget(mBackToGameButton);
}

//0x45C930
void NewOptionsDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::RemovedFromManager(theWidgetManager);
    /*RemoveWidget(mGameplayButton);
    RemoveWidget(mControllerButton);
    RemoveWidget(mLanguageButton);*/
    RemoveWidget(mAlmanacButton);
    RemoveWidget(mRestartButton);
    RemoveWidget(mBackToMainButton);
    RemoveWidget(mMusicVolumeSlider);
    RemoveWidget(mSfxVolumeSlider);
    RemoveWidget(mHardwareAccelerationCheckbox);
    RemoveWidget(mFullscreenCheckbox);
    RemoveWidget(mNativeHDRCheckbox);
    RemoveWidget(mBackToGameButton);
}

//0x45C9D0
void NewOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    Dialog::Resize(theX, theY, theWidth, theHeight);
    mMusicVolumeSlider->Resize(199, 116, 135, 40);
    mSfxVolumeSlider->Resize(199, 143, 135, 40);
    mHardwareAccelerationCheckbox->Resize(283, 175, 46, 45);
    mFullscreenCheckbox->Resize(284, 206, 46, 45);
    mNativeHDRCheckbox->Resize(284, 237, 46, 45);
    mAlmanacButton->Resize(107, 276, 209, 46);
    mRestartButton->Resize(mAlmanacButton->mX, mAlmanacButton->mY + 39, 209, 46);
    mBackToMainButton->Resize(mRestartButton->mX, mRestartButton->mY + 39, 209, 46);
    mBackToGameButton->Resize(30, 395, mBackToGameButton->mWidth, mBackToGameButton->mHeight);

    /*mGameplayButton->Resize(mAlmanacButton->mX, 116, 209, 46);
    mControllerButton->Resize(mAlmanacButton->mX, mGameplayButton->mY + 43, 209, 46);
    mLanguageButton->Resize(mAlmanacButton->mX, mControllerButton->mY + 43, 209, 46);*/

    if (mFromGameSelector)
    {
        mMusicVolumeSlider->mY += 5;
        mSfxVolumeSlider->mY += 10;
        mHardwareAccelerationCheckbox->mY += 15;
        mFullscreenCheckbox->mY += 20;
        mNativeHDRCheckbox->mY += 20;

        /*mGameplayButton->mY += 69;
        mControllerButton->mY += 69;
        mLanguageButton->mY += 69;
        mBackToMainButton->Resize(mAlmanacButton->mX, mLanguageButton->mY + 43, 209, 46);*/
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM || mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ICE)
    {
        mAlmanacButton->mY += 43;
    }
}

//0x45CB50
void NewOptionsDialog::Draw(Sexy::Graphics* g)
{
    g->DrawImage(IMAGE_OPTIONS_MENUBACK, 0, 0);

    int aMusicOffset = 0;
    int aSfxOffset = 0;
    int a3DAccelOffset = 0;
    int aFullScreenOffset = 0;
    int aNativeHDROffset = 0;
    if (mFromGameSelector)
    {
        aMusicOffset = 5;
        aSfxOffset = 10;
        a3DAccelOffset = 15;
        aFullScreenOffset = 20;
        aNativeHDROffset = 20;
    }
    Sexy::Color aTextColor(107, 109, 145);

   /* if (mFromGameSelector)
    {
        SexyString version = _S("Version: ") + StringToSexyString(mApp->mProductVersion);
#ifdef _DEBUG
        version = version + _S(" DEBUG");
#endif
        TodDrawString(g, _S("Plants vs. Zombies"), 210, 130.5f + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_CENTER);
        TodDrawString(g, version, 210, 157.5f + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_CENTER);
    }
    else*/
    {
        TodDrawString(g, TodStringTranslate(_S("[MUSIC_LABEL]")), 186, 140 + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[SOUND_LABEL]")), 186, 167 + aSfxOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("Vertical-Sync")), 274, 197 + a3DAccelOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[FULLSCREEN_LABEL]")), 274, 229 + aFullScreenOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[NATIVE_HDR_LABEL]")), 274, 260 + aNativeHDROffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    }
}

//0x45CF50
void NewOptionsDialog::SliderVal(int theId, double theVal)
{
    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_MusicVolume:
        mApp->SetMusicVolume(theVal);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        /*if (!mMusicVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }*/
        break;

    case NewOptionsDialog::NewOptionsDialog_SoundVolume:
        mApp->SetSfxVolume(theVal * 0.65);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        if (!mSfxVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }
        break;
    }
}

//0x45CFF0
void NewOptionsDialog::CheckboxChecked(int theId, bool checked)
{
    mApp->PlaySample(SOUND_BUTTONCLICK);

    if (theId == NewOptionsDialog::NewOptionsDialog_NativeHDR)
    {
        mApp->mEnableNativeHDR = checked;
        mApp->RegistryWriteBoolean(_S("EnableNativeHDR"), checked);

        if (checked != mApp->mNativeHDRRenderer)
        {
            mApp->DoDialog(
                Dialogs::DIALOG_INFO,
                true,
                _S("[NATIVE_HDR_RESTART_HEADER]"),
                _S("[NATIVE_HDR_RESTART_BODY]"),
                _S("[OK_LABEL]"),
                Dialog::BUTTONS_FOOTER
            );
        }
    }

    /*switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_Fullscreen:
    {
        if (!checked && mApp->mForceFullscreen)
        {
            mApp->DoDialog(
                Dialogs::DIALOG_COLORDEPTH_EXP,
                true,
                _S("[NO_WINDOWED_HEADER]"),
                _S("[NO_WINDOWED_LINES]"),
                _S("[OK_LABEL]"),
                Dialog::BUTTONS_FOOTER
            );

            mFullscreenCheckbox->SetChecked(true, false);
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_HardwareAcceleration:
    {
        if (checked)
        {

            if (!mApp->Is3DAccelerationSupported())
            {

                mHardwareAccelerationCheckbox->SetChecked(false, false);
                mApp->DoDialog(
                    Dialogs::DIALOG_INFO,
                    true,
                    _S("[NOT_SUPPORTED_HEADER]"),
                    _S("[NOT_SUPPORTED_LINES]"),
                    _S("[OK_LABEL]"),
                    Dialog::BUTTONS_FOOTER
                );
            }
            else if (!mApp->Is3DAccelerationRecommended())
            {
                mApp->DoDialog(
                    Dialogs::DIALOG_INFO,
                    true,
                    _S("[WARNING_HEADER]"),
                    _S("[WARNING_LINES]"),
                    _S("[OK_LABEL]"),
                    Dialog::BUTTONS_FOOTER
                );
            }
        }
        break;
    }
    }*/
}

//0x45D290
void NewOptionsDialog::KeyDown(Sexy::KeyCode theKey)
{
    if (mApp->mBoard)
    {
        mApp->mBoard->DoTypingCheck(theKey);
    }

    if (theKey == KeyCode::KEYCODE_SPACE || theKey == KeyCode::KEYCODE_RETURN)
    {
        Dialog::ButtonDepress(Dialog::ID_OK);
    }
    else if (theKey == KeyCode::KEYCODE_ESCAPE)
    {
        Dialog::ButtonDepress(/*Dialog::ID_CANCEL*/Dialog::ID_OK);
    }
}

//0x45D2F0
void NewOptionsDialog::ButtonPress(int theId)
{
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

//0x45D310
void NewOptionsDialog::ButtonDepress(int theId)
{
    Dialog::ButtonDepress(theId);

    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_Almanac:
    {
        AlmanacDialog* aDialog = mApp->DoAlmanacDialog(SeedType::SEED_NONE, ZombieType::ZOMBIE_INVALID);
        aDialog->WaitForResult(true);
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_MainMenu:
    {
        if (mFromGameSelector)
        {
            mApp->KillNewOptionsDialog();
            mApp->KillGameSelector();
            mApp->ShowAwardScreen(AwardType::AWARD_CREDITS_ZOMBIENOTE, false);
        }
        else if (mApp->mBoard && mApp->mBoard->NeedSaveGame())
        {
            mApp->DoConfirmBackToMain();
        }
        else if (mApp->mBoard && mApp->mBoard->mCutScene && mApp->mBoard->mCutScene->IsSurvivalRepick())
        {
            mApp->DoConfirmBackToMain();
        }
        else
        {
            mApp->mBoardResult = BoardResult::BOARDRESULT_QUIT;
            mApp->DoBackToMain();
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Restart:
    {
        if (mApp->mBoard)
        {
            SexyString aDialogTitle;
            SexyString aDialogMessage;
            if (mApp->IsPuzzleMode())
            {
                aDialogTitle = _S("[RESTART_PUZZLE_HEADER]");
                aDialogMessage = _S("[RESTART_PUZZLE_BODY]");
            }
            else if (mApp->IsChallengeMode())
            {
                aDialogTitle = _S("[RESTART_CHALLENGE_HEADER]");
                aDialogMessage = _S("[RESTART_CHALLENGE_BODY]");
            }
            else if (mApp->IsSurvivalMode())
            {
                aDialogTitle = _S("[RESTART_SURVIVAL_HEADER]");
                aDialogMessage = _S("[RESTART_SURVIVAL_BODY]");
            }
            else
            {
                aDialogTitle = _S("[RESTART_LEVEL_HEADER]");
                aDialogMessage = _S("[RESTART_LEVEL_BODY]");
            }

            LawnDialog* aDialog = (LawnDialog*)mApp->DoDialog(Dialogs::DIALOG_CONFIRM_RESTART, true, aDialogTitle, aDialogMessage, _S(""), Dialog::BUTTONS_YES_NO);
            aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[RESTART_LABEL]"));
            aDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_CANCEL]"));
            
            if (aDialog->WaitForResult(true) == Dialog::ID_YES)
            {
                mApp->mMusic->StopAllMusic();
                mApp->mSoundSystem->CancelPausedFoley();
                mApp->KillNewOptionsDialog();
                mApp->mBoardResult = BoardResult::BOARDRESULT_RESTART;
                mApp->mSawYeti = mApp->mBoard->mKilledYeti;
                mApp->PreNewGame(mApp->mGameMode, false, mApp->mBoard->mLevel);
            }
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Update:
        mApp->CheckForUpdates();
        break;

    case NewOptionsDialog::NewOptionsDialog_Language:
    {
        mApp->KillNewOptionsDialog();
        mApp->KillGameSelector();
        mApp->ShowLanagugeScreen();
        break;
    }
    }
}
