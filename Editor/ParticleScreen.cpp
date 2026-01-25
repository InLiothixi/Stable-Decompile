#include "ParticleScreen.h"
#include "../Lawn/Widget/GameButton.h"
#include "../LawnApp.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../Resources.h"
#include "../SexyAppFramework/ImageFont.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/SysFont.h"
#include "../Sexy.TodLib/TodParticle.h"
#include "../Sexy.TodLib/Definition.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/EffectSystem.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

extern "C" {
#include <tinyfiledialogs.h>
}

std::vector<TodParticleSystem*> gParticleTests;

ParticleScreen::ParticleScreen(LawnApp* theApp) {
	mApp = theApp;
	mWidth = mApp->mWidth;
	mHeight = mApp->mHeight;
	mIsImported = false;
	mParticleDef = new TodParticleDefinition();
	memcpy(mParticleDef, &gParticleDefArray[Rand() % ParticleEffect::NUM_PARTICLES], sizeof(TodParticleDefinition));
	InstantiateParticle();
	mIsDragging = false;
	mDownX = 0;
	mDownY = 0;
	mViewX = 0;
	mViewY = 0;
	mStartViewX = 0;
	mStartViewY = 0;
	mIsPaused = false;
	mIsSysPaused = false;
}

ParticleScreen::~ParticleScreen() {
	gParticleTests.clear();
}

void ParticleScreen::ResetParticle()
{
	for (TodParticleSystem* aParticle : gParticleTests) {
		aParticle->ParticleSystemDie();
	}
	gParticleTests.clear();
	InstantiateParticle();
}

void ParticleScreen::InstantiateParticle()
{
	if (gParticleTests.size() >= 2) return;

	TodParticleSystem* aParticle = mApp->mEffectSystem->mParticleHolder->mParticleSystems.DataArrayAlloc();
	aParticle->mParticleHolder = mApp->mEffectSystem->mParticleHolder;
	aParticle->TodParticleInitializeFromDef(mWidth / 2, mHeight / 2, 0, mParticleDef, ParticleEffect::PARTICLE_MELONSPLASH);

	for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext) {
		TodParticleEmitter* emitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
		uint tempFlags = static_cast<uint>(emitter->mEmitterDef->mParticleFlags);
		SetBit(tempFlags, (unsigned int)ParticleFlags::PARTICLE_SOFTWARE_ONLY, false);
		SetBit(tempFlags, (unsigned int)ParticleFlags::PARTICLE_HARDWARE_ONLY, false);
		emitter->mEmitterDef->mParticleFlags = static_cast<int>(tempFlags);
	}

	gParticleTests.push_back(aParticle);
}

void ParticleScreen::Resize(int theX, int theY, int theWidth, int theHeight) {
	Widget::Resize(theX, theY, theWidth, theHeight);
}

void ParticleScreen::Update() {
	Widget::Update();
	MarkDirty();

	int aNewParticles = 0;

	for (TodParticleSystem* aParticle : gParticleTests) {
		aParticle->mDontUpdate = mIsPaused || mIsSysPaused;
		aParticle->Update();

		if (aParticle->mDead)
		{
			aNewParticles++;
			auto it = std::find(gParticleTests.begin(), gParticleTests.end(), aParticle);
			if (it != gParticleTests.end())
				gParticleTests.erase(it);
		}
	}

	for (int i = 0; i < aNewParticles; i++) InstantiateParticle();

	ImGuiIO& io = ImGui::GetIO();
	bool imguiWantsMouse = io.WantCaptureMouse;

	if (mIsDown && mWidgetManager->mLastMouseX < mWidth)
	{
		int dx = mWidgetManager->mLastMouseX - mStartMouseX;
		int dy = mWidgetManager->mLastMouseY - mStartMouseY;
		int dragDistanceSquared = dx * dx + dy * dy;

		if (!mIsDragging && dragDistanceSquared > 16) 
		{
			mIsDragging = true;
			if (!imguiWantsMouse) mApp->SetCursor(CURSOR_DRAGGING);
			mDownX = mWidgetManager->mLastMouseX;
			mDownY = mWidgetManager->mLastMouseY;
			mStartViewX = mViewX;
			mStartViewY = mViewY;
		}

		if (mIsDragging)
		{
			mViewX = mStartViewX + (mWidgetManager->mLastMouseX - mDownX);
			mViewY = mStartViewY + (mWidgetManager->mLastMouseY - mDownY);

			mViewX = max(-mWidth + mWidth / 2, min(mViewX, mWidth / 2));
			mViewY = max(-mWidth + mHeight, min(mViewY, mWidth - mHeight));
		}
	}
	else if (mIsDragging)
	{
		mIsDragging = false;
		if (!imguiWantsMouse) mApp->SetCursor(CURSOR_POINTER);
	}
}

void ParticleScreen::MouseUp(int x, int y, int theClickCount)
{
	Widget::MouseUp(x, y, theClickCount);
	
	mIsDragging = false;

	ImGuiIO& io = ImGui::GetIO();
	bool imguiWantsMouse = io.WantCaptureMouse;

	if (!imguiWantsMouse) mApp->SetCursor(CURSOR_POINTER);
}

void ParticleScreen::MouseDown(int x, int y, int theClickCount)
{
	Widget::MouseDown(x, y, theClickCount);
	mStartMouseX = x;
	mStartMouseY = y;
}

void ParticleScreen::Draw(Graphics* g) {
	g->PushState();
	g->SetColor(Color::Black);
	g->FillRect(0, 0, mWidth, mHeight);
	int screenZeroX = mViewX + mWidth / 2;
	int screenZeroY = mViewY + mHeight / 2;
	int aExtraHeight = mWidth - mHeight;
	for (int y = 0; y < (aExtraHeight * 2 + mHeight) / 50 + 1; y++)
	{
		g->SetColor(Color(32, 32, 32));
		int theY = y * 50 + mViewY - aExtraHeight;
		g->DrawLine(0, theY, mWidth, theY);
	}
	for (int x = 0; x < (mWidth * 2) / 50 + 1; x++)
	{
		g->SetColor(Color(32, 32, 32));
		int theX = x * 50 - mWidth / 2 + mViewX ;
		g->DrawLine(theX, 0, theX, mHeight);
	}
	g->SetColor(Color(255, 0, 0));
	g->DrawLine(0, screenZeroY, mWidth, screenZeroY);
	g->SetColor(Color(0, 255, 0));
	g->DrawLine(screenZeroX, 0, screenZeroX, mHeight);
	g->mTransX += mViewX;
	g->mTransY += mViewY;

	TodParticleSystem* aParticle;
	if (aParticle = gParticleTests[0])
	{
		for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext)
		{
			TodParticleEmitter* emitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
			TodEmitterDefinition* def = emitter->mEmitterDef;

			if (def->mEmitterType == EmitterType::EMITTER_CIRCLE ||
				def->mEmitterType == EmitterType::EMITTER_CIRCLE_PATH ||
				def->mEmitterType == EmitterType::EMITTER_CIRCLE_EVEN_SPACING)
			{
				g->SetColor(Color(255, 255, 255));
				float aRadius1 = FloatTrackEvaluate(def->mEmitterRadius, emitter->mSystemTimeValue, 1);
				g->DrawCircle(mWidth / 2, mHeight / 2, aRadius1, (int)(aRadius1 / PI) * 180);
			}
			else if (def->mEmitterType == EmitterType::EMITTER_BOX || def->mEmitterType == EmitterType::EMITTER_BOX_PATH)
			{
				g->SetColor(Color(255, 255, 255));
				g->DrawRect(FloatTrackEvaluate(def->mEmitterBoxX, emitter->mSystemTimeValue, 0) + mWidth / 2, FloatTrackEvaluate(def->mEmitterBoxY, emitter->mSystemTimeValue, 0) + mHeight / 2,
					-FloatTrackEvaluate(def->mEmitterBoxX, emitter->mSystemTimeValue, 0) + FloatTrackEvaluate(def->mEmitterBoxX, emitter->mSystemTimeValue, 1),
					-FloatTrackEvaluate(def->mEmitterBoxY, emitter->mSystemTimeValue, 0) + FloatTrackEvaluate(def->mEmitterBoxY, emitter->mSystemTimeValue, 1));
			}

			for (int i = 0; i < emitter->mEmitterDef->mParticleFieldCount; i++)
			{
				ParticleField* aParticleField = &emitter->mEmitterDef->mParticleFields[i];
				if (aParticleField->mFieldType == ParticleFieldType::FIELD_GROUND_CONSTRAINT) {
					float theY = FloatTrackEvaluate(aParticleField->mY, 0, i) + mHeight / 2;
					g->SetColor(Color(0, 0, 255));
					g->DrawLine(-mViewX, theY, -mViewX + mWidth, theY);
				}
			}
		}
		aParticle->Draw(g);
	}
	
	g->PopState();
}

void ParticleScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
}

//0x42F6B0
void ParticleScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
}

void ParticleScreen::ButtonDepress(int theId)
{

}

void ParticleScreen::KeyDown(KeyCode theKey)
{
	Widget::KeyDown(theKey);

	if (theKey == KeyCode::KEYCODE_SPACE) mIsPaused = !mIsPaused;
}

void ParticleScreen::PresetDown(TodParticleDefinition* theDef) {
	memcpy(mParticleDef, theDef, sizeof(TodParticleDefinition));
	ResetParticle();
}

bool openReloadPopup = false;
bool showDebugger = false;

void ParticleScreen::MenuBar() {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New File")) {
				// func
			}

			if (ImGui::BeginMenu("Open Existing"))
			{
				ImGui::TextDisabled("Presets");
				ImGui::Separator();

				if (ImGui::BeginChild("PresetScroll", ImVec2(0.0f, ImGui::GetFrameHeight() * min(gParticleParamArraySize, 7)), false, ImGuiWindowFlags_None))
				{
					for (int i = 0; i < gParticleParamArraySize; i++) {
						ParticleParams* aParam = &gParticleParamArray[i];
						std::string name = aParam->mParticleFileName;
						const std::string prefix = "particles\\";
						if (name.rfind(prefix, 0) == 0) {
							name.erase(0, prefix.length());
						}
						const std::string suffix = ".xml";
						if (name.size() >= suffix.size() &&
							name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
							name.erase(name.size() - suffix.size());
						}

						if (ImGui::MenuItem(name.c_str())) {
							PresetDown(&gParticleDefArray[i]);
						}
					}

					ImGui::EndChild();
				}

				ImGui::Separator();
				ImGui::TextDisabled("From Disk");

				if (ImGui::MenuItem("Open from file...")) {
					const char* filterPatterns[] = { "*.xml", "*xml.compiled" };

					const char* xmlPath = tinyfd_openFileDialog(
						"Open",
						NULL,
						2,
						filterPatterns,
						"XML File / Compiled XML File",
						0
					);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Reload File"))
			{
				openReloadPopup = true;
			}

			if (ImGui::MenuItem("Save File")) {
				// func
			}

			if (ImGui::MenuItem("Export File")) {
				// func
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Show Debugger", nullptr, &showDebugger);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::MenuItem("Preferences");
			if (ImGui::BeginMenu("UI Theme")) {
				if (ImGui::MenuItem("Classic (Default)")) ImGui::StyleColorsClassic();
				if (ImGui::MenuItem("Light")) ImGui::StyleColorsLight();
				if (ImGui::MenuItem("Dark")) ImGui::StyleColorsDark();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation")) { /* open URL */ }
			if (ImGui::MenuItem("About")) { /* show about popup */ }
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (openReloadPopup) {
		ImGui::OpenPopup("Reload Warning");
		openReloadPopup = false;
		mIsSysPaused = true;
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Reload Warning", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("WARNING");
		ImGui::Separator();
		ImGui::Text(
			"Reloading will discard any unsaved changes.\n"
			"This action cannot be undone."
		);

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Reload Anyway", ImVec2(150, 0)))
		{
			mIsSysPaused = false;
			ResetParticle();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(150, 0)))
		{
			mIsSysPaused = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ParticleScreen::Debugger()
{
	if (!showDebugger) return;
	ImGui::Begin("Debugger", &showDebugger, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	const float fps = ImGui::GetIO().Framerate;
	ImGui::Text("FPS: %.1f\n", fps);

	int particles = 0;
	int emitters = 0;
	for (TodParticleSystem* aParticle : gParticleTests) {
		for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext) {
			TodParticleEmitter* emitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
			particles += emitter->mParticleList.mSize;
			emitters++;
		}
	}
	ImGui::Text("Emitters: %d\n", emitters);
	ImGui::Text("Particles: %d\n", particles);
	ImGui::End();
}

void ParticleScreen::ImGuiDraw()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	Debugger();
	MenuBar();
	
	ImGui::Render();
}