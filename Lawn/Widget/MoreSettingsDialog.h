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
		MoreSettingsDialog_RefreshPrevious = 20000,
		MoreSettingsDialog_RefreshNext,
		MoreSettingsDialog_ExclusiveFullscreen,
		MoreSettingsDialog_IntegerScaling,
		MoreSettingsDialog_ShowFPS,
		MoreSettingsDialog_HDRPaperWhite,
		MoreSettingsDialog_RestoreDefaults,
	};

	LawnApp*				mApp;
	Sexy::Slider*			mHDRPaperWhiteSlider;
	Sexy::Checkbox*		mExclusiveFullscreenCheckbox;
	Sexy::Checkbox*		mIntegerScalingCheckbox;
	Sexy::Checkbox*		mShowFPSCheckbox;
	LawnStoneButton*		mRefreshPreviousButton;
	LawnStoneButton*		mRefreshNextButton;
	LawnStoneButton*		mRestoreDefaultsButton;
	std::vector<int>		mAvailableRefreshRatesMilliHz;
	int						mRefreshRateIndex;
	int						mOriginalRefreshRateMilliHz;
	bool					mOriginalExclusiveFullscreen;

	int						GetContentTop() const;
	void					BuildRefreshRateList();
	void					UpdateRefreshControls();
	SexyString				GetRefreshRateLabel() const;
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
