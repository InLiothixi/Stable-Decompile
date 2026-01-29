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
#include "../SexyAppFramework/XMLParser.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

extern "C" {
#include <tinyfiledialogs.h>
}

std::vector<TodParticleSystem*> gParticleTests;

bool showDebugger = false;
bool showEmitterPanel = false;
bool showGrid = true;
bool showAxis = true;
bool showSpawnArea = true;
bool lockCamera = false;
bool panInvertX = false;
bool panInvertY = false;
float panSens = 1.0f;

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
	delete mParticleDef;
	mParticleDef = nullptr;
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
	aParticle->TodParticleInitializeFromDef(mWidth / 2, mHeight / 2, 0, mParticleDef, ParticleEffect::PARTICLE_NONE);

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

	if (mIsDown && mWidgetManager->mLastMouseX < mWidth && !lockCamera)
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
			mViewX = mStartViewX + (mWidgetManager->mLastMouseX - mDownX) * panSens * (panInvertX ? -1 : 1);
			mViewY = mStartViewY + (mWidgetManager->mLastMouseY - mDownY) * panSens * (panInvertY ? -1 : 1);

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
	if (showGrid)
	{
		for (int y = 0; y < (aExtraHeight * 2 + mHeight) / 50 + 1; y++)
		{
			g->SetColor(Color(32, 32, 32));
			int theY = y * 50 + mViewY - aExtraHeight;
			g->DrawLine(0, theY, mWidth, theY);
		}
		for (int x = 0; x < (mWidth * 2) / 50 + 1; x++)
		{
			g->SetColor(Color(32, 32, 32));
			int theX = x * 50 - mWidth / 2 + mViewX;
			g->DrawLine(theX, 0, theX, mHeight);
		}
	}
	if (showAxis)
	{
		g->SetColor(Color(255, 0, 0));
		g->DrawLine(0, screenZeroY, mWidth, screenZeroY);
		g->SetColor(Color(0, 255, 0));
		g->DrawLine(screenZeroX, 0, screenZeroX, mHeight);
	}
	g->mTransX += mViewX;
	g->mTransY += mViewY;

	TodParticleSystem* aParticle;
	if (aParticle = gParticleTests[0])
	{
		for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext)
		{
			TodParticleEmitter* emitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
			TodEmitterDefinition* def = emitter->mEmitterDef;

			if (showSpawnArea)
			{
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
			}

			for (int i = 0; i < emitter->mEmitterDef->mParticleFieldCount; i++)
			{
				ParticleField* aParticleField = &emitter->mEmitterDef->mParticleFields[i];
				if (aParticleField->mFieldType == ParticleFieldType::FIELD_GROUND_CONSTRAINT && showAxis) {
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
bool openAboutPopup = false;

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

					TodTrace(xmlPath);


					XMLParser aXMLParser = XMLParser();
					if (!aXMLParser.OpenFile(xmlPath))
					{
						TodTrace("File not found!");

					}
					else
					{
						TodTrace("File Found!");
					}
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
			ImGui::TextDisabled("Debug");
			ImGui::Separator();
			ImGui::MenuItem("Show Debugger", nullptr, &showDebugger);
			ImGui::Separator();
			ImGui::TextDisabled("Tools");
			ImGui::MenuItem("Show Emitter Panel", nullptr, &showEmitterPanel);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::TextDisabled("Background");
			ImGui::Separator();
			ImGui::MenuItem("Show Grid", nullptr, &showGrid);
			ImGui::MenuItem("Show Axes", nullptr, &showAxis);
			ImGui::MenuItem("Show Spawn-area", nullptr, &showSpawnArea);
			ImGui::Separator();
			ImGui::TextDisabled("UI");
			if (ImGui::BeginMenu("Theme")) {
				if (ImGui::MenuItem("Classic (Default)")) ImGui::StyleColorsClassic();
				if (ImGui::MenuItem("Light")) ImGui::StyleColorsLight();
				if (ImGui::MenuItem("Dark")) ImGui::StyleColorsDark();
				ImGui::EndMenu();
			}
			ImGui::Separator();
			ImGui::TextDisabled("Camera");
			ImGui::MenuItem("Lock Position", nullptr, &lockCamera);
			if (ImGui::BeginMenu("Panning"))
			{
				ImGui::MenuItem("Invert X Direction", nullptr, &panInvertX);
				ImGui::MenuItem("Invert Y Direction", nullptr, &panInvertY);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f + 20);
				ImGui::SliderFloat("Speed", &panSens, 0.1f, 3.0f, "%.2fx");
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation")) { /* open URL */ }
			if (ImGui::MenuItem("About")) {
				openAboutPopup = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (openReloadPopup) {
		ImGui::OpenPopup("Reload Warning");
		openReloadPopup = false;
		mIsSysPaused = true;
	}

	if (openAboutPopup) {
		ImGui::OpenPopup("About");
		openAboutPopup = false;
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

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Particle Editor");
		ImGui::SameLine();
		ImGui::TextDisabled(" v0.1");
		ImGui::Separator();
		ImGui::Text(
			"A particle-creator tool for\nthe original game \"Plants Vs Zombies (2009/GOTY)\".\n\n"
		);
		ImGui::Text("Made by @inliothixie");
		ImGui::SameLine();
		ImGui::TextDisabled(" with imgui");

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Join Discord", ImVec2(150, 0)))
		{
			ShellExecute(NULL, _S("open"), "http://discord.com/invite/feJPyVt6HH", NULL, NULL, SW_SHOWNORMAL);
		}

		ImGui::SameLine();

		if (ImGui::Button("Close", ImVec2(150, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ParticleScreen::Debugger()
{
	if (!showDebugger) return;
	ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + 10.0f, vp->Pos.y + vp->Size.y - 10.0f),ImGuiCond_Always, ImVec2(0.0f, 1.0f));	
	ImGui::Begin("Debugger", &showDebugger, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
	const float fps = ImGui::GetIO().Framerate;
	ImGui::Text("FPS: %.1f", fps);

	int particles = 0;
	int emitters = 0;
	for (TodParticleSystem* aParticle : gParticleTests) {
		for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext) {
			TodParticleEmitter* emitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
			particles += emitter->mParticleList.mSize;
			emitters++;
		}
	}
	ImGui::Text("Emitters: %d", emitters);
	ImGui::Text("Particles: %d", particles);
	ImGui::End();
}


void ParticleScreen::EmitterPannel()
{
	if (!showEmitterPanel) return;
	ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + 10.0f, vp->Pos.y + 30.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Emitter Pannel", &showEmitterPanel, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);

	int emitterCount = 0;
	for (TodParticleSystem* ps : gParticleTests)
		for (auto* n = ps->mEmitterList.mHead; n; n = n->mNext)
			emitterCount++;

	ImGui::TextDisabled("Emitters (%d)", emitterCount);
	ImGui::Separator();

	ImGui::BeginChild("EmitterScroll", ImVec2(200.0f, ImGui::GetFrameHeight() * min(emitterCount, 7)), false, ImGuiWindowFlags_None);

	if (ImGui::BeginTable("EmitterTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchSame))
	{
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("Modify");
		ImGui::TableSetupColumn("Delete");

		for (int i = 0; i < mParticleDef->mEmitterDefCount; i++)
		{
			TodEmitterDefinition* def = &mParticleDef->mEmitterDefs[i];

			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			if (ImGui::Selectable(StrFormat("%s##sel%d", gEmitterTypeSymbols[0].mSymbolName, i).c_str(), false))
			{
			}

			ImGui::TableNextColumn();
			if (ImGui::SmallButton(StrFormat("Duplicate##%d", i).c_str()))
			{
				memcpy(&mParticleDef->mEmitterDefs[mParticleDef->mEmitterDefCount], def, sizeof(TodEmitterDefinition));
				mParticleDef->mEmitterDefCount++;
			}

			ImGui::TableNextColumn();
			if (ImGui::SmallButton(StrFormat("Delete##%d", i).c_str()))
			{

			}
		}

		ImGui::EndTable();
	}

	ImGui::EndChild();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Add Emitter"))
	{
		mParticleDef->mEmitterDefCount++;
		TodEmitterDefinition emitter = mParticleDef->mEmitterDefs[mParticleDef->mEmitterDefCount] = TodEmitterDefinition();
		memcpy(&emitter, &mParticleDef->mEmitterDefs[mParticleDef->mEmitterDefCount--], sizeof(TodEmitterDefinition));
	}

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
	EmitterPannel();
	
	ImGui::Render();
}