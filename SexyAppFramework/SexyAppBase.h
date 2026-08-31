#ifndef __SEXYAPPBASE_H__
#define __SEXYAPPBASE_H__

#include "Common.h"
#include "Rect.h"
#include "Color.h"
#include "ButtonListener.h"
#include "DialogListener.h"
#include "Buffer.h"
#include "CritSect.h"
#include "SharedImage.h"
#include "Ratio.h"
#include <ImageLib.h>

namespace ImageLib
{
	class Image;
};

namespace Sexy
{

class WidgetManager;
class DDInterface;
class Image;
class DDImage;
class Widget;
class SoundManager;
class MusicInterface;
class MemoryImage;
class HTTPTransfer;
class Dialog;

class ResourceManager;

class WidgetSafeDeleteInfo
{
public:
	int						mUpdateAppDepth;
	Widget*					mWidget;
};


typedef std::list<WidgetSafeDeleteInfo> WidgetSafeDeleteList;
typedef std::set<MemoryImage*> MemoryImageSet;
typedef std::map<int, Dialog*> DialogMap;
typedef std::list<Dialog*> DialogList;
typedef std::list<MSG> WindowsMessageList;
typedef std::vector<SexyString> StringVector;
//typedef std::basic_string<TCHAR> tstring; // string of TCHARs

typedef std::map<std::string, SexyString> StringSexyStringMap;
typedef std::map<std::string, std::string> StringStringMap;
typedef std::map<std::string, std::wstring> StringWStringMap;
typedef std::map<SexyString, bool> StringBoolMap;
typedef std::map<SexyString, int> StringIntMap;
typedef std::map<SexyString, double> StringDoubleMap;
typedef std::map<SexyString, StringVector> StringStringVectorMap;

enum
{
	CURSOR_POINTER,
	CURSOR_HAND,
	CURSOR_DRAGGING,
	CURSOR_TEXT,
	CURSOR_CIRCLE_SLASH,
	CURSOR_SIZEALL,
	CURSOR_SIZENESW,
	CURSOR_SIZENS,
	CURSOR_SIZENWSE,
	CURSOR_SIZEWE,	
	CURSOR_WAIT,
	CURSOR_NONE,
	CURSOR_CUSTOM,
	NUM_CURSORS
};

enum
{
	DEMO_MOUSE_POSITION,	
	DEMO_ACTIVATE_APP,
	DEMO_SIZE,
	DEMO_KEY_DOWN,
	DEMO_KEY_UP,
	DEMO_KEY_CHAR,
	DEMO_CLOSE,
	DEMO_MOUSE_ENTER,
	DEMO_MOUSE_EXIT,
	DEMO_LOADING_COMPLETE,
	DEMO_REGISTRY_GETSUBKEYS,
	DEMO_REGISTRY_READ,
	DEMO_REGISTRY_WRITE,
	DEMO_REGISTRY_ERASE,	
	DEMO_FILE_EXISTS,
	DEMO_FILE_READ,
	DEMO_FILE_WRITE,
	DEMO_HTTP_RESULT,
	DEMO_SYNC,
	DEMO_ASSERT_STRING_EQUAL,
	DEMO_ASSERT_INT_EQUAL,
	DEMO_MOUSE_WHEEL,
	DEMO_HANDLE_COMPLETE,
	DEMO_VIDEO_DATA,
	DEMO_IDLE = 31
};

enum {
	FPS_ShowFPS,
	FPS_ShowCoords,
	Num_FPS_Types
};

enum
{
	UPDATESTATE_MESSAGES,
	UPDATESTATE_PROCESS_1,
	UPDATESTATE_PROCESS_2,
	UPDATESTATE_PROCESS_DONE
};

typedef std::map<HANDLE, int> HandleToIntMap;

class SexyAppBase : public ButtonListener, public DialogListener
{
public:
	
	ulong					mRandSeed;
		
	SexyString				mCompanyName;
	SexyString				mFullCompanyName;
	SexyString				mProdName;
	SexyString				mTitle;
	SexyString				mRegKey;
	SexyString				mChangeDirTo;
	
	int						mRelaxUpdateBacklogCount; // app doesn't try to catch up for this many frames
	int						mPreferredX;
	int						mPreferredY;
	int						mPreferredWIDTH;
	int						mPreferredHEIGHT;
	bool					mPreferredMAXIMIZED;
	int						mWidth;
	int						mHeight;
	int						mFullscreenBits;
	double					mMusicVolume;
	double					mSfxVolume;
	double					mDemoMusicVolume;
	double					mDemoSfxVolume;
	bool					mNoSoundNeeded;
	bool					mWantFMod;
	bool					mCmdLineParsed;
	bool					mSkipSignatureChecks;
	bool					mStandardWordWrap;
	bool					mbAllowExtendedChars;

	HANDLE					mMutex;
	bool					mOnlyAllowOneCopyToRun;
	UINT					mNotifyGameMessage;
	CritSect				mCritSect;	
	bool					mBetaValidate;
	uchar					mAdd8BitMaxTable[512];
	WidgetManager*			mWidgetManager;
	DialogMap				mDialogMap;
	DialogList				mDialogList;
	DWORD					mPrimaryThreadId;
	bool					mSEHOccured;
	bool					mShutdown;
	bool					mExitToTop;
	bool					mIsWindowed;
	bool					mIsPhysWindowed;
	bool					mFullScreenWindow; // uses ChangeDisplaySettings to run fullscreen with mIsWindowed true
	bool					mForceFullscreen;
	bool					mForceWindowed;	
	bool					mInitialized;	
	bool					mProcessInTimer;
	DWORD					mTimeLoaded;
	HWND					mHWnd;
	HWND					mInvisHWnd;
	bool					mIsScreenSaver;
	bool					mAllowMonitorPowersave;
	WindowsMessageList		mDeferredMessages;
	bool					mNoDefer;	
	bool					mFullScreenPageFlip;	
	bool					mTabletPC;
	DDInterface*			mDDInterface;
	bool					mAlphaDisabled;
	MusicInterface*			mMusicInterface;	
	bool					mReadFromRegistry;
	SexyString				mRegisterLink;
	SexyString				mProductVersion;
	Image*					mCursorImages[NUM_CURSORS];
	HCURSOR					mOverrideCursor;
	bool					mIsOpeningURL;
	bool					mShutdownOnURLOpen;
	SexyString				mOpeningURL;
	DWORD					mOpeningURLTime;
	DWORD					mLastTimerTime;
	DWORD					mLastBigDelayTime;	
	double					mUnmutedMusicVolume;
	double					mUnmutedSfxVolume;	
	int						mMuteCount;
	int						mAutoMuteCount;
	bool					mDemoMute;
	bool					mMuteOnLostFocus;
	MemoryImageSet			mMemoryImageSet;
	SharedImageMap			mSharedImageMap;
	bool					mCleanupSharedImages;
	
	int						mNonDrawCount;
	int						mFrameTime;

	bool					mIsDrawing;
	bool					mLastDrawWasEmpty;
	bool					mHasPendingDraw;
	double					mPendingUpdatesAcc;
	double					mUpdateFTimeAcc;
	DWORD					mLastTimeCheck;
	DWORD					mLastTime;
	DWORD					mLastUserInputTick;

	int						mSleepCount;
	int						mDrawCount;
	int						mUpdateCount;
	int						mUpdateAppState;
	int						mUpdateAppDepth;
	double					mUpdateMultiplier;		
	bool					mPaused;
	int						mFastForwardToUpdateNum;
	bool					mFastForwardToMarker;
	bool					mFastForwardStep;
	DWORD					mLastDrawTick;
	DWORD					mNextDrawTick;
	int						mStepMode;  // 0 = off, 1 = step, 2 = waiting for step

	int						mCursorNum;
	SoundManager*			mSoundManager;
	HCURSOR					mHandCursor;
	HCURSOR					mDraggingCursor;
	WidgetSafeDeleteList	mSafeDeleteList;
	bool					mMouseIn;	
	bool					mRunning;
	bool					mActive;
	bool					mMinimized;
	bool					mPhysMinimized;
	bool					mIsDisabled;
	bool					mHasFocus;
	int						mDrawTime;
	ulong					mFPSStartTick;
	int						mFPSFlipCount;
	int						mFPSDirtyCount;
	int						mFPSTime;
	int						mFPSCount;
	bool					mShowFPS;
	int						mShowFPSMode;
	int						mScreenBltTime;
	bool					mAutoStartLoadingThread;
	bool					mLoadingThreadStarted;
	bool					mLoadingThreadCompleted;
	bool					mLoaded;
	bool					mYieldMainThread;
	bool					mLoadingFailed;
	bool					mCursorThreadRunning;
	bool					mSysCursor;	
	bool					mCustomCursorsEnabled;
	bool					mCustomCursorDirty;	
	bool					mLastShutdownWasGraceful;
	bool					mIsWideWindow;

	int						mNumLoadingThreadTasks;
	int						mCompletedLoadingThreadTasks;

	// For recording/playback of program control
	bool					mRecordingDemoBuffer;
	bool					mPlayingDemoBuffer;
	bool					mManualShutdown;
	SexyString 			    mDemoPrefix;
	SexyString				mDemoFileName;
	Buffer					mDemoBuffer;
	int						mDemoLength;
	int						mLastDemoMouseX;
	int						mLastDemoMouseY;
	int						mLastDemoUpdateCnt;
	bool					mDemoNeedsCommand;
	bool					mDemoIsShortCmd;
	int						mDemoCmdNum;
	int						mDemoCmdOrder;
	int						mDemoCmdBitPos;
	bool					mDemoLoadingComplete;
	HandleToIntMap			mHandleToIntMap; // For waiting on handles
	int						mCurHandleNum;

	typedef std::pair<SexyString, int> DemoMarker;
	typedef std::list<DemoMarker> DemoMarkerList;
	DemoMarkerList			mDemoMarkerList;

	bool					mDebugKeysEnabled;
	bool					mEnableMaximizeButton;
	bool					mCtrlDown;
	bool					mAltDown;
	
	int						mSyncRefreshRate;
	bool					mVSyncUpdates;
	bool					mVSyncBroken;
	int						mVSyncBrokenCount;
	DWORD					mVSyncBrokenTestStartTick;
	DWORD					mVSyncBrokenTestUpdates;
	bool					mWaitForVSync;
	bool					mSoftVSyncWait;
	bool					mUserChanged3DSetting;
	bool					mAutoEnable3D;
	bool					mTest3D;
	DWORD					mMinVidMemory3D;
	DWORD					mRecommendedVidMemory3D;

	bool					mWidescreenAware;
	Rect					mScreenBounds;
	bool					mEnableWindowAspect;
	Ratio					mWindowAspect;

	StringWStringMap		mStringProperties;
	StringBoolMap			mBoolProperties;
	StringIntMap			mIntProperties;
	StringDoubleMap			mDoubleProperties;
	StringStringVectorMap	mStringVectorProperties;
	ResourceManager*		mResourceManager;

#ifdef ZYLOM
	uint					mZylomGameId;
#endif

	LONG					mOldWndProc;
	bool					mIsPreviewSaver;
	HWND					mPreviewHWnd;

	bool					mIsParticleEditor;
	bool					mIsTouch;
	int						mResolutionMode;

	bool					mEnableVsync;

protected:	
	void					RehupFocus();
	void					ClearKeysDown();
	bool					ProcessDeferredMessages(bool singleMessage);
	void					UpdateFTimeAcc();
	virtual bool			Process(bool allowSleep = true);		
	virtual void			UpdateFrames();
	virtual bool			DoUpdateFrames();
	virtual void			DoUpdateFramesF(float theFrac);
	virtual void			MakeWindow();
	virtual void			EnforceCursor();
	virtual void			ReInitImages();
	virtual void			DeleteNativeImageData();	
	virtual void			DeleteExtraImageData();
	
	// Loading thread methods	
	virtual void			LoadingThreadCompleted();
	static void				LoadingThreadProcStub(void *theArg);	

	// Cursor thread methods
	void					CursorThreadProc();
	static void				CursorThreadProcStub(void *theArg);
	void					StartCursorThread();
	
	void					WaitForLoadingThread();				
	void					ProcessSafeDeleteList();	
	void					RestoreScreenResolution();
	void					DoExit(int theCode);

	void					TakeScreenshot();
	void					DumpProgramInfo();	
	void					ShowMemoryUsage();			

	// Registry helpers
	bool					RegistryRead(const SexyString& theValueName, ulong* theType, uchar* theValue, ulong* theLength);
	bool					RegistryReadKey(const SexyString& theValueName, ulong* theType, uchar* theValue, ulong* theLength, HKEY theMainKey = HKEY_CURRENT_USER);
	bool					RegistryWrite(const SexyString& theValueName, ulong theType, const uchar* theValue, ulong theLength);

	// Demo recording helpers	
	void					ProcessDemo();

public:
	SexyAppBase();
	virtual ~SexyAppBase();

	// Common overrides:
	virtual MusicInterface*	CreateMusicInterface(HWND theHWnd);
	virtual void			InitHook();
	virtual void			ShutdownHook();	
	virtual void			PreTerminate();
	virtual void			LoadingThreadProc();
	virtual void			WriteToRegistry();
	virtual void			ReadFromRegistry();
	virtual Dialog*			NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);		
	virtual void			PreDisplayHook();

	// Public methods
	virtual void			BeginPopup();
	virtual void			EndPopup();
	virtual int				MsgBox(const std::string &theText, const std::string &theTitle = "Message", int theFlags = MB_OK);
	virtual int				MsgBox(const std::wstring &theText, const std::wstring &theTitle = L"Message", int theFlags = MB_OK);
	virtual void			Popup(const std::string& theString);
	virtual void			Popup(const std::wstring& theString);
	virtual void			LogScreenSaverError(const SexyString&theError);
	virtual void			SafeDeleteWidget(Widget* theWidget);	

	virtual void			URLOpenFailed(const SexyString& theURL);
	virtual void			URLOpenSucceeded(const SexyString& theURL);
	virtual bool			OpenURL(const SexyString& theURL, bool shutdownOnOpen = false);
	virtual SexyString		GetProductVersion(const SexyString& thePath);

	virtual void			SEHOccured();
	virtual SexyString		GetGameSEHInfo();
	virtual void			GetSEHWebParams(DefinesMap* theDefinesMap);
	virtual void			Shutdown();	

	virtual void			DoParseCmdLine();
	virtual void			ParseCmdLine(const SexyString& theCmdLine);
	virtual void			HandleCmdLineParam(const SexyString& theParamName, const SexyString& theParamValue);
	virtual void			HandleNotifyGameMessage(int theType, int theParam); // for HWND_BROADCAST of mNotifyGameMessage (0-1000 are reserved for SexyAppBase for theType)
	virtual void			HandleGameAlreadyRunning(); 

	virtual void			Start();	
	virtual void			Init();	
	virtual void			PreDDInterfaceInitHook();
	virtual void			PostDDInterfaceInitHook();
	virtual bool			ChangeDirHook(const char *theIntendedPath);
	virtual void			PlaySample(int theSoundNum);
	virtual void			PlaySample(int theSoundNum, int thePan);

	virtual double			GetMasterVolume();
	virtual double			GetMusicVolume();
	virtual double			GetSfxVolume();
	virtual bool			IsMuted();

	virtual void			SetMasterVolume(double theVolume);
	virtual void			SetMusicVolume(double theVolume);
	virtual void			SetSfxVolume(double theVolume);	
	virtual void			Mute(bool autoMute = false);
	virtual void			Unmute(bool autoMute = false);

	void					StartLoadingThread();
	virtual double			GetLoadingThreadProgress();	

	void					CopyToClipboard(const SexyString& theString);
	SexyString				GetClipboard();

	void					SetCursor(int theCursorNum);
	int						GetCursor();
	void					EnableCustomCursors(bool enabled);	
	virtual DDImage*		GetImage(const SexyString& theFileName, bool commitBits = true);
	virtual SharedImageRef	GetSharedImage(const SexyString& theFileName, const SexyString& theVariant = _S(""), bool* isNew = NULL);

	void					CleanSharedImages();
	void					PrecacheAdditive(MemoryImage* theImage);
	void					PrecacheAlpha(MemoryImage* theImage);
	void					PrecacheNative(MemoryImage* theImage);
	void					SetCursorImage(int theCursorNum, Image* theImage);

	DDImage*				CreateCrossfadeImage(Image* theImage1, const Rect& theRect1, Image* theImage2, const Rect& theRect2, double theFadeFactor);
	void					ColorizeImage(Image* theImage, const Color& theColor);
	DDImage*				CreateColorizedImage(Image* theImage, const Color& theColor);
	DDImage*				CopyImage(Image* theImage, const Rect& theRect);
	DDImage*				CopyImage(Image* theImage);
	void					MirrorImage(Image* theImage);
	void					FlipImage(Image* theImage);
	void					RotateImageHue(Sexy::MemoryImage *theImage, int theDelta);
	ulong					HSLToRGB(int h, int s, int l);
	ulong					RGBToHSL(int r, int g, int b);
	void					HSLToRGB(const ulong* theSource, ulong* theDest, int theSize);
	void					RGBToHSL(const ulong* theSource, ulong* theDest, int theSize);

	void					AddMemoryImage(MemoryImage* theMemoryImage);
	void					RemoveMemoryImage(MemoryImage* theMemoryImage);
	void					Remove3DData(MemoryImage* theMemoryImage);
	virtual void			SwitchScreenMode();
	virtual void			SwitchScreenMode(bool wantWindowed);
	virtual void			SwitchScreenMode(bool wantWindowed, bool is3d, bool force = false);
	virtual void			SetAlphaDisabled(bool isDisabled);
	
	virtual Dialog*			DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
	virtual Dialog*			GetDialog(int theDialogId);
	virtual void			AddDialog(int theDialogId, Dialog* theDialog);
	virtual void			AddDialog(Dialog* theDialog);
	virtual bool			KillDialog(int theDialogId, bool removeWidget, bool deleteWidget);
	virtual bool			KillDialog(int theDialogId);
	virtual bool			KillDialog(Dialog* theDialog);
	virtual int				GetDialogCount();
	virtual void			ModalOpen();
	virtual void			ModalClose();	
	virtual void			DialogButtonPress(int theDialogId, int theButtonId);
	virtual void			DialogButtonDepress(int theDialogId, int theButtonId);

	virtual void			GotFocus();
	virtual void			LostFocus();	
	virtual bool			IsAltKeyUsed(WPARAM wParam);
	virtual bool			DebugKeyDown(int theKey);	
	virtual bool			DebugKeyDownAsync(int theKey, bool ctrlDown, bool altDown);
	void					SetShowFPS(bool theShowFPS);
	virtual void			CloseRequestAsync();
	bool					Is3DAccelerated();
	bool					Is3DAccelerationSupported();
	bool					Is3DAccelerationRecommended();
	void					DemoSyncRefreshRate();
	void					Set3DAcclerated(bool is3D, bool reinit = true);
	virtual void			Done3dTesting();
	virtual SexyString		NotifyCrashHook(); // return file name that you want to upload
	
	virtual bool			CheckSignature(const Buffer& theBuffer, const SexyString& theFileName);
	virtual bool			DrawDirtyStuff();
	virtual void			Redraw(Rect* theClipRect);

	// Properties access methods
	bool					LoadProperties(const SexyString& theFileName, bool required, bool checkSig);
	bool					LoadProperties();
	virtual void			InitPropertiesHook();

	// Resource access methods
	void					LoadResourceManifest();
	void					ShowResourceError(bool doExit = false);
	
	bool					GetBoolean(const SexyString& theId);
	bool					GetBoolean(const SexyString& theId, bool theDefault);
	int						GetInteger(const SexyString& theId);
	int						GetInteger(const SexyString& theId, int theDefault);
	double					GetDouble(const SexyString& theId);
	double					GetDouble(const SexyString& theId, double theDefault);
	SexyString				GetString(const SexyString& theId);
	SexyString				GetString(const SexyString& theId, const SexyString& theDefault);

	StringVector			GetStringVector(const SexyString& theId);

	void					SetBoolean(const SexyString& theId, bool theValue);
	void					SetInteger(const SexyString& theId, int theValue);
	void					SetDouble(const SexyString& theId, double theValue);
	void					SetString(const SexyString& theId, const std::wstring& theValue);
	
	// Demo access methods
	bool					PrepareDemoCommand(bool required);
	void					WriteDemoTimingBlock();
	void					WriteDemoBuffer();
	bool					ReadDemoBuffer(SexyString&theError);//UNICODE
	void					DemoSyncBuffer(Buffer* theBuffer);
	void					DemoSyncString(SexyString* theString);
	void					DemoSyncInt(int* theInt);
	void					DemoSyncBool(bool* theBool);
	void					DemoAssertStringEqual(const SexyString& theString);
	void					DemoAssertIntEqual(int theInt);
	void					DemoAddMarker(const SexyString& theString);
	void					DemoRegisterHandle(HANDLE theHandle);
	void					DemoWaitForHandle(HANDLE theHandle);
	bool					DemoCheckHandle(HANDLE theHandle);
	

	// Registry access methods
	bool					RegistryGetSubKeys(const SexyString& theKeyName, StringVector* theSubKeys);
	bool					RegistryReadString(const SexyString& theValueName, SexyString* theString);
	bool					RegistryReadInteger(const SexyString& theValueName, int* theValue);
	bool					RegistryReadBoolean(const SexyString& theValueName, bool* theValue);
	bool					RegistryReadData(const SexyString& theValueName, uchar* theValue, ulong* theLength);
	bool					RegistryWriteString(const SexyString& theValueName, const SexyString& theString);
	bool					RegistryWriteInteger(const SexyString& theValueName, int theValue);
	bool					RegistryWriteBoolean(const SexyString& theValueName, bool theValue);
	bool					RegistryWriteData(const SexyString& theValueName, const uchar* theValue, ulong theLength);
	bool					RegistryEraseKey(const SexyString& theKeyName);
	void					RegistryEraseValue(const SexyString& theValueName);

	// File access methods
	bool					WriteBufferToFile(const SexyString& theFileName, const Buffer* theBuffer);
	bool					ReadBufferFromFile(const SexyString& theFileName, Buffer* theBuffer, bool dontWriteToDemo = false);//UNICODE
	bool					WriteBytesToFile(const SexyString& theFileName, const void *theData, unsigned long theDataLen);
	bool					FileExists(const SexyString& theFileName);
	bool					EraseFile(const SexyString& theFileName);

	// Misc methods
	virtual void			DoMainLoop();
	virtual bool			UpdateAppStep(bool* updated);
	virtual bool			UpdateApp();
	int						InitDDInterface();
	void					ClearUpdateBacklog(bool relaxForASecond = false);
	bool					IsScreenSaver();
	virtual bool			AppCanRestore();
	static LRESULT CALLBACK	WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	

	bool					IsPreviewSaver();
	bool					IsParticleEditor();
};

extern SexyAppBase* gSexyAppBase;

};

#endif //__SEXYAPPBASE_H__
