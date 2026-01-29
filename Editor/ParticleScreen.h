#ifndef __PARTICLESCREEN_H__
#define __PARTICLESCREEN_H__

#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "../../SexyAppFramework/ScrollListener.h"
#include "../Sexy.TodLib/TodParticle.h"
#include <vector>

using namespace Sexy;

class LawnApp;
class NewLawnButton;

class ParticleScreen : public Widget, public ButtonListener, public ScrollListener
{
public:
	LawnApp* mApp;
	bool mIsSysPaused;
	bool mIsPaused;
	bool mIsImported;
	bool mIsDragging;
	int mStartMouseX;
	int mStartMouseY;
	int mDownX;
	int mDownY;
	int mStartViewX;
	int mStartViewY;
	int mViewX;
	int mViewY;
	TodParticleDefinition* mParticleDef;

public:
	ParticleScreen(LawnApp* theApp);
	~ParticleScreen();

	void ResetParticle();
	void InstantiateParticle();

	virtual void Update();
	virtual void Draw(Graphics* g);
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void MouseUp(int x, int y, int theClickCount);
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void KeyDown(KeyCode theKey);
	virtual void ButtonDepress(int theId);
	void AddedToManager(WidgetManager* theWidgetManager);
	void RemovedFromManager(WidgetManager* theWidgetManager);

	void ImGuiDraw();
	void MenuBar();
	void Debugger();
	void EmitterPannel();

	void PresetDown(TodParticleDefinition* theDef);
};

extern std::vector<TodParticleSystem*> gParticleTests;

#endif