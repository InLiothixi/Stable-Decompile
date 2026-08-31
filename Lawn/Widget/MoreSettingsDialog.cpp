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
	mOriginalFSR1Enabled = mApp->mEnableFSR1;
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
	mFSR1EnabledCheckbox = MakeNewCheckbox(
		MoreSettingsDialog_FSR1Enabled,
		this,
		mApp->mEnableFSR1
	);
	mFSR1SharpnessSlider = new Slider(
		IMAGE_OPTIONS_SLIDERSLOT,
		IMAGE_OPTIONS_SLIDERKNOB2,
		MoreSettingsDialog_FSR1Sharpness,
		this
	);
	mFSR1SharpnessSlider->SetValue(std::clamp(mApp->mFSR1SharpnessPercent, 0, 100) / 100.0);
	mToneMappingPageButton = MakeButton(
		MoreSettingsDialog_ToneMappingPage,
		this,
		_S("[ADVANCED_HDR_TAB]")
	);
	mDisplayPageButton = MakeButton(
		MoreSettingsDialog_DisplayPage,
		this,
		_S("[ADVANCED_DISPLAY_TAB]")
	);
	mScalingPageButton = MakeButton(
		MoreSettingsDialog_ScalingPage,
		this,
		_S("[ADVANCED_SCALING_TAB]")
	);
	mRefreshPreviousButton = MakeButton(MoreSettingsDialog_RefreshPrevious, this, _S("<"));
	mRefreshNextButton = MakeButton(MoreSettingsDialog_RefreshNext, this, _S(">"));
	mFSR1QualityPreviousButton = MakeButton(MoreSettingsDialog_FSR1QualityPrevious, this, _S("<"));
	mFSR1QualityNextButton = MakeButton(MoreSettingsDialog_FSR1QualityNext, this, _S(">"));
	mRestoreDefaultsButton = MakeButton(
		MoreSettingsDialog_RestoreDefaults,
		this,
		_S("[ADVANCED_RESTORE_DEFAULTS]")
	);

	BuildRefreshRateList();
	UpdateRefreshControls();
	UpdateFSR1Controls();
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
	delete mFSR1EnabledCheckbox;
	delete mFSR1SharpnessSlider;
	delete mRefreshPreviousButton;
	delete mRefreshNextButton;
	delete mFSR1QualityPreviousButton;
	delete mFSR1QualityNextButton;
	delete mToneMappingPageButton;
	delete mDisplayPageButton;
	delete mScalingPageButton;
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
	AddWidget(mFSR1EnabledCheckbox);
	AddWidget(mFSR1SharpnessSlider);
	AddWidget(mRefreshPreviousButton);
	AddWidget(mRefreshNextButton);
	AddWidget(mFSR1QualityPreviousButton);
	AddWidget(mFSR1QualityNextButton);
	AddWidget(mToneMappingPageButton);
	AddWidget(mDisplayPageButton);
	AddWidget(mScalingPageButton);
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
	RemoveWidget(mFSR1EnabledCheckbox);
	RemoveWidget(mFSR1SharpnessSlider);
	RemoveWidget(mRefreshPreviousButton);
	RemoveWidget(mRefreshNextButton);
	RemoveWidget(mFSR1QualityPreviousButton);
	RemoveWidget(mFSR1QualityNextButton);
	RemoveWidget(mToneMappingPageButton);
	RemoveWidget(mDisplayPageButton);
	RemoveWidget(mScalingPageButton);
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

	mToneMappingPageButton->Resize(56, aTop + 2, 157, 42);
	mDisplayPageButton->Resize(221, aTop + 2, 157, 42);
	mScalingPageButton->Resize(386, aTop + 2, 157, 42);

	mHDRPaperWhiteSlider->Resize(280, aTop + 48, 210, 36);
	mHDRExposureSlider->Resize(280, aTop + 105, 210, 36);
	mHDRAdaptiveToneMappingCheckbox->Resize(72, aTop + 157, 46, 45);

	mExclusiveFullscreenCheckbox->Resize(72, aTop + 50, 46, 45);
	mRefreshPreviousButton->Resize(245, aTop + 101, 71, 42);
	mRefreshNextButton->Resize(456, aTop + 101, 71, 42);
	mShowFPSCheckbox->Resize(72, aTop + 158, 46, 45);

	mFSR1EnabledCheckbox->Resize(72, aTop + 50, 46, 45);
	mFSR1QualityPreviousButton->Resize(245, aTop + 101, 71, 42);
	mFSR1QualityNextButton->Resize(456, aTop + 101, 71, 42);
	mFSR1SharpnessSlider->Resize(280, aTop + 158, 210, 36);
	mIntegerScalingCheckbox->Resize(72, aTop + 210, 46, 45);
	mRestoreDefaultsButton->Resize(195, aTop + 300, 209, 42);
}

void MoreSettingsDialog::SetPage(SettingsPage thePage)
{
	mCurrentPage = thePage;
	const bool isToneMappingPage = mCurrentPage == SETTINGS_PAGE_TONE_MAPPING;
	const bool isDisplayPage = mCurrentPage == SETTINGS_PAGE_DISPLAY;
	const bool isScalingPage = mCurrentPage == SETTINGS_PAGE_SCALING;

	mToneMappingPageButton->mInverted = isToneMappingPage;
	mToneMappingPageButton->SetDisabled(isToneMappingPage);
	mDisplayPageButton->mInverted = isDisplayPage;
	mDisplayPageButton->SetDisabled(isDisplayPage);
	mScalingPageButton->mInverted = isScalingPage;
	mScalingPageButton->SetDisabled(isScalingPage);

	mHDRPaperWhiteSlider->SetVisible(isToneMappingPage);
	mHDRExposureSlider->SetVisible(isToneMappingPage);
	mHDRAdaptiveToneMappingCheckbox->SetVisible(isToneMappingPage);
	mExclusiveFullscreenCheckbox->SetVisible(isDisplayPage);
	mRefreshPreviousButton->SetVisible(isDisplayPage);
	mRefreshNextButton->SetVisible(isDisplayPage);
	mShowFPSCheckbox->SetVisible(isDisplayPage);
	mFSR1EnabledCheckbox->SetVisible(isScalingPage);
	mFSR1QualityPreviousButton->SetVisible(isScalingPage);
	mFSR1QualityNextButton->SetVisible(isScalingPage);
	mFSR1SharpnessSlider->SetVisible(isScalingPage);
	mIntegerScalingCheckbox->SetVisible(isScalingPage);
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
	else if (mCurrentPage == SETTINGS_PAGE_DISPLAY)
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
			TodStringTranslate(_S("[ADVANCED_SHOW_FPS]")),
			114,
			aTop + 182,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawStringWrapped(
			g,
			TodStringTranslate(_S("[ADVANCED_DISPLAY_RESTART_NOTE]")),
			Rect(56, aTop + 210, 487, 32),
			FONT_DWARVENTODCRAFT12,
			aSubtleColor,
			DS_ALIGN_RIGHT
		);
	}
	else
	{
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_FSR1]")),
			114,
			aTop + 74,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_FSR1_QUALITY]")),
			72,
			aTop + 126,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawStringWrapped(
			g,
			GetFSR1QualityLabel(),
			Rect(321, aTop + 101, 129, 42),
			FONT_DWARVENTODCRAFT12,
			aTextColor,
			DS_ALIGN_CENTER_VERTICAL_MIDDLE
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_FSR1_SHARPNESS]")),
			72,
			aTop + 182,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_LEFT
		);
		TodDrawString(
			g,
			StrFormat("%d%%", mApp->mFSR1SharpnessPercent),
			550,
			aTop + 182,
			FONT_DWARVENTODCRAFT15,
			aTextColor,
			DS_ALIGN_RIGHT
		);
		TodDrawString(
			g,
			TodStringTranslate(_S("[ADVANCED_INTEGER_SCALING]")),
			114,
			aTop + 234,
			FONT_DWARVENTODCRAFT18,
			aTextColor,
			DS_ALIGN_LEFT
		);
		const SexyString anFSR1Note = mApp->mEnableFSR1 && mApp->mFSR1Unavailable
			? TodStringTranslate(_S("[ADVANCED_FSR1_UNAVAILABLE]"))
			: TodStringTranslate(_S("[ADVANCED_FSR1_NOTE]"));
		TodDrawStringWrapped(
			g,
			anFSR1Note,
			Rect(56, aTop + 260, 487, 0),
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

void MoreSettingsDialog::UpdateFSR1Controls()
{
	mApp->mFSR1Quality = std::clamp(
		mApp->mFSR1Quality,
		(int)FSR1_QUALITY_ULTRA_QUALITY,
		(int)FSR1_QUALITY_PERFORMANCE
	);
	// These values can be prepared before enabling FSR, matching the HDR page.
	mFSR1QualityPreviousButton->SetDisabled(mApp->mFSR1Quality <= FSR1_QUALITY_ULTRA_QUALITY);
	mFSR1QualityNextButton->SetDisabled(mApp->mFSR1Quality >= FSR1_QUALITY_PERFORMANCE);
	mFSR1SharpnessSlider->SetDisabled(false);
	MarkDirty();
}

SexyString MoreSettingsDialog::GetFSR1QualityLabel() const
{
	switch (mApp->mFSR1Quality)
	{
	case FSR1_QUALITY_ULTRA_QUALITY:
		return TodStringTranslate(_S("[ADVANCED_FSR1_ULTRA_QUALITY]"));
	case FSR1_QUALITY_BALANCED:
		return TodStringTranslate(_S("[ADVANCED_FSR1_BALANCED]"));
	case FSR1_QUALITY_PERFORMANCE:
		return TodStringTranslate(_S("[ADVANCED_FSR1_PERFORMANCE]"));
	case FSR1_QUALITY_QUALITY:
	default:
		return TodStringTranslate(_S("[ADVANCED_FSR1_QUALITY_PRESET]"));
	}
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
		if (checked && mApp->mEnableFSR1)
		{
			mApp->mEnableFSR1 = false;
			mFSR1EnabledCheckbox->SetChecked(false, false);
		}
		mApp->ApplyLogicalPresentationMode();
		UpdateFSR1Controls();
		MarkDirty();
		break;
	case MoreSettingsDialog_ShowFPS:
		mApp->mShowFPSMode = FPS_ShowFPS;
		mApp->SetShowFPS(checked);
		break;
	case MoreSettingsDialog_FSR1Enabled:
		mApp->mEnableFSR1 = checked;
		// An explicit off/on cycle retries a transient backend failure once.
		// Unsupported renderers have no backend and remain visibly unavailable.
		if (checked && mApp->mFSR1Backend != nullptr)
			mApp->mFSR1Unavailable = false;
		if (checked && mApp->mUseIntegerScaling)
		{
			mApp->mUseIntegerScaling = false;
			mIntegerScalingCheckbox->SetChecked(false, false);
			mApp->ApplyLogicalPresentationMode();
		}
		mApp->InvalidateFSR1Presentation();
		UpdateFSR1Controls();
		MarkDirty();
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
	case MoreSettingsDialog_FSR1Sharpness:
	{
		const int aSharpnessPercent = std::clamp((int)std::lround(theVal * 20.0) * 5, 0, 100);
		mApp->mFSR1SharpnessPercent = aSharpnessPercent;
		mFSR1SharpnessSlider->SetValue(aSharpnessPercent / 100.0);
		mApp->RequestFSR1Redraw();
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
	mApp->mEnableFSR1 = false;
	mApp->mFSR1Quality = FSR1_QUALITY_QUALITY;
	mApp->mFSR1SharpnessPercent = 20;
	mApp->mShowFPSMode = FPS_ShowFPS;
	mApp->SetShowFPS(false);

	mHDRPaperWhiteSlider->SetValue(1.0 / 3.0);
	mHDRExposureSlider->SetValue(0.5);
	mHDRAdaptiveToneMappingCheckbox->SetChecked(false, false);
	mExclusiveFullscreenCheckbox->SetChecked(false, false);
	mIntegerScalingCheckbox->SetChecked(false, false);
	mFSR1EnabledCheckbox->SetChecked(false, false);
	mFSR1SharpnessSlider->SetValue(0.2);
	mShowFPSCheckbox->SetChecked(false, false);
	BuildRefreshRateList();
	UpdateRefreshControls();
	UpdateFSR1Controls();
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
	case MoreSettingsDialog_ScalingPage:
		SetPage(SETTINGS_PAGE_SCALING);
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
	case MoreSettingsDialog_FSR1QualityPrevious:
		if (mApp->mFSR1Quality > FSR1_QUALITY_ULTRA_QUALITY)
		{
			mApp->mFSR1Quality--;
			mApp->InvalidateFSR1Presentation();
			UpdateFSR1Controls();
		}
		break;
	case MoreSettingsDialog_FSR1QualityNext:
		if (mApp->mFSR1Quality < FSR1_QUALITY_PERFORMANCE)
		{
			mApp->mFSR1Quality++;
			mApp->InvalidateFSR1Presentation();
			UpdateFSR1Controls();
		}
		break;
	case MoreSettingsDialog_RestoreDefaults:
		RestoreDefaults();
		break;
	}
}

void MoreSettingsDialog::KeyDown(KeyCode theKey)
{
	if (theKey == KEYCODE_LEFT && mCurrentPage > SETTINGS_PAGE_TONE_MAPPING)
	{
		mApp->PlaySample(SOUND_GRAVEBUTTON);
		SetPage((SettingsPage)(mCurrentPage - 1));
	}
	else if (theKey == KEYCODE_RIGHT && mCurrentPage < SETTINGS_PAGE_SCALING)
	{
		mApp->PlaySample(SOUND_GRAVEBUTTON);
		SetPage((SettingsPage)(mCurrentPage + 1));
	}
	else if (theKey == KEYCODE_ESCAPE)
		Dialog::ButtonDepress(Dialog::ID_OK);
	else
		LawnDialog::KeyDown(theKey);
}

bool MoreSettingsDialog::RequiresDisplayRestart() const
{
	return mOriginalFSR1Enabled != mApp->mEnableFSR1 ||
		mOriginalExclusiveFullscreen != mApp->mUseExclusiveFullscreen ||
		((mOriginalExclusiveFullscreen || mApp->mUseExclusiveFullscreen) &&
		 mOriginalRefreshRateMilliHz != mApp->mPreferredRefreshRateMilliHz);
}
