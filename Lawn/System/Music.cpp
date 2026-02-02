#include "Music.h"
#include "../Board.h"
#include "PlayerInfo.h"
#include "../../LawnApp.h"
#include "../../PakLib/PakInterface.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/BassLoader.h"
#include "../../SexyAppFramework/BassMusicInterface.h"

using namespace Sexy;

//0x45A260
Music::Music()
{
	mApp = (LawnApp*)gSexyAppBase;
	mMusicInterface = gSexyAppBase->mMusicInterface;
	mCurMusicTune = MusicTune::MUSIC_TUNE_NONE;
	mCurMusicFileMain = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileDrums = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileHihats = MusicFile::MUSIC_FILE_NONE;
	mBurstOverride = -1;
	mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_OFF;
	mQueuedDrumTrackPackedOrder = -1;
	mBaseBPM = 155;
	mBaseModSpeed = 3;
	mMusicBurstState = MusicBurstState::MUSIC_BURST_OFF;
	mPauseOffset = 0;
	mPauseOffsetDrums = 0;
	mPaused = false;
	mMusicDisabled = false;
	mFadeOutCounter = 0;
	mFadeOutDuration = 0;
}

MusicFileData gMusicFileData[MusicFile::NUM_MUSIC_FILES];  //0x6A9ED0

//0x45A2C0
bool Music::TodLoadMusic(MusicFile theMusicFile, const std::string& theFileName)
{
	if (mApp->mNoSoundNeeded)	return false;

	HMUSIC aHMusic = NULL;
	HSTREAM aStream = NULL;
	BassMusicInterface* aBass = (BassMusicInterface*)mApp->mMusicInterface;
	std::string anExt;

	int aDot = theFileName.rfind('.');
	if (aDot != std::string::npos)  // 文件名中不含“.”（文件无扩展名）
		anExt = StringToLower(theFileName.substr(aDot + 1));  // 取得小写的文件扩展名

	if (anExt.compare("wav") && anExt.compare("ogg") && anExt.compare("mp3"))  // 如果不是这三种拓展名
	{
		PFILE* pFile = p_fopen(("resourcepack\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(("extension\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(("dependency\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(theFileName.c_str(), "rb");

		if (pFile == nullptr)
			return false;

		p_fseek(pFile, 0, SEEK_END);  // 指针调整至文件末尾
		int aSize = p_ftell(pFile);  // 当前位置即为文件长度
		p_fseek(pFile, 0, SEEK_SET);  // 指针调回文件开头
		void* aData = operator new[](aSize);
		p_fread(aData, sizeof(char), aSize, pFile);  // 按字节读取数据
		p_fclose(pFile);  // 关闭文件流

		aHMusic = gBass->BASS_MusicLoad(BASS_FILE_MEM, aData, 0, aSize, aBass->mMusicLoadFlags, 0);
		delete[](char*)aData;

		if (aHMusic == NULL)
			return false;
	}
	else
	{
		PFILE* pFile = p_fopen(("resourcepack\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(("extension\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(("dependency\\" + theFileName).c_str(), "rb");

		if (pFile == nullptr)
			pFile = p_fopen(theFileName.c_str(), "rb");

		if (pFile == nullptr)
			return false;

		p_fseek(pFile, 0, SEEK_END);  // 指针调整至文件末尾
		int aSize = p_ftell(pFile);  // 当前位置即为文件长度
		p_fseek(pFile, 0, SEEK_SET);  // 指针调回文件开头
		void* aData = operator new[](aSize);
		p_fread(aData, sizeof(char), aSize, pFile);  // 按字节读取数据
		p_fclose(pFile);  // 关闭文件流
		
		aStream = gBass->BASS_StreamCreateFile(true, aData, 0, aSize, 0);
		TOD_ASSERT(gMusicFileData[theMusicFile].mFileData == nullptr);
		gMusicFileData[theMusicFile].mFileData = (unsigned int*)aData;

		if (aStream == 0)
			return false;
	}
	
	BassMusicInfo aMusicInfo;
	aMusicInfo.mHStream = aStream;
	aMusicInfo.mHMusic = aHMusic;
	aBass->mMusicMap.insert(BassMusicMap::value_type(theMusicFile, aMusicInfo));  // 将目标音乐文件编号和音乐信息的对组加入音乐数据容器
	return true;
}

//0x45A6C0
void Music::SetupMusicFileForTune(MusicFile theMusicFile, MusicTune theMusicTune)
{
	int aTrackCount = 0;
	int aTrackStart1 = -1, aTrackEnd1 = -1, aTrackStart2 = -1, aTrackEnd2 = -1;

	switch (theMusicTune)
	{
	case MusicTune::MUSIC_TUNE_DAY_GRASSWALK:
		switch (theMusicFile) {
		case MusicFile::MUSIC_FILE_MAIN_MUSIC:		aTrackCount = 29;	aTrackStart1 = 0;	aTrackEnd1 = 23;											break;
		case MusicFile::MUSIC_FILE_HIHATS:			aTrackCount = 29;	aTrackStart1 = 27;	aTrackEnd1 = 27;											break;
		case MusicFile::MUSIC_FILE_DRUMS:			aTrackCount = 29;	aTrackStart1 = 24;	aTrackEnd1 = 26;											break;
		} break;
	case MusicTune::MUSIC_TUNE_POOL_WATERYGRAVES:
		switch (theMusicFile) {
		case MusicFile::MUSIC_FILE_MAIN_MUSIC:		aTrackCount = 29;	aTrackStart1 = 0;	aTrackEnd1 = 17;											break;
		case MusicFile::MUSIC_FILE_HIHATS:			aTrackCount = 29;	aTrackStart1 = 18;	aTrackEnd1 = 24;	aTrackStart2 = 29;	aTrackEnd2 = 29;	break;
		case MusicFile::MUSIC_FILE_DRUMS:			aTrackCount = 29;	aTrackStart1 = 25;	aTrackEnd1 = 28;											break;
		} break;
	case MusicTune::MUSIC_TUNE_FOG_RIGORMORMIST:
		switch (theMusicFile) {
		case MusicFile::MUSIC_FILE_MAIN_MUSIC:		aTrackCount = 29;	aTrackStart1 = 0;	aTrackEnd1 = 15;											break;
		case MusicFile::MUSIC_FILE_HIHATS:			aTrackCount = 29;	aTrackStart1 = 23;	aTrackEnd1 = 23;											break;
		case MusicFile::MUSIC_FILE_DRUMS:			aTrackCount = 29;	aTrackStart1 = 16;	aTrackEnd1 = 22;											break;
		} break;
	case MusicTune::MUSIC_TUNE_ROOF_GRAZETHEROOF:
		switch (theMusicFile) {
		case MusicFile::MUSIC_FILE_MAIN_MUSIC:		aTrackCount = 29;	aTrackStart1 = 0;	aTrackEnd1 = 17;											break;
		case MusicFile::MUSIC_FILE_HIHATS:			aTrackCount = 29;	aTrackStart1 = 21;	aTrackEnd1 = 21;											break;
		case MusicFile::MUSIC_FILE_DRUMS:			aTrackCount = 29;	aTrackStart1 = 18;	aTrackEnd1 = 20;											break;
		} break;
	default:
		if (theMusicFile == MusicFile::MUSIC_FILE_MAIN_MUSIC || theMusicFile == MusicFile::MUSIC_FILE_HIHATS || theMusicFile == MusicFile::MUSIC_FILE_DRUMS)
		{
			aTrackCount = 29;
			aTrackStart1 = 0;
			aTrackEnd1 = 29;
		}
		break;
	}

	HMUSIC aHMusic = GetBassMusicHandle(theMusicFile);
	for (int aTrack = 0; aTrack <= 29; aTrack++) 
	{
		gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_VOL_CHAN + aTrack, 1.0f);
	}

	for (int aTrack = 0; aTrack <= aTrackCount; aTrack++) // Originally: aTrack < aTrackCount (It ignores Channel 30 which makes Hihats play on Non-Burst Music)
	{
		int aVolume;
		if (aTrack >= aTrackStart1 && aTrack <= aTrackEnd1)
			aVolume = 1.0f;
		else if (aTrack >= aTrackStart2 && aTrack <= aTrackEnd2)
			aVolume = 1.0f;
		else
			aVolume = 0;

		gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_VOL_CHAN + aTrack, aVolume);
	}
}

void Music::LoadSong(MusicFile theMusicFile, const std::string& theFileName)
{
	TodHesitationTrace("preloadsong");
	if (!TodLoadMusic(theMusicFile, theFileName))
	{
		TodTrace("music failed to load\n");
		mMusicDisabled = true;
	}
	else
	{
		gBass->BASS_ChannelSetAttribute(GetBassMusicHandle(theMusicFile), BASS_ATTRIB_MUSIC_PSCALER, 4);
		TodHesitationTrace("song '%s'", theFileName.c_str());
	}
}

//0x45A8A0
void Music::MusicTitleScreenInit()
{
	LoadSong(MusicFile::MUSIC_FILE_MAIN_MUSIC, "sounds\\mainmusic.mo3");

	if (!mApp->IsScreenSaver() && !mApp->IsPreviewSaver() && !mApp->IsParticleEditor())
		MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);
}

//0x45A980
void Music::MusicInit()
{
	if (mApp->IsParticleEditor()) return;

#ifdef _DEBUG
	int aNumLoadingTasks = mApp->mCompletedLoadingThreadTasks + GetNumLoadingTasks();
#endif 
	LoadSong(MusicFile::MUSIC_FILE_DRUMS, "sounds\\mainmusic.mo3");
	mApp->mCompletedLoadingThreadTasks += /*原版*/3500;///*内测版*/800;

	if (mApp->IsScreenSaver() || mApp->IsPreviewSaver())
		return;

	LoadSong(MusicFile::MUSIC_FILE_HIHATS, "sounds\\mainmusic.mo3"); // Originally: "sounds\\mainmusic_hihats.mo3" (Hihats have missing instruments in MUSIC_TUNE_POOL_WATERYGRAVES)
	mApp->mCompletedLoadingThreadTasks += /*原版*/3500;///*内测版*/800;
	if (mApp->HasFinishedAdventure()) 
	{
		LoadSong(MusicFile::MUSIC_FILE_CREDITS_ZOMBIES_ON_YOUR_LAWN, "sounds\\ZombiesOnYourLawn.ogg");
		mApp->mCompletedLoadingThreadTasks += /*原版*/3500;///*内测版*/800;
	}
#ifdef _DEBUG
	if (mApp->mCompletedLoadingThreadTasks != aNumLoadingTasks)
		TodTrace("Didn't calculate loading task count correctly!!!!");
#endif
	//LoadSong(MusicFile::MUSIC_FILE_GRASS_THE_MOON, "sounds\\grassthemoon.ogg");
	//mApp->mCompletedLoadingThreadTasks += /*原版*/3500;
	//LoadSong(MusicFile::MUSIC_FILE_GRASS_THE_MOON_DRUMS, "sounds\\grassthemoon_drums.ogg");
	//mApp->mCompletedLoadingThreadTasks += /*原版*/3500;
	//LoadSong(MusicFile::MUSIC_FILE_GRASS_THE_MOON_HIHATS, "sounds\\grassthemoon_hihats.ogg");
	//mApp->mCompletedLoadingThreadTasks += /*原版*/3500;
	//LoadSong(MusicFile::MUSIC_FILE_FOG_PARTY, "sounds\\fogparty.ogg");
	//mApp->mCompletedLoadingThreadTasks += /*原版*/3500;

}

//0x45AAC0
void Music::MusicLoadCreditsSong()
{
//#ifndef _CMD
	BassMusicInterface* aBass = (BassMusicInterface*)mApp->mMusicInterface;
	if (aBass->mMusicMap.find((int)MusicFile::MUSIC_FILE_CREDITS_ZOMBIES_ON_YOUR_LAWN) == aBass->mMusicMap.end())  // 如果尚未加载
		LoadSong(MusicFile::MUSIC_FILE_CREDITS_ZOMBIES_ON_YOUR_LAWN, "sounds\\ZombiesOnYourLawn.ogg");
//#endif
}

//0x45ABB0
void Music::StopAllMusic()
{
	if (mMusicInterface != nullptr)
	{
		if (mCurMusicFileMain != MusicFile::MUSIC_FILE_NONE)
			mMusicInterface->StopMusic(mCurMusicFileMain);
		if (mCurMusicFileDrums != MusicFile::MUSIC_FILE_NONE)
			mMusicInterface->StopMusic(mCurMusicFileDrums);
		if (mCurMusicFileHihats != MusicFile::MUSIC_FILE_NONE)
			mMusicInterface->StopMusic(mCurMusicFileHihats);
	}

	mCurMusicTune = MusicTune::MUSIC_TUNE_NONE;
	mCurMusicFileMain = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileDrums = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileHihats = MusicFile::MUSIC_FILE_NONE;
	mQueuedDrumTrackPackedOrder = -1;
	mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_OFF;
	mMusicBurstState = MusicBurstState::MUSIC_BURST_OFF;
	mPauseOffset = 0;
	mPauseOffsetDrums = 0;
	mPaused = false;
	mFadeOutCounter = 0;
}

//0x45AC20
HMUSIC Music::GetBassMusicHandle(MusicFile theMusicFile)
{
	BassMusicInterface* aBass = (BassMusicInterface*)mApp->mMusicInterface;
	auto anItr = aBass->mMusicMap.find((int)theMusicFile);
	TOD_ASSERT(anItr != aBass->mMusicMap.end());
	return anItr->second.mHMusic;
}

//0x45AC70
void Music::PlayFromOffset(MusicFile theMusicFile, int theOffset, double theVolume)
{
	if (mApp == nullptr) mApp = (LawnApp*)gSexyApp;
	BassMusicInterface* aBass = (BassMusicInterface*)mApp->mMusicInterface;
	auto anItr = aBass->mMusicMap.find((int)theMusicFile);
	TOD_ASSERT(anItr != aBass->mMusicMap.end());
	BassMusicInfo* aMusicInfo = &anItr->second;

	if (aMusicInfo->mHStream)
	{
		bool aNoLoop = theMusicFile == MusicFile::MUSIC_FILE_CREDITS_ZOMBIES_ON_YOUR_LAWN;  // MV 音乐不循环
		double oldCap = aMusicInfo->mVolumeCap;
		aMusicInfo->mVolumeCap = theVolume;
		mMusicInterface->PlayMusic(theMusicFile, theOffset, aNoLoop);
		aMusicInfo->mVolumeCap = oldCap;
	}
	else
	{
		gBass->BASS_ChannelStop(aMusicInfo->mHMusic);  // 先停止正在播放的音乐
		SetupMusicFileForTune(theMusicFile, mCurMusicTune);  // 调整每条轨道的静音与否
		aMusicInfo->mStopOnFade = false;
		aMusicInfo->mVolume = aMusicInfo->mVolumeCap * theVolume;
		aMusicInfo->mVolumeAdd = 0.0;

		gBass->BASS_ChannelSetAttribute(aMusicInfo->mHMusic, BASS_ATTRIB_VOL, aMusicInfo->mVolume);
		gBass->BASS_ChannelFlags(aMusicInfo->mHMusic, BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMPS | BASS_MUSIC_SURROUND | BASS_MUSIC_SURROUND2 | BASS_MUSIC_SINCINTER | BASS_MUSIC_LOOP, -1);
		gBass->BASS_ChannelSetPosition(aMusicInfo->mHMusic, theOffset /**/, BASS_POS_MUSIC_ORDER);  // 设置偏移位置
		gBass->BASS_ChannelPlay(aMusicInfo->mHMusic, false);  // 重新开始播放
	}
}

//0x45ADB0
void Music::PlayMusic(MusicTune theMusicTune, int theOffset, int theDrumsOffset)
{
	if (mMusicDisabled)
		return;

	mCurMusicTune = theMusicTune;
	mCurMusicFileMain = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileDrums = MusicFile::MUSIC_FILE_NONE;
	mCurMusicFileHihats = MusicFile::MUSIC_FILE_NONE;
	bool aRestartingSong = theOffset != -1;

	switch (theMusicTune)
	{
		case MusicTune::MUSIC_TUNE_DAY_GRASSWALK:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_DRUMS;
		mCurMusicFileHihats = MusicFile::MUSIC_FILE_HIHATS;
		if (theOffset == -1)
			theOffset = 0;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theOffset, 0.0);
		PlayFromOffset(mCurMusicFileHihats, theOffset, 0.0);
		break;

	case MusicTune::MUSIC_TUNE_NIGHT_MOONGRAINS:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_DRUMS;
		if (theOffset == -1)
		{
			theOffset = 0x30;
			theDrumsOffset = 0x5C;
		}
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theDrumsOffset, 0.0);
		break;

	case MusicTune::MUSIC_TUNE_POOL_WATERYGRAVES:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_DRUMS;
		mCurMusicFileHihats = MusicFile::MUSIC_FILE_HIHATS;
		if (theOffset == -1)
			theOffset = 0x5E;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theOffset, 0.0);
		PlayFromOffset(mCurMusicFileHihats, theOffset, 0.0);
		break;
	

	case MusicTune::MUSIC_TUNE_FOG_RIGORMORMIST:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_DRUMS;
		mCurMusicFileHihats = MusicFile::MUSIC_FILE_HIHATS;
		if (theOffset == -1)
			theOffset = 0x7D;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theOffset, 0.0);
		PlayFromOffset(mCurMusicFileHihats, theOffset, 0.0);
		break;

	case MusicTune::MUSIC_TUNE_ROOF_GRAZETHEROOF:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_DRUMS;
		mCurMusicFileHihats = MusicFile::MUSIC_FILE_HIHATS;
		if (theOffset == -1)
			theOffset = 0xB8;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theOffset, 0.0);
		PlayFromOffset(mCurMusicFileHihats, theOffset, 0.0);
		break;

	case MusicTune::MUSIC_TUNE_CHOOSE_YOUR_SEEDS:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0x7A;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0x98;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_ZEN_GARDEN:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0xDD;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_PUZZLE_CEREBRAWL:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0xB1;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_MINIGAME_LOONBOON:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0xA6;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_CONVEYER:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0xD4;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_FINAL_BOSS_BRAINIAC_MANIAC:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_MAIN_MUSIC;
		if (theOffset == -1)
			theOffset = 0x9E;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	case MusicTune::MUSIC_TUNE_CREDITS_ZOMBIES_ON_YOUR_LAWN:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_CREDITS_ZOMBIES_ON_YOUR_LAWN;
		if (theOffset == -1)
			theOffset = 0;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;

	/*case MusicTune::MUSIC_TUNE_GRASS_THE_MOON:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_GRASS_THE_MOON;
		mCurMusicFileDrums = MusicFile::MUSIC_FILE_GRASS_THE_MOON_DRUMS;
		mCurMusicFileHihats = MusicFile::MUSIC_FILE_GRASS_THE_MOON_HIHATS;
		if (theOffset == -1)
			theOffset = 0;
		mBaseBPM = 1;
		mBaseModSpeed = 1;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		PlayFromOffset(mCurMusicFileDrums, theOffset, 0.0);
		PlayFromOffset(mCurMusicFileHihats, theOffset, 0.0);
		break;

	case MusicTune::MUSIC_TUNE_FOG_PARTY:
		mCurMusicFileMain = MusicFile::MUSIC_FILE_FOG_PARTY;
		if (theOffset == -1)
			theOffset = 0;
		PlayFromOffset(mCurMusicFileMain, theOffset, 1.0);
		break;*/

	default:
		TOD_ASSERT(false);
		break;
	}

	/*if (mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON)	return;*/

	if (aRestartingSong)
	{
		if (mCurMusicFileMain != MusicFile::MUSIC_FILE_NONE)
		{
			HMUSIC aHMusic = GetBassMusicHandle(mCurMusicFileMain);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_BPM, mBaseBPM);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_SPEED, mBaseModSpeed);
		}
		if (mCurMusicFileDrums != -1)
		{
			HMUSIC aHMusic = GetBassMusicHandle(mCurMusicFileDrums);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_BPM, mBaseBPM);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_SPEED, mBaseModSpeed);
		}
		if (mCurMusicFileHihats != -1)
		{
			HMUSIC aHMusic = GetBassMusicHandle(mCurMusicFileHihats);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_BPM, mBaseBPM);
			gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_SPEED, mBaseModSpeed);
		}
	}
	else
	{
		HMUSIC aHMusic = GetBassMusicHandle(mCurMusicFileMain);
		gBass->BASS_ChannelGetAttribute(aHMusic, BASS_ATTRIB_MUSIC_BPM, &mBaseBPM);
		gBass->BASS_ChannelGetAttribute(aHMusic, BASS_ATTRIB_MUSIC_SPEED, &mBaseModSpeed);
	}
}

unsigned long Music::GetMusicOrder(MusicFile theMusicFile)
{
	TOD_ASSERT(theMusicFile != MusicFile::MUSIC_FILE_NONE);
	return ((BassMusicInterface*)mApp->mMusicInterface)->GetMusicOrder((int)theMusicFile);
}


//0x45B1B0
void Music::MusicResyncChannel(MusicFile theMusicFileToMatch, MusicFile theMusicFileToSync)
{
	unsigned int aPosToMatch = GetMusicOrder(theMusicFileToMatch);
	unsigned int aPosToSync = GetMusicOrder(theMusicFileToSync);
	int aDiff = (aPosToSync >> 16) - (aPosToMatch >> 16);  // 待同步的音乐与目标音乐的乐曲序号之差
	if (abs(aDiff) <= 128)  // 当前进行的乐曲序号之差超过 128 时，开摆（
	{
		HMUSIC aHMusic = GetBassMusicHandle(theMusicFileToSync);

		float aBPM = mBaseBPM;
		if (aDiff > 2)
			aBPM -= 2;
		else if (aDiff > 0)
			aBPM -= 1;
		else if (aDiff < -2)
			aBPM += 2;
		else if (aDiff < 0)
			aBPM += 1;

		gBass->BASS_ChannelSetAttribute(aHMusic, BASS_ATTRIB_MUSIC_BPM, aBPM);
	}
}

void Music::MusicResync()
{
	/*if (mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON) return;*/

	if (mCurMusicFileMain != MusicFile::MUSIC_FILE_NONE)
	{
		if (mCurMusicFileDrums != MusicFile::MUSIC_FILE_NONE)
			MusicResyncChannel(mCurMusicFileMain, mCurMusicFileDrums);
		if (mCurMusicFileHihats != MusicFile::MUSIC_FILE_NONE)
			MusicResyncChannel(mCurMusicFileMain, mCurMusicFileHihats);
	}
}

//0x45B240
void Music::StartBurst()
{ 
	if (mMusicBurstState == MusicBurstState::MUSIC_BURST_OFF)
	{ 
		mMusicBurstState = MusicBurstState::MUSIC_BURST_STARTING;
		mBurstStateCounter = 400;
	}
}

void Music::FadeOut(int theFadeOutDuration)
{ 
	if (mCurMusicTune != MusicTune::MUSIC_TUNE_NONE)
	{
		mFadeOutCounter = theFadeOutDuration;
		mFadeOutDuration = theFadeOutDuration;
	}
}

//0x45B260
void Music::UpdateMusicBurst()
{
	if (mApp->mBoard == nullptr)
		return;

	int aBurstScheme;
	if (mCurMusicTune == MusicTune::MUSIC_TUNE_DAY_GRASSWALK || mCurMusicTune == MusicTune::MUSIC_TUNE_POOL_WATERYGRAVES ||
		mCurMusicTune == MusicTune::MUSIC_TUNE_FOG_RIGORMORMIST || mCurMusicTune == MusicTune::MUSIC_TUNE_ROOF_GRAZETHEROOF /*|| 
		mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON*/)
		aBurstScheme = 1;
	else if (mCurMusicTune == MusicTune::MUSIC_TUNE_NIGHT_MOONGRAINS)
		aBurstScheme = 2;
	else
		return;

	int aPackedOrderMain = GetMusicOrder(mCurMusicFileMain);
	/*if (mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON)
		aPackedOrderMain = ((BassMusicInterface*)mApp->mMusicInterface)->GetChannelPosition((int)mCurMusicFileMain);
	else*/
	if (mBurstStateCounter > 0)
		mBurstStateCounter--;
	if (mDrumsStateCounter > 0)
		mDrumsStateCounter--;

	float aFadeTrackVolume = 0.0f;
	float aDrumsVolume = 0.0f;
	float aMainTrackVolume = 1.0f;
	switch (mMusicBurstState)
	{
		case MusicBurstState::MUSIC_BURST_OFF:
			if (mApp->mBoard->CountZombiesOnScreen() >= 10 && mApp->mGameMode != GameMode::GAMEMODE_INTRO || mBurstOverride == 1)
				StartBurst();
			break;
		case MusicBurstState::MUSIC_BURST_STARTING:
			if (aBurstScheme == 1)
			{
				aFadeTrackVolume = TodAnimateCurveFloat(400, 0, mBurstStateCounter, 0.0f, 1.0f, TodCurves::CURVE_LINEAR);
				if (mBurstStateCounter == 100)
				{
					mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_ON_QUEUED;
					mQueuedDrumTrackPackedOrder = aPackedOrderMain;
				}
				else if (mBurstStateCounter == 0)
				{
					mMusicBurstState = MusicBurstState::MUSIC_BURST_ON;
					mBurstStateCounter = 800;
				}
			}
			else if (aBurstScheme == 2)
			{
				if (mMusicDrumsState == MusicDrumsState::MUSIC_DRUMS_OFF)
				{
					mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_ON_QUEUED;
					mQueuedDrumTrackPackedOrder = aPackedOrderMain;
					mBurstStateCounter = 400;
				}
				else if (mMusicDrumsState == MusicDrumsState::MUSIC_DRUMS_ON_QUEUED)
					mBurstStateCounter = 400;
				else
				{
					aMainTrackVolume = TodAnimateCurveFloat(400, 0, mBurstStateCounter, 1.0f, 0.0f, TodCurves::CURVE_LINEAR);
					if (mBurstStateCounter == 0)
					{
						mMusicBurstState = MusicBurstState::MUSIC_BURST_ON;
						mBurstStateCounter = 800;
					}
				}
			}
			break;
		case MusicBurstState::MUSIC_BURST_ON:
			aFadeTrackVolume = 1.0f;
			if (aBurstScheme == 2)
				aMainTrackVolume = 0.0f;
			if (mBurstStateCounter == 0 && ((mApp->mBoard->CountZombiesOnScreen() < 4 && mBurstOverride == -1) || mBurstOverride == 2))
			{
				if (aBurstScheme == 1)
				{
					mMusicBurstState = MusicBurstState::MUSIC_BURST_FINISHING;
					mBurstStateCounter = 800;
					mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_OFF_QUEUED;
					mQueuedDrumTrackPackedOrder = aPackedOrderMain;
				}
				else if (aBurstScheme == 2)
				{
					mMusicBurstState = MusicBurstState::MUSIC_BURST_FINISHING;
					mBurstStateCounter = 1100;
					mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_FADING;
					mDrumsStateCounter = 800;
				}
			}
			break;
		case MusicBurstState::MUSIC_BURST_FINISHING:
			if (aBurstScheme == 1)
				aFadeTrackVolume = TodAnimateCurveFloat(800, 0, mBurstStateCounter, 1.0f, 0.0f, TodCurves::CURVE_LINEAR);
			else
				aMainTrackVolume = TodAnimateCurveFloat(400, 0, mBurstStateCounter, 0.0f, 1.0f, TodCurves::CURVE_LINEAR);
			if (mBurstStateCounter == 0 && mMusicDrumsState == MusicDrumsState::MUSIC_DRUMS_OFF)
				mMusicBurstState = MusicBurstState::MUSIC_BURST_OFF;
			break;
	}

	int aDrumsJumpOrder = -1;
	int aOrderMain = 0, aOrderDrum = 0;
	if (aBurstScheme == 1)
	{
		/*if (mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON)
		{
			aOrderMain = aPackedOrderMain / 128;
			aOrderDrum = mQueuedDrumTrackPackedOrder / 128;
		}
		else*/
		{
			aOrderMain = (int)round(((aPackedOrderMain >> 16) & 0xFFFF) / 128.0f);
			aOrderDrum = (int)round(((mQueuedDrumTrackPackedOrder >> 16) & 0xFFFF) / 128.0f);
		}
	}
	else if (aBurstScheme == 2)
	{
		aOrderMain = LOWORD(aPackedOrderMain);
		aOrderDrum = LOWORD(mQueuedDrumTrackPackedOrder);
		if (HIWORD(aPackedOrderMain >> 16) > 252)
			aOrderMain++;
		if (HIWORD(mQueuedDrumTrackPackedOrder >> 16) > 252)
			aOrderDrum++;
	}

	switch (mMusicDrumsState)
	{
		case MusicDrumsState::MUSIC_DRUMS_ON_QUEUED:
			if (aOrderMain != aOrderDrum)
			{
				aDrumsVolume = 1.0f; 
				mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_ON;
				if (aBurstScheme == 2)
					aDrumsJumpOrder = ((aOrderMain >> 16) % 2 == 0) ? 76 : 77;
			}
			break;
		case MusicDrumsState::MUSIC_DRUMS_ON:
			aDrumsVolume = 1.0f;
			break;
		case MusicDrumsState::MUSIC_DRUMS_OFF_QUEUED:
			aDrumsVolume = 1.0f;
			if ((aOrderMain != aOrderDrum) && aBurstScheme == 1)
			{
				mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_FADING;
				mDrumsStateCounter = 50;
			}
			break;
		case MusicDrumsState::MUSIC_DRUMS_FADING:
			if (aBurstScheme == 2)
				aDrumsVolume = TodAnimateCurveFloat(800, 0, mDrumsStateCounter, 1.0f, 0.0f, TodCurves::CURVE_LINEAR);
			else
				aDrumsVolume = TodAnimateCurveFloat(50, 0, mDrumsStateCounter, 1.0f, 0.0f, TodCurves::CURVE_LINEAR);
			if (mDrumsStateCounter == 0)
				mMusicDrumsState = MusicDrumsState::MUSIC_DRUMS_OFF;
			break;
	}

	if (aBurstScheme == 1)
	{
		mMusicInterface->SetSongVolume(mCurMusicFileHihats, aFadeTrackVolume);
		mMusicInterface->SetSongVolume(mCurMusicFileDrums, aDrumsVolume);
	}
	else if (aBurstScheme == 2)
	{
		mMusicInterface->SetSongVolume(mCurMusicFileMain, aMainTrackVolume);
		mMusicInterface->SetSongVolume(mCurMusicFileDrums, aDrumsVolume);
		if (aDrumsJumpOrder != -1)
			gBass->BASS_ChannelSetPosition(GetBassMusicHandle(mCurMusicFileDrums), LOWORD(aDrumsJumpOrder) /*| 0x80000000*/, BASS_POS_MUSIC_ORDER);
	}
}

//0x45B670
void Music::MusicUpdate()
{
	if (mFadeOutCounter > 0)
	{
		mFadeOutCounter--;
		if (mFadeOutCounter == 0)
		{
			StopAllMusic();
		}
		else if (mMusicInterface && mCurMusicFileMain)
		{
			float aFadeLevel = TodAnimateCurveFloat(mFadeOutDuration, 0, mFadeOutCounter, 1.0f, 0.0f, TodCurves::CURVE_LINEAR);
			mMusicInterface->SetSongVolume(mCurMusicFileMain, aFadeLevel);
		}
	}

	if (mApp && mApp->mBoard && (mApp->mBoard == nullptr || !mApp->mBoard->mPaused))
	{
		UpdateMusicBurst();
		MusicResync();
	}
}

//0x45B750
void Music::MakeSureMusicIsPlaying(MusicTune theMusicTune)
{
	if (mCurMusicTune != theMusicTune)
	{
		StopAllMusic();
		PlayMusic(theMusicTune, -1, -1);
	}
}

//0x45B770
void Music::StartGameMusic()
{
	TOD_ASSERT(mApp->mBoard);

	MusicTune mTune = MusicTune::MUSIC_TUNE_DAY_GRASSWALK;

	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN ||
		mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
	{
		mTune = MusicTune::MUSIC_TUNE_ZEN_GARDEN;
	}
	/*else if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_FOG_OF_WAR)
	{
		mTune = MusicTune::MUSIC_TUNE_FOG_PARTY;
	}*/
	else if (mApp->IsFinalBossLevel())
	{
		mTune = MusicTune::MUSIC_TUNE_FINAL_BOSS_BRAINIAC_MANIAC;
	}
	else if (mApp->IsWallnutBowlingLevel() || mApp->IsWhackAZombieLevel() ||
		mApp->IsLittleTroubleLevel() || mApp->IsBungeeBlitzLevel() ||
		mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_SPEED 
#ifdef _DS_MINIGAMES
		|| mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE
		|| mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP
#endif
		)
	{
		mTune = MusicTune::MUSIC_TUNE_MINIGAME_LOONBOON;
	}
	else if ((mApp->IsAdventureMode() &&
		(mApp->mBoard->mLevel == 10	||
		mApp->mBoard->mLevel == 20	||
		mApp->mBoard->mLevel == 30))	||
		mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_COLUMN)
	{
		mTune = MusicTune::MUSIC_TUNE_CONVEYER;
	}
	else if (mApp->IsStormyNightLevel())
	{
		StopAllMusic();
		return; 
	}
	else if (mApp->IsScaryPotterLevel() || mApp->IsIZombieLevel())
	{
		mTune = MusicTune::MUSIC_TUNE_PUZZLE_CEREBRAWL;
	}
	else if (mApp->mBoard->StageHasFog())
	{
		mTune = MusicTune::MUSIC_TUNE_FOG_RIGORMORMIST;
	}
	else if (mApp->mBoard->StageIsNight())
	{
		mTune = MusicTune::MUSIC_TUNE_NIGHT_MOONGRAINS;
	}
	else if (mApp->mBoard->StageHas6Rows())
	{
		mTune = MusicTune::MUSIC_TUNE_POOL_WATERYGRAVES;
	}
	else if (mApp->mBoard->StageHasRoof())
	{
		mTune = MusicTune::MUSIC_TUNE_ROOF_GRAZETHEROOF;
	}

	MakeSureMusicIsPlaying(mTune);
}

//0x45B930
void Music::GameMusicPause(bool thePause)
{
	if (thePause)
	{
		if (!mPaused && mCurMusicTune != MusicTune::MUSIC_TUNE_NONE && mMusicInterface != NULL)
		{
			BassMusicInterface* aBass = (BassMusicInterface*)mMusicInterface;
			auto anItr = aBass->mMusicMap.find(mCurMusicFileMain);
			TOD_ASSERT(anItr != aBass->mMusicMap.end());
			BassMusicInfo* aMusicInfo = &anItr->second;
			
			if (aMusicInfo->mHStream)
			{
				mPauseOffset = gBass->BASS_ChannelGetPosition(aMusicInfo->mHStream, BASS_POS_MUSIC_ORDER);
				mMusicInterface->StopMusic(mCurMusicFileMain);

				/*if (mCurMusicTune == MusicTune::MUSIC_TUNE_GRASS_THE_MOON) {
					mMusicInterface->StopMusic(mCurMusicFileDrums);
					mMusicInterface->StopMusic(mCurMusicFileHihats);
				}*/
			}
			else
			{
				unsigned long aOrderMain = GetMusicOrder(mCurMusicFileMain);
				mPauseOffset = ((aOrderMain & 0xffff) | ((int)round((aOrderMain >> 16) / 4.0f) << 16));
				mMusicInterface->StopMusic(mCurMusicFileMain);

				if (mCurMusicTune == MusicTune::MUSIC_TUNE_DAY_GRASSWALK || mCurMusicTune == MusicTune::MUSIC_TUNE_POOL_WATERYGRAVES ||
					mCurMusicTune == MusicTune::MUSIC_TUNE_FOG_RIGORMORMIST || mCurMusicTune == MusicTune::MUSIC_TUNE_ROOF_GRAZETHEROOF)
				{
					mMusicInterface->StopMusic(mCurMusicFileDrums);
					mMusicInterface->StopMusic(mCurMusicFileHihats);
				}
				else if (mCurMusicTune == MusicTune::MUSIC_TUNE_NIGHT_MOONGRAINS)
				{
					unsigned long aOrderDrums = GetMusicOrder(mCurMusicFileDrums);
					mPauseOffsetDrums = ((aOrderDrums & 0xffff) | ((int)round((aOrderDrums >> 16) / 4.0f) << 16));
					mMusicInterface->StopMusic(mCurMusicFileDrums);
				}
			}
			mPaused = true;
		}
	}
	else if (mPaused)
	{
		if (mCurMusicTune != MusicTune::MUSIC_TUNE_NONE)
			PlayMusic(mCurMusicTune, mPauseOffset, mPauseOffsetDrums);
		mPaused = false;
	}
}

int Music::GetNumLoadingTasks()
{
	//return 800 * 3;  // 内测版
	return 3500 * 2;  // 原版
}

