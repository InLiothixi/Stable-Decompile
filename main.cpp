#include "LawnApp.h"
#include "Resources.h"
#include "Sexy.TodLib/TodStringFile.h"
#include "GameConstants.h"

using namespace Sexy;

bool (*gAppCloseRequest)();				//[0x69E6A0]
bool (*gAppHasUsedCheatKeys)();			//[0x69E6A4]
SexyString (*gGetCurrentLevelName)();

//0x44E8F0
#include <shlwapi.h> 
static std::string GetExeDirectory() {
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH); 
	PathRemoveFileSpecA(exePath);

	return std::string(exePath);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	gHInstance = hInstance;

#if defined(_SHOW_OUTPUT_CONSOLE)
    AllocConsole();

    FILE* dummy;
    freopen_s(&dummy, "CONIN$", "r", stdin);
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(hInput, &prevMode);

	SetConsoleMode(hInput, (prevMode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_EXTENDED_FLAGS | ENABLE_INSERT_MODE | ENABLE_PROCESSED_INPUT);
#endif

	TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
	gGetCurrentLevelName = LawnGetCurrentLevelName;
	gAppCloseRequest = LawnGetCloseRequest;
	gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;

	gLawnApp = new LawnApp();
	std::string exeDir = GetExeDirectory();
	if (Sexy::FileExists(exeDir + "\\properties\\resources.xml") ||
		Sexy::FileExists(exeDir + "\\main.pak")) {
		gLawnApp->mChangeDirTo = exeDir;
	}
	else {
		gLawnApp->mChangeDirTo = (!Sexy::FileExists(_S("properties\\resources.xml")) &&
			Sexy::FileExists(_S("..\\properties\\resources.xml"))) ?
			_S("..") : _S(".");
	}
	
	gLawnApp->Init();
	gLawnApp->Start();
	gLawnApp->Shutdown();

	SDL_Renderer* aRenderer = LawnApp::mSDLRenderer;
	SDL_Window* aWindow = LawnApp::mSDLWindow;
	if (gLawnApp)
	{
		gLawnApp->mHWnd = nullptr;
		delete gLawnApp;
		gLawnApp = nullptr;
	}

	SDL_DestroyRenderer(aRenderer);
	SDL_DestroyWindow(aWindow);
	LawnApp::mSDLRenderer = nullptr;
	LawnApp::mSDLWindow = nullptr;
	SDL_Quit();

	return 0;
};
