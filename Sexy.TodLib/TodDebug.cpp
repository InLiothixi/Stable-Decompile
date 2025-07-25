#include <time.h>
#include "TodDebug.h"
#include "TodCommon.h"
#include "../SexyAppFramework/Debug.h"
#include "../SexyAppFramework/SEHCatcher.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include "../GameConstants.h"

using namespace Sexy;

static char gLogFileName[MAX_PATH];
static char gDebugDataFolder[MAX_PATH];

//0x514EA0
void TodErrorMessageBox(const char* theMessage, const char* theTitle)
{
	HWND hWnd = (gSexyAppBase && gSexyAppBase->mHWnd) ? gSexyAppBase->mHWnd : GetActiveWindow();
	TodTraceAndLog("%s.%s", theMessage, theTitle);
#ifdef _USE_WIDE_STRING
	MessageBox(hWnd, StringToSexyString(theMessage).c_str(), StringToSexyString(theTitle).c_str(), MB_ICONEXCLAMATION);
#else
	MessageBoxA(hWnd, theMessage, theTitle, MB_ICONEXCLAMATION);
#endif
}

void TodTraceMemory()
{
}

void* TodMalloc(int theSize)
{
	TOD_ASSERT(theSize > 0);
	return malloc(theSize);
}

void TodFree(void* theBlock)
{
	if (theBlock != nullptr)
	{
		free(theBlock);
	}
}

void TodAssertFailed(const char* theCondition, const char* theFile, int theLine, const char* theMsg, ...)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	char aFormattedMsg[1024];
	va_list argList;
	va_start(argList, theMsg);
	int aCount = TodVsnprintf(aFormattedMsg, sizeof(aFormattedMsg), theMsg, argList);
	va_end(argList);

	if (aFormattedMsg[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aFormattedMsg[aCount] = '\n';
			aFormattedMsg[aCount + 1] = '\0';
		}
		else
		{
			aFormattedMsg[aCount - 1] = '\n';
		}
	}

	char aBuffer[1024];
	if (*theCondition != '\0')
	{
		TodSnprintf(aBuffer, sizeof(aBuffer), "\n%s(%d)\nassertion failed: '%s'\n%s\n", theFile, theLine, theCondition, aFormattedMsg);
	}
	else
	{
		TodSnprintf(aBuffer, sizeof(aBuffer), "\n%s(%d)\nassertion failed: %s\n", theFile, theLine, aFormattedMsg);
	}
	TodTrace("%s", aBuffer);

	if (!IsDebuggerPresent())
	{
		if (gInAssert)
		{
			TodLog("Assert during exception processing");
			exit(0);
		}


		gInAssert = true;
		LPEXCEPTION_POINTERS exp;

		__try
		{
			RaiseException(EXCEPTION_NONCONTINUABLE_EXCEPTION, NULL, NULL, NULL);
		}
		__except (exp = GetExceptionInformation(), EXCEPTION_CONTINUE_EXECUTION)
		{
			TodReportError(exp, aFormattedMsg);
		}

		gInAssert = false;
		exit(0);
	}
#endif
}

void TodLog(const char* theFormat, ...)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	TodLogString(aButter);
	printf(aButter);
#endif
}

void TodLogString(const char* theMsg)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	FILE* f = fopen(gLogFileName, "a");
	if (f == nullptr)
	{
		OutputDebugString(_S("Failed to open log file\n"));
		printf(_S("Failed to open log file\n"));
	}

	if (fwrite(theMsg, strlen(theMsg), 1, f) != 1)
	{
		OutputDebugString(_S("Failed to write to log file\n"));
		printf(_S("Failed to write to log file\n"));
	}

	fclose(f);
#endif
}

void TodTrace(const char* theFormat, ...)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	OutputDebugStringA(aButter);
	printf(aButter);
#endif
}

void TodHesitationTrace(...)
{
}

void TodTraceAndLog(const char* theFormat, ...)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	OutputDebugStringA(aButter);
	printf(aButter);
	TodLogString(aButter);
#endif
}

void TodTraceWithoutSpamming(const char* theFormat, ...)
{
#ifdef _SHOW_OUTPUT_CONSOLE
	static __time64_t gLastTraceTime = 0i64;
	__time64_t aTime = _time64(nullptr);
	if (aTime < gLastTraceTime)
	{
		return;
	}

	gLastTraceTime = aTime;
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	OutputDebugStringA(aButter);
	printf(aButter);
#endif
}

void TodReportError(LPEXCEPTION_POINTERS exceptioninfo, const char* theMessage)
{
	Sexy::SEHCatcher::UnhandledExceptionFilter(exceptioninfo);
}

long __stdcall TodUnhandledExceptionFilter(LPEXCEPTION_POINTERS exceptioninfo)
{
	if (gInAssert)
	{
		TodLog("Exception during exception processing");
	}
	else
	{
		gInAssert = true;
		TodLog("\nUnhandled Exception");
		TodReportError(exceptioninfo, "Unhandled Exception");
		gInAssert = false;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void (*gBetaSubmitFunc)() = nullptr;

void TodAssertInitForApp()
{
	MkDir(GetAppDataFolder() + "userdata");
	string aRelativeUserPath = GetAppDataFolder() + "userdata\\";
	strcpy(gDebugDataFolder, GetFullPath(aRelativeUserPath).c_str());
	strcpy(gLogFileName, gDebugDataFolder);
	strcpy(gLogFileName + strlen(gLogFileName), "log.txt");
	TOD_ASSERT(strlen(gLogFileName) < MAX_PATH);

	__time64_t aclock = _time64(nullptr);
	TodLog("Started %s\n", asctime(_localtime64(&aclock)));

	SetUnhandledExceptionFilter(TodUnhandledExceptionFilter);
}
