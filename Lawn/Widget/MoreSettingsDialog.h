#ifndef __MORESETTINGSDIALOG_H__
#define __MORESETTINGSDIALOG_H__

#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "../../SexyAppFramework/CheckboxListener.h"
#include "LawnDialog.h"

class LawnApp;
class LawnStoneButton;
namespace Sexy
{
	class Slider;
	class Checkbox;
};

class MoreSettingsDialog : public LawnDialog, public Sexy::CheckboxListener 
{
private:
	enum {
		MoreSettingsDialog_Page1 = 20000,
		MoreSettingsDialog_Page2,
        MoreSettingsDialog_HardwareAcceleration,
		MoreSettingsDialog_FPS,
		MoreSettingsDialog_CustomCursor,
		MoreSettingsDialog_AutoPause,
		MoreSettingsDialog_OptimizedGameplay,
		MoreSettingsDialog_NoToolTip,
	}; 

	enum MoreSettingsPages {
		MoreSettingsPage_1,
		MoreSettingsPage_2,
		NUM_OF_PAGES
	};
public:
	LawnApp*			mApp;
    MoreSettingsPages	mCurPage;
	LawnStoneButton*	mPage1;
	LawnStoneButton*	mPage2;
	// PP1
	Checkbox*			mHardwareAcceleration;
	Checkbox*			mCustomCursor;
	Checkbox*			mFPSToggle;
	Checkbox*			mAutoPause;
	Checkbox*			mShowToolTip;

public:
	MoreSettingsDialog(LawnApp* theApp);
	~MoreSettingsDialog();
	void				AddedToManager(Sexy::WidgetManager* theWidgetManager);
	void				RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
	void				Resize(int theX, int theY, int theWidth, int theHeight);
	void				Draw(Sexy::Graphics* g);
	void				CheckboxChecked(int theId, bool checked);
	void				ButtonDepress(int theId);
	void				ChangePage(MoreSettingsPages thePage);
	void				Update();
};
#endif