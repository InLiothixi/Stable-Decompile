#ifndef __ZOMBATARTOS_H__
#define __ZOMBATARTOS_H__

#include "LawnDialog.h"
#include "../../SexyAppFramework/CheckboxListener.h"

#ifdef _HAS_ZOMBATAR

class ZombatarTOS : public LawnDialog, public CheckboxListener
{
private:
	enum {
		ZOMBATARTOS_BACK,
		ZOMBATARTOS_AGREEMENT,
		ZOMBATARTOS_ACCEPT
	};

public:
	LawnApp*			mApp;
	NewLawnButton*		mBackButton;
	Checkbox*			mAgreementCheckbox;
	NewLawnButton*		mAcceptButton;
	float				mScrollAmount;
	float				mScrollPosition;
	float				mMaxScrollPosition;
	int					mHintCounter;
	bool				mIsDraggingThumb;
	bool				mScrollableDown;
	bool				mMouseIsDragging;
	bool				mIsSwiping;
	int					mSwipeStartY;
	float				mSwipeVelocityY;
	bool				mSwipeJustReleased;

public:
	ZombatarTOS(LawnApp* theApp);
	virtual ~ZombatarTOS();

	virtual int			GetPreferredHeight(int theWidth);
	virtual void		Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void		AddedToManager(WidgetManager* theWidgetManager);
	virtual void		RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void		Draw(Graphics* g);
	virtual void		CalcSize(int theExtraX, int theExtraY);
	virtual void        MouseWheel(int theDelta);
	virtual void        Update();
	virtual void		SetButtonDelay(int theDelay);
	virtual void        ButtonDepress(int theId);
	virtual void        CheckboxChecked(int theId, bool theCheck);
	virtual void		MouseDown(int x, int y, int theBtnNum, int theClickCount);
	virtual void		MouseUp(int x, int y, int theClickCount);
	virtual void		MouseMove(int x, int y);
};
#endif

#endif