#ifndef __MORESETTINGSDIALOG_H__
#define __MORESETTINGSDIALOG_H__

#include <vector>

#include "../../SexyAppFramework/CheckboxListener.h"
#include "../../SexyAppFramework/SliderListener.h"
#include "LawnDialog.h"

class LawnApp;
class LawnStoneButton;

namespace Sexy
{
	class Checkbox;
	class Slider;
}

class MoreSettingsDialog : public LawnDialog, public Sexy::CheckboxListener, public Sexy::SliderListener
{
private:
	enum
	{
		MoreSettingsDialog_ToneMappingPage = 20000,
		MoreSettingsDialog_DisplayPage,
		MoreSettingsDialog_ScalingPage,
		MoreSettingsDialog_HDRPaperWhite,
		MoreSettingsDialog_HDRExposure,
		MoreSettingsDialog_HDRAdaptiveToneMapping,
		MoreSettingsDialog_RefreshPrevious,
		MoreSettingsDialog_RefreshNext,
		MoreSettingsDialog_ExclusiveFullscreen,
		MoreSettingsDialog_IntegerScaling,
		MoreSettingsDialog_ShowFPS,
		MoreSettingsDialog_FSR1Enabled,
		MoreSettingsDialog_FSR1QualityPrevious,
		MoreSettingsDialog_FSR1QualityNext,
		MoreSettingsDialog_FSR1Sharpness,
		MoreSettingsDialog_RestoreDefaults,
	};
	enum SettingsPage
	{
		SETTINGS_PAGE_TONE_MAPPING,
		SETTINGS_PAGE_DISPLAY,
		SETTINGS_PAGE_SCALING,
	};

	LawnApp*				mApp;
	Sexy::Slider*			mHDRPaperWhiteSlider;
	Sexy::Slider*			mHDRExposureSlider;
	Sexy::Checkbox*		mHDRAdaptiveToneMappingCheckbox;
	Sexy::Checkbox*		mExclusiveFullscreenCheckbox;
	Sexy::Checkbox*		mIntegerScalingCheckbox;
	Sexy::Checkbox*		mShowFPSCheckbox;
	Sexy::Checkbox*		mFSR1EnabledCheckbox;
	Sexy::Slider*			mFSR1SharpnessSlider;
	LawnStoneButton*		mRefreshPreviousButton;
	LawnStoneButton*		mRefreshNextButton;
	LawnStoneButton*		mFSR1QualityPreviousButton;
	LawnStoneButton*		mFSR1QualityNextButton;
	LawnStoneButton*		mToneMappingPageButton;
	LawnStoneButton*		mDisplayPageButton;
	LawnStoneButton*		mScalingPageButton;
	LawnStoneButton*		mRestoreDefaultsButton;
	std::vector<int>		mAvailableRefreshRatesMilliHz;
	int						mRefreshRateIndex;
	int						mUnavailableRefreshRateMilliHz;
	int						mOriginalRefreshRateMilliHz;
	bool					mOriginalExclusiveFullscreen;
	bool					mOriginalFSR1Enabled;
	SettingsPage			mCurrentPage;

	int						GetContentTop() const;
	void					SetPage(SettingsPage thePage);
	void					BuildRefreshRateList();
	void					UpdateRefreshControls();
	SexyString				GetRefreshRateLabel() const;
	void					UpdateFSR1Controls();
	SexyString				GetFSR1QualityLabel() const;
	void					RestoreDefaults();

public:
	MoreSettingsDialog(LawnApp* theApp);
	~MoreSettingsDialog();

	void					AddedToManager(Sexy::WidgetManager* theWidgetManager);
	void					RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
	void					Resize(int theX, int theY, int theWidth, int theHeight);
	void					Draw(Sexy::Graphics* g);
	void					CheckboxChecked(int theId, bool checked);
	void					SliderVal(int theId, double theVal);
	void					ButtonDepress(int theId);
	void					KeyDown(Sexy::KeyCode theKey);
	bool					RequiresDisplayRestart() const;
};

#endif
