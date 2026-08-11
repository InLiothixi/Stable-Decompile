#include "MoreSettingsDialog.h"

#include <algorithm>
#include <cmath>

#include "../../ConstEnums.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../SexyAppFramework/Font.h"
#include "../../SexyAppFramework/Slider.h"
#include "../LawnCommon.h"
#include "../System/Music.h"
#include "GameButton.h"

using namespace Sexy;

MoreSettingsDialog::MoreSettingsDialog(LawnApp* theApp) :
	LawnDialog(
		theApp,
		Dialogs::DIALOG_MORESETTINGS,
		true,
		_S("[ADVANCED_SETTINGS_HEADER]"),
		_S(""),
		_S("[ADVANCED_SETTINGS_DONE]"),
		Dialog::BUTTONS_FOOTER
	)
{
	mApp = theApp;
	mOriginalRefreshRateMilliHz = mApp->mPreferredRefreshRateMilliHz;
	mOriginalExclusiveFullscreen = mApp->mUseExclusiveFullscreen;
	mRefreshRateIndex = 0;
	mUnavailableRefreshRateMilliHz = 0;
	mCurrentPage = SETTINGS_PAGE_TONE_MAPPING;

	mHDRPaperWhiteSlider = new Slider(
		IMAGE_OPTIONS_SLIDERSLOT,
		IMAGE_OPTIONS_SLIDERKNOB2,
		MoreSettingsDialog_HDRPaperWhite,
		this
	);
	mHDRPaperWhiteSlider->SetValue((std::clamp(mApp->mHDRPaperWhitePercent, 50, 200) - 50) / 150.0);
	mHDRExposureSlider = new Slider(
		IMAGE_OPTIONS_SLIDERSLOT,
		IMAGE_OPTIONS_SLIDERKNOB2,
		MoreSettingsDialog_HDRExposure,
		this
	);
	mHDRExposureSlider->SetValue((std::clamp(mApp->mHDRExposureTenthsEV, -20, 20) + 20) / 40.0);
	mHDRAdaptiveToneMappingCheckbox = MakeNewCheckbox(
		MoreSettingsDialog_HDRAdaptiveToneMapping,
		this,
		mApp->mHDRAdaptiveToneMapping
	);
	mExclusiveFullscreenCheckbox = MakeNewCheckbox(
		MoreSettingsDialog_ExclusiveFullscreen,
		this,
		mApp->mUseExclusiveFullscreen
	);
	mIntegerScalingCheckbox = MakeNewCheckbox(
		MoreSettingsDialog_IntegerScaling,
		this,
		mApp->mUseIntegerScaling
	);
	mShowFPSCheckbox = MakeNewCheckbox(MoreSettingsDialog_ShowFPS, this, mApp->mShowFPS);
	mToneMappingPageButton = MakeButton(
		MoreSettingsDialog_ToneMappingPage,
		this,
		_S("[ADVANCED_TONE_MAPPING_TAB]")
	);
	mDisplayPageButton = MakeButton(
		MoreSettingsDialog_DisplayPage,
		this,
		_S("[ADVANCED_DISPLAY_TAB]")
	);
	mRefreshPreviousButton = MakeButton(MoreSettingsDialog_RefreshPrevious, this, _S("<"));
	mRefreshNextButton = MakeButton(MoreSettingsDialog_RefreshNext, this, _S(">"));
	mRestoreDefaultsButton = MakeButton(
		MoreSettingsDialog_RestoreDefaults,
		this,
		_S("[ADVANCED_RESTORE_DEFAULTS]")
	);

	BuildRefreshRateList();
	UpdateRefreshControls();
	Resize(0, 0, 599, 578);
	LawnApp::CenterDialog(this, mWidth, mHeight);
	SetPage(SETTINGS_PAGE_TONE_MAPPING);
}

MoreSettingsDialog::~MoreSettingsDialog()
{
	delete mHDRPaperWhiteSlider;
	delete mHDRExposureSlider;
	delete mHDRAdaptiveToneMappingCheckbox;
	delete mExclusiveFullscreenCheckbox;
	delete mIntegerScalingCheckbox;
	delete mShowFPSCheckbox;
	delete mRefreshPreviousButton;
	delete mRefreshNextButton;
	delete mToneMappingPageButton;
	delete mDisplayPageButton;
	delete mRestoreDefaultsButton;
}

void MoreSettingsDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mHDRPaperWhiteSlider);
	AddWidget(mHDRExposureSlider);
	AddWidget(mHDRAdaptiveToneMappingCheckbox);
	AddWidget(mExclusiveFullscreenCheckbox);
	AddWidget(mIntegerScalingCheckbox);
	AddWidget(mShowFPSCheckbox);
	AddWidget(mRefreshPreviousButton);
	AddWidget(mRefreshNextButton);
	AddWidget(mToneMappingPageButton);
	AddWidget(mDisplayPageButton);
	AddWidget(mRestoreDefaultsButton);
}

void MoreSettingsDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mHDRPaperWhiteSlider);
	RemoveWidget(mHDRExposureSlider);
	RemoveWidget(mHDRAdaptiveToneMappingCheckbox);
	RemoveWidget(mExclusiveFullscreenCheckbox);
	RemoveWidget(mIntegerScalingCheckbox);
	RemoveWidget(mShowFPSCheckbox);
	RemoveWidget(mRefreshPreviousButton);
	RemoveWidget(mRefreshNextButton);
	RemoveWidget(mToneMappingPageButton);
	RemoveWidget(mDisplayPageButton);
	RemoveWidget(mRestoreDefaultsButton);
}

int MoreSettingsDialog::GetContentTop() const
{
	int aStartY = mContentInsets.mTop + mBackgroundInsets.mTop + DIALOG_HEADER_OFFSET + 10;
	if (!mDialogHeader.empty())
	{
		const int aOffsetY = aStartY - mHeaderFont->GetAscentPadding() + mHeaderFont->GetAscent();
		aStartY = aOffsetY - mHeaderFont->GetAscent() + mHeaderFont->GetHeight() + mSpaceAfterHeader;
	}
	return aStartY;
}

void MoreSettingsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	const int aTop = GetContentTop();

	mToneMappingPageButton->Resize(72, aTop + 2, 218, 42);
	mDisplayPageButton->Resize(309, aTop + 2, 218, 42);

	mHDRPaperWhiteSlider->Resize(280, aTop + 48, 210, 36);
	mHDRExposureSlider->Resize(280, aTop + 105, 210, 36);
	mHDRAdaptiveToneMappingCheckbox->Resize(72, aTop + 157, 46, 45);

	mExclusiveFullscreenCheckbox->Resize(72, aTop + 50, 46, 45);
	mRefreshPreviousButton->Resize(245, aTop + 101, 71, 42);
	mRefreshNextButton->Resize(456, aTop + 101, 71, 42);
	mIntegerScalingCheckbox->Resize(72, aTop + 158, 46, 45);
	mShowFPSCheckbox->Resize(72, aTop + 210, 46, 45);
	mRestoreDefaultsButton->Resize(195, aTop + 300, 209, 42);
}

void MoreSettingsDialog::SetPage(SettingsPage thePage)
{
	mCurrentPage = thePage;
	const bool isToneMappingPage = mCurrentPage == SETTINGS_PAGE_TONE_MAPPING;

	mToneMappingPageButton->mInverted = isToneMappingPage;
	mToneMappingPageButton->SetDisabled(isToneMappingPage);
	mDisplayPageButton->mInverted = !isToneMappingPage;
	mDisplayPageButton->SetDisabled(!isToneMappingPage);

	mHDRPaperWhiteSlider->SetVisible(isToneMappingPage);
	mHDRExposureSlider->SetVisible(isToneMappingPage);
	mHDRAdaptiveToneMappingCheckbox->SetVisible(isToneMappingPage);
	mExclusiveFullscreenCheckbox->SetVisible(!isToneMappingPage);
	mRefreshPreviousButton->SetVisible(!isToneMappingPage);
	mRefreshNextButton->SetVisible(!isToneMappingPage);
	mIntegerScalingCheckbox->SetVisible(!isToneMappingPage);
	mShowFPSCheckbox->SetVisible(!isToneMappingPage);
	MarkDirty();
}

void MoreSettingsDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);
	const int aTop = GetContentTop();
	const Color aTextColor(107, 109, 145);
	const Color aSubtleColor(126, 128, 160);

	if (mCurrentPage == SETTINGS_PAGE_TONE_MAPPING)
	{
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_HDR_PAPER_WHITE]")),
			72,
			aTop + 72,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			StrFormat("%d%%", mApp->mHDRPaperWhitePercent),
			550,
			aTop + 72,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_RIGHT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_HDR_EXPOSURE]")),
			72,
			aTop + 129,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			StrFormat("%+.1f EV", mApp->mHDRExposureTenthsEV / 10.0),
			550,
			aTop + 129,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_RIGHT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_HDR_ADAPTIVE]")),
			114,
			aTop + 181,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);

		SexyString anHDRStatus;
		if (!mApp->mEnableNativeHDR)
			anHDRStatus = TodStringTranslate(_S("[ADVANCED_HDR_DISABLED]"));
		else if (mApp->IsNativeHDRActive())
		{
			const SDL_PropertiesID aProperties = SDL_GetRendererProperties(LawnApp::mSDLRenderer);
			const float aHeadroom = SDL_GetFloatProperty(aProperties, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 1.0f);
			anHDRStatus = StrFormat(
				"%s  %.1fx",
				TodStringTranslate(_S("[ADVANCED_HDR_ACTIVE]")).c_str(),
				aHeadroom
			);
		}
		else if (mApp->mNativeHDRRenderer)
			anHDRStatus = TodStringTranslate(_S("[ADVANCED_WINDOWS_HDR_OFF]"));
		else
			anHDRStatus = TodStringTranslate(_S("[ADVANCED_HDR_RESTART]"));
		const Rect anHDRStatusRect(56, aTop + 207, 487, 0);
		const int anHDRStatusHeight = TodDrawStringWrappedHelper(
			g,
			anHDRStatus,
			anHDRStatusRect,
			FONT_DWARVENTODCRAFT12,
			aSubtleColor,
			DS_ALIGN_LEFT,
			false
		);
		TodDrawStringWrapped(g, anHDRStatus, anHDRStatusRect, FONT_DWARVENTODCRAFT12, aSubtleColor, DS_ALIGN_LEFT);

		const SexyString anAdaptiveNote = TodStringTranslate(_S("[ADVANCED_HDR_ADAPTIVE_NOTE]"));
		TodDrawStringWrapped(
			g,
			anAdaptiveNote,
			Rect(56, anHDRStatusRect.mY + anHDRStatusHeight + 4, 487, 0),
			FONT_DWARVENTODCRAFT12,
			aSubtleColor,
			DS_ALIGN_RIGHT
		);
	}
	else
	{
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_EXCLUSIVE_FULLSCREEN]")),
			114,
			aTop + 74,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_REFRESH_RATE]")),
			72,
			aTop + 126,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawStringWrapped(
			g,
			GetRefreshRateLabel(),
			Rect(321, aTop + 101, 129, 42),
			FONT_DWARVENTODCRAFT12,
			aTextColor,
			DS_ALIGN_CENTER_VERTICAL_MIDDLE
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_INTEGER_SCALING]")),
			114,
			aTop + 182,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_SHOW_FPS]")),
			114,
			aTop + 234,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawStringWrapped(
			g,
			TodStringTranslate(_S("[ADVANCED_DISPLAY_RESTART_NOTE]")),
			Rect(56, aTop + 260, 487, 32),
			FONT_DWARVENTODCRAFT12,
			aSubtleColor,
			DS_ALIGN_RIGHT
		);
	}
}

void MoreSettingsDialog::BuildRefreshRateList()
{
	mAvailableRefreshRatesMilliHz.clear();
	mAvailableRefreshRatesMilliHz.push_back(0);
	mUnavailableRefreshRateMilliHz = 0;

	const SDL_DisplayID aDisplay = LawnApp::mSDLWindow == nullptr
		? 0
		: SDL_GetDisplayForWindow(LawnApp::mSDLWindow);
	const SDL_DisplayMode* aDesktopMode = aDisplay == 0 ? nullptr : SDL_GetDesktopDisplayMode(aDisplay);
	int aModeCount = 0;
	SDL_DisplayMode** aModes = aDisplay == 0 ? nullptr : SDL_GetFullscreenDisplayModes(aDisplay, &aModeCount);
	if (aDesktopMode != nullptr && aModes != nullptr)
	{
		for (int i = 0; i < aModeCount; i++)
		{
			const SDL_DisplayMode* aMode = aModes[i];
			if (aMode == nullptr ||
				aMode->w != aDesktopMode->w ||
				aMode->h != aDesktopMode->h ||
				aMode->format != aDesktopMode->format ||
				std::abs(aMode->pixel_density - aDesktopMode->pixel_density) > 0.01f ||
				aMode->refresh_rate <= 0.0f)
				continue;
			mAvailableRefreshRatesMilliHz.push_back((int)std::lround(aMode->refresh_rate * 1000.0f));
		}
	}
	SDL_free(aModes);

	std::sort(mAvailableRefreshRatesMilliHz.begin(), mAvailableRefreshRatesMilliHz.end());
	mAvailableRefreshRatesMilliHz.erase(
		std::unique(mAvailableRefreshRatesMilliHz.begin(), mAvailableRefreshRatesMilliHz.end()),
		mAvailableRefreshRatesMilliHz.end()
	);

	auto aCurrentRate = std::find_if(
		mAvailableRefreshRatesMilliHz.begin(),
		mAvailableRefreshRatesMilliHz.end(),
		[this](int theRateMilliHz)
		{
			return std::abs(theRateMilliHz - mApp->mPreferredRefreshRateMilliHz) <= 100;
		}
	);
	if (aCurrentRate == mAvailableRefreshRatesMilliHz.end())
	{
		mUnavailableRefreshRateMilliHz = mApp->mPreferredRefreshRateMilliHz;
		aCurrentRate = mAvailableRefreshRatesMilliHz.begin();
	}
	mRefreshRateIndex = aCurrentRate == mAvailableRefreshRatesMilliHz.end()
		? 0
		: (int)std::distance(mAvailableRefreshRatesMilliHz.begin(), aCurrentRate);
}

void MoreSettingsDialog::UpdateRefreshControls()
{
	const bool canSelectRefreshRate = mApp->mUseExclusiveFullscreen && mAvailableRefreshRatesMilliHz.size() > 1;
	mRefreshPreviousButton->SetDisabled(!canSelectRefreshRate || mRefreshRateIndex <= 0);
	mRefreshNextButton->SetDisabled(
		!canSelectRefreshRate || mRefreshRateIndex >= (int)mAvailableRefreshRatesMilliHz.size() - 1
	);
	MarkDirty();
}

SexyString MoreSettingsDialog::GetRefreshRateLabel() const
{
	if (!mApp->mUseExclusiveFullscreen)
		return TodStringTranslate(_S("[ADVANCED_DESKTOP_CONTROLLED]"));
	if (mUnavailableRefreshRateMilliHz > 0)
	{
		return StrFormat(
			"%.2f Hz - %s",
			mUnavailableRefreshRateMilliHz / 1000.0,
			TodStringTranslate(_S("[ADVANCED_REFRESH_UNAVAILABLE]")).c_str()
		);
	}
	if (mAvailableRefreshRatesMilliHz.empty() || mRefreshRateIndex < 0 ||
		mRefreshRateIndex >= (int)mAvailableRefreshRatesMilliHz.size() ||
		mAvailableRefreshRatesMilliHz[mRefreshRateIndex] == 0)
	{
		return TodStringTranslate(_S("[ADVANCED_DESKTOP_RATE]"));
	}

	const int aRateMilliHz = mAvailableRefreshRatesMilliHz[mRefreshRateIndex];
	if (aRateMilliHz % 1000 == 0)
		return StrFormat("%d Hz", aRateMilliHz / 1000);
	return StrFormat("%.2f Hz", aRateMilliHz / 1000.0);
}

void MoreSettingsDialog::CheckboxChecked(int theId, bool checked)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
	switch (theId)
	{
	case MoreSettingsDialog_HDRAdaptiveToneMapping:
		mApp->mHDRAdaptiveToneMapping = checked;
		mApp->mHDRToneMapUnavailable = false;
		if (!checked)
			mApp->DestroyHDRToneMapTexture();
		MarkDirty();
		break;
	case MoreSettingsDialog_ExclusiveFullscreen:
		mApp->mUseExclusiveFullscreen = checked;
		UpdateRefreshControls();
		break;
	case MoreSettingsDialog_IntegerScaling:
		mApp->mUseIntegerScaling = checked;
		mApp->ApplyLogicalPresentationMode();
		break;
	case MoreSettingsDialog_ShowFPS:
		mApp->mShowFPSMode = FPS_ShowFPS;
		mApp->SetShowFPS(checked);
		break;
	}
}

void MoreSettingsDialog::SliderVal(int theId, double theVal)
{
	switch (theId)
	{
	case MoreSettingsDialog_HDRPaperWhite:
	{
		const int aPercent = std::clamp(50 + (int)std::lround(theVal * 30.0) * 5, 50, 200);
		mApp->mHDRPaperWhitePercent = aPercent;
		mApp->mHDRToneMapUnavailable = false;
		mHDRPaperWhiteSlider->SetValue((aPercent - 50) / 150.0);
		break;
	}
	case MoreSettingsDialog_HDRExposure:
	{
		const int anExposureTenthsEV = std::clamp(-20 + (int)std::lround(theVal * 40.0), -20, 20);
		mApp->mHDRExposureTenthsEV = anExposureTenthsEV;
		mApp->mHDRToneMapUnavailable = false;
		mHDRExposureSlider->SetValue((anExposureTenthsEV + 20) / 40.0);
		break;
	}
	default:
		return;
	}
	MarkDirty();
}

void MoreSettingsDialog::RestoreDefaults()
{
	mApp->mHDRPaperWhitePercent = 100;
	mApp->mHDRExposureTenthsEV = 0;
	mApp->mHDRAdaptiveToneMapping = false;
	mApp->mHDRToneMapUnavailable = false;
	mApp->DestroyHDRToneMapTexture();
	mApp->mPreferredRefreshRateMilliHz = 0;
	mApp->mUseExclusiveFullscreen = false;
	mApp->mUseIntegerScaling = false;
	mApp->mShowFPSMode = FPS_ShowFPS;
	mApp->SetShowFPS(false);

	mHDRPaperWhiteSlider->SetValue(1.0 / 3.0);
	mHDRExposureSlider->SetValue(0.5);
	mHDRAdaptiveToneMappingCheckbox->SetChecked(false, false);
	mExclusiveFullscreenCheckbox->SetChecked(false, false);
	mIntegerScalingCheckbox->SetChecked(false, false);
	mShowFPSCheckbox->SetChecked(false, false);
	BuildRefreshRateList();
	UpdateRefreshControls();
	mApp->ApplyLogicalPresentationMode();
}

void MoreSettingsDialog::ButtonDepress(int theId)
{
	LawnDialog::ButtonDepress(theId);
	switch (theId)
	{
	case MoreSettingsDialog_ToneMappingPage:
		SetPage(SETTINGS_PAGE_TONE_MAPPING);
		break;
	case MoreSettingsDialog_DisplayPage:
		SetPage(SETTINGS_PAGE_DISPLAY);
		break;
	case MoreSettingsDialog_RefreshPrevious:
		if (mRefreshRateIndex > 0)
		{
			mRefreshRateIndex--;
			mUnavailableRefreshRateMilliHz = 0;
			mApp->mPreferredRefreshRateMilliHz = mAvailableRefreshRatesMilliHz[mRefreshRateIndex];
			UpdateRefreshControls();
		}
		break;
	case MoreSettingsDialog_RefreshNext:
		if (mRefreshRateIndex + 1 < (int)mAvailableRefreshRatesMilliHz.size())
		{
			mRefreshRateIndex++;
			mUnavailableRefreshRateMilliHz = 0;
			mApp->mPreferredRefreshRateMilliHz = mAvailableRefreshRatesMilliHz[mRefreshRateIndex];
			UpdateRefreshControls();
		}
		break;
	case MoreSettingsDialog_RestoreDefaults:
		RestoreDefaults();
		break;
	}
}

void MoreSettingsDialog::KeyDown(KeyCode theKey)
{
	if (theKey == KEYCODE_LEFT && mCurrentPage != SETTINGS_PAGE_TONE_MAPPING)
	{
		mApp->PlaySample(SOUND_GRAVEBUTTON);
		SetPage(SETTINGS_PAGE_TONE_MAPPING);
	}
	else if (theKey == KEYCODE_RIGHT && mCurrentPage != SETTINGS_PAGE_DISPLAY)
	{
		mApp->PlaySample(SOUND_GRAVEBUTTON);
		SetPage(SETTINGS_PAGE_DISPLAY);
	}
	else if (theKey == KEYCODE_ESCAPE)
		Dialog::ButtonDepress(Dialog::ID_OK);
	else
		LawnDialog::KeyDown(theKey);
}

bool MoreSettingsDialog::RequiresDisplayRestart() const
{
	return mOriginalExclusiveFullscreen != mApp->mUseExclusiveFullscreen ||
		((mOriginalExclusiveFullscreen || mApp->mUseExclusiveFullscreen) &&
		 mOriginalRefreshRateMilliHz != mApp->mPreferredRefreshRateMilliHz);
}
