#include "Music.h"
#include "SaveGame.h"
#include "../Board.h"
#include "../Challenge.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "../CursorObject.h"
#include "../../Resources.h"
#include "../../ConstEnums.h"
#include "../MessageWidget.h"
#include "../../Sexy.TodLib/Trail.h"
#include <zlib.h>
#include "../../Sexy.TodLib/Attachment.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/TodParticle.h"
#include "../../Sexy.TodLib/EffectSystem.h"
#include "../../GameConstants.h"

#include <array>
#include <limits>
#include <memory>
#include <vector>

#ifdef _GOTY
static const char* FILE_COMPILE_TIME_STRING = "Dec 10 201014:56:46";
static const char* FILE_COMPILE_TIME_STRING2 = "Jul  2 201011:47:03";
#else
static const char* FILE_COMPILE_TIME_STRING = "Feb 16 200923:03:38";
#endif
// "Feb 16 200923:03:38"; // OG
// "Jul  2 201011:47:03"; // OLD GOTY
// "Dec 10 201014:56:46"; // GOTY Steam
static const unsigned int SAVE_FILE_MAGIC_NUMBER = 0xFEEDDEAD;
static const unsigned int SAVE_FILE_VERSION = 2U;
// This optional chunk follows the original end marker. Original executables do
// not require the buffer to end at that marker, so they continue to load saves
// written with this metadata. New builds treat a missing chunk as the legacy
// 800x600 board-local coordinate space.
static const unsigned int SAVE_COORDINATE_EXTENSION_MAGIC = 0x435A5650U; // "PVZC"
static const unsigned int SAVE_COORDINATE_EXTENSION_VERSION = 1U;
static const int SAVE_COORDINATE_EXTENSION_PAYLOAD_SIZE = 3 * sizeof(unsigned int);
static unsigned int SAVE_FILE_DATE = crc32(0, (Bytef*)FILE_COMPILE_TIME_STRING, strlen(FILE_COMPILE_TIME_STRING));  //[0x6AA7EC]
#ifdef _GOTY
static unsigned int SAVE_FILE_DATE2 = crc32(0, (Bytef*)FILE_COMPILE_TIME_STRING2, strlen(FILE_COMPILE_TIME_STRING2));
#endif

static bool IsSupportedSaveFileHeader(const SaveFileHeader& theHeader)
{
	if (theHeader.mMagicNumber != SAVE_FILE_MAGIC_NUMBER || theHeader.mBuildVersion != SAVE_FILE_VERSION)
		return false;

	if (theHeader.mBuildDate == SAVE_FILE_DATE)
		return true;
#ifdef _GOTY
	if (theHeader.mBuildDate == SAVE_FILE_DATE2)
		return true;
#endif
	return false;
}

enum class CoordinateExtensionReadStatus
{
	NOT_PRESENT,
	LOADED,
	MALFORMED,
	UNSUPPORTED
};

static SaveGameCoordinateMetadata GetLegacyCoordinateMetadata()
{
	SaveGameCoordinateMetadata aMetadata;
	aMetadata.mCoordinateSpace = SaveGameCoordinateSpace::BOARD_LOCAL_DESIGN_V1;
	aMetadata.mBoardWidth = BOARD_WIDTH;
	aMetadata.mBoardHeight = BOARD_HEIGHT;
	return aMetadata;
}

static void WriteCoordinateMetadata(SaveGameContext& theContext, const SaveGameCoordinateMetadata& theMetadata)
{
	theContext.mBuffer.WriteLong(SAVE_COORDINATE_EXTENSION_MAGIC);
	theContext.mBuffer.WriteLong(SAVE_COORDINATE_EXTENSION_VERSION);
	theContext.mBuffer.WriteLong(SAVE_COORDINATE_EXTENSION_PAYLOAD_SIZE);
	theContext.mBuffer.WriteLong((unsigned int)theMetadata.mCoordinateSpace);
	theContext.mBuffer.WriteLong(theMetadata.mBoardWidth);
	theContext.mBuffer.WriteLong(theMetadata.mBoardHeight);
}

static CoordinateExtensionReadStatus ReadCoordinateMetadata(
	SaveGameContext& theContext,
	SaveGameCoordinateMetadata& theMetadata)
{
	theMetadata = GetLegacyCoordinateMetadata();

	// A version-2 save from the original game ends immediately after SyncBoard's
	// magic marker. Unknown trailing data is left untouched for compatibility
	// with other forks that may already append their own extensions.
	if (theContext.ByteLeftToRead() < (int)sizeof(unsigned int))
		return CoordinateExtensionReadStatus::NOT_PRESENT;

	const int aSavedReadBitPos = theContext.mBuffer.mReadBitPos;
	const unsigned int aMagic = (unsigned int)theContext.mBuffer.ReadLong();
	if (aMagic != SAVE_COORDINATE_EXTENSION_MAGIC)
	{
		theContext.mBuffer.mReadBitPos = aSavedReadBitPos;
		return CoordinateExtensionReadStatus::NOT_PRESENT;
	}

	if (theContext.ByteLeftToRead() < 2 * (int)sizeof(unsigned int))
		return CoordinateExtensionReadStatus::MALFORMED;

	const unsigned int anExtensionVersion = (unsigned int)theContext.mBuffer.ReadLong();
	const int aPayloadSize = theContext.mBuffer.ReadLong();
	if (aPayloadSize < 0 || aPayloadSize > theContext.ByteLeftToRead())
		return CoordinateExtensionReadStatus::MALFORMED;

	const int aPayloadEndBitPos = theContext.mBuffer.mReadBitPos + aPayloadSize * 8;
	if (anExtensionVersion != SAVE_COORDINATE_EXTENSION_VERSION)
	{
		theContext.mBuffer.mReadBitPos = aPayloadEndBitPos;
		return CoordinateExtensionReadStatus::UNSUPPORTED;
	}

	if (aPayloadSize < SAVE_COORDINATE_EXTENSION_PAYLOAD_SIZE)
		return CoordinateExtensionReadStatus::MALFORMED;

	theMetadata.mCoordinateSpace = (SaveGameCoordinateSpace)(unsigned int)theContext.mBuffer.ReadLong();
	theMetadata.mBoardWidth = theContext.mBuffer.ReadLong();
	theMetadata.mBoardHeight = theContext.mBuffer.ReadLong();

	// Permit later versions to append fields without changing the v1 prefix.
	theContext.mBuffer.mReadBitPos = aPayloadEndBitPos;
	return CoordinateExtensionReadStatus::LOADED;
}

// The original serializer writes raw class tails, so an x64 save cannot be
// loaded by Win32. Earlier x64 fork builds nevertheless used the Win32 file
// name. The first raw block has a stored byte count that is deterministic for
// each ABI and can be checked without deserializing any pointers. The supported
// upstream layouts record 22431 bytes on Win32 and 22435 bytes on x64.
static const unsigned int LEGACY_X64_BOARD_BLOCK_SIZE = 22435U;

LegacySaveMigrationStatus TryMigrateLegacyX64Save(const SexyString& theLegacyFilePath, const SexyString& theX64FilePath)
{
#ifndef _WIN64
	(void)theLegacyFilePath;
	(void)theX64FilePath;
	return LegacySaveMigrationStatus::NOT_APPLICABLE;
#else
	// Feature-flagged/modded Board layouts are valid builds, but they must not
	// claim compatibility with saves from this known x64 raw layout.
	if (sizeof(Board) - offsetof(Board, mPaused) != LEGACY_X64_BOARD_BLOCK_SIZE)
		return LegacySaveMigrationStatus::NOT_APPLICABLE;

	// Never replace an x64-named save, even if another process creates it while
	// this probe is running.
#ifdef _USE_WIDE_STRING
	const DWORD aTargetAttributes = GetFileAttributesW(theX64FilePath.c_str());
#else
	const DWORD aTargetAttributes = GetFileAttributesA(theX64FilePath.c_str());
#endif
	if (aTargetAttributes != INVALID_FILE_ATTRIBUTES)
		return LegacySaveMigrationStatus::TARGET_PRESENT;

	const DWORD aTargetError = GetLastError();
	if (aTargetError != ERROR_FILE_NOT_FOUND && aTargetError != ERROR_PATH_NOT_FOUND)
	{
		TodTrace("Could not inspect architecture-specific save path: filesystem error %lu", aTargetError);
		return LegacySaveMigrationStatus::PROBE_FAILED;
	}

#ifdef _USE_WIDE_STRING
	HANDLE aFile = CreateFileW(theLegacyFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
	HANDLE aFile = CreateFileA(theLegacyFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#endif
	if (aFile == INVALID_HANDLE_VALUE)
	{
		const DWORD anError = GetLastError();
		if (anError == ERROR_FILE_NOT_FOUND || anError == ERROR_PATH_NOT_FOUND)
			return LegacySaveMigrationStatus::NOT_APPLICABLE;

		TodTrace("Could not inspect legacy unsuffixed save: filesystem error %lu", anError);
		return LegacySaveMigrationStatus::PROBE_FAILED;
	}

	unsigned int aProbe[5] = {};
	DWORD aBytesRead = 0;
	const BOOL aReadSucceeded = ReadFile(aFile, aProbe, sizeof(aProbe), &aBytesRead, nullptr);
	CloseHandle(aFile);

	if (!aReadSucceeded || aBytesRead != sizeof(aProbe))
	{
		TodTrace("Could not inspect complete legacy unsuffixed save header");
		return LegacySaveMigrationStatus::PROBE_FAILED;
	}

	SaveFileHeader aHeader = { aProbe[1], aProbe[2], aProbe[3] };
	if (aProbe[0] != sizeof(SaveFileHeader) || !IsSupportedSaveFileHeader(aHeader) ||
		aProbe[4] != LEGACY_X64_BOARD_BLOCK_SIZE)
	{
		return LegacySaveMigrationStatus::NOT_APPLICABLE;
	}

#ifdef _USE_WIDE_STRING
	const BOOL aMoved = MoveFileW(theLegacyFilePath.c_str(), theX64FilePath.c_str());
#else
	const BOOL aMoved = MoveFileA(theLegacyFilePath.c_str(), theX64FilePath.c_str());
#endif
	if (aMoved)
	{
		TodTrace("Migrated legacy unsuffixed x64 save");
		return LegacySaveMigrationStatus::MIGRATED;
	}

	const DWORD anError = GetLastError();
	if (anError == ERROR_FILE_EXISTS || anError == ERROR_ALREADY_EXISTS)
		return LegacySaveMigrationStatus::TARGET_PRESENT;
	if (anError == ERROR_FILE_NOT_FOUND || anError == ERROR_PATH_NOT_FOUND)
		return LegacySaveMigrationStatus::NOT_APPLICABLE;

	// The name is not part of the serialized data. If renaming is denied, the
	// caller can safely load this positively identified x64 save in place. The
	// next normal save writes the architecture-specific path while this source
	// remains untouched as a recovery copy.
	TodTrace("Could not rename legacy unsuffixed x64 save; using it in place: filesystem error %lu", anError);
	return LegacySaveMigrationStatus::USE_LEGACY_PATH;
#endif
}

static bool ConsumeUnsignedNumber(const SexyString& theText, SexyString::size_type& thePosition)
{
	const SexyString::size_type aStart = thePosition;
	while (thePosition < theText.length() && theText[thePosition] >= _S('0') && theText[thePosition] <= _S('9'))
		thePosition++;
	return thePosition > aStart;
}

static bool ConsumeLiteral(const SexyString& theText, SexyString::size_type& thePosition, const SexyString& theLiteral)
{
	if (theText.compare(thePosition, theLiteral.length(), theLiteral) != 0)
		return false;
	thePosition += theLiteral.length();
	return true;
}

static bool IsProfileSaveGameArtifactName(const SexyString& theFileName, const SexyString& theProfilePrefix)
{
	if (theFileName.compare(0, theProfilePrefix.length(), theProfilePrefix) != 0)
		return false;

	SexyString::size_type aPosition = theProfilePrefix.length();
	if (!ConsumeUnsignedNumber(theFileName, aPosition))
		return false;

	const SexyString aReplayMarker = _S("_replay_");
	if (theFileName.compare(aPosition, aReplayMarker.length(), aReplayMarker) == 0)
	{
		aPosition += aReplayMarker.length();
		if (!ConsumeUnsignedNumber(theFileName, aPosition))
			return false;
	}

	const SexyString anX64Marker = _S("_x64");
	if (theFileName.compare(aPosition, anX64Marker.length(), anX64Marker) == 0)
		aPosition += anX64Marker.length();

	if (!ConsumeLiteral(theFileName, aPosition, _S(".dat")))
		return false;
	if (aPosition == theFileName.length())
		return true;

	if (!ConsumeLiteral(theFileName, aPosition, _S(".rejected")))
		return false;
	if (aPosition == theFileName.length())
		return true;
	if (!ConsumeLiteral(theFileName, aPosition, _S(".")) || !ConsumeUnsignedNumber(theFileName, aPosition))
		return false;
	return aPosition == theFileName.length();
}

void DeleteSaveGameArtifactsForProfile(int theProfileId)
{
	const SexyString aUserDataFolder = GetAppDataFolder() + _S("userdata\\");
	const SexyString aProfilePrefix = StrFormat(_S("game%d_"), theProfileId);
	const SexyString aPattern = aUserDataFolder + aProfilePrefix + _S("*");

#ifdef _USE_WIDE_STRING
	WIN32_FIND_DATAW aFindData;
	HANDLE aFindHandle = FindFirstFileW(aPattern.c_str(), &aFindData);
#else
	WIN32_FIND_DATAA aFindData;
	HANDLE aFindHandle = FindFirstFileA(aPattern.c_str(), &aFindData);
#endif
	if (aFindHandle == INVALID_HANDLE_VALUE)
		return;

	do
	{
		const SexyString aFileName = aFindData.cFileName;
		if (!(aFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && IsProfileSaveGameArtifactName(aFileName, aProfilePrefix))
			gSexyAppBase->EraseFile(aUserDataFolder + aFileName);
#ifdef _USE_WIDE_STRING
	} while (FindNextFileW(aFindHandle, &aFindData));
#else
	} while (FindNextFileA(aFindHandle, &aFindData));
#endif

	FindClose(aFindHandle);
}

static bool QuarantineRejectedSave(const SexyString& theFilePath, const char* theReason)
{
	// PreNewGame replaces a save after a failed load. Move rejected data out of that
	// path first so an incompatible build or corrupt payload cannot destroy it.
	for (int aCopyIndex = 0; aCopyIndex < 1000; aCopyIndex++)
	{
		SexyString aRejectedPath = theFilePath + _S(".rejected");
		if (aCopyIndex > 0)
			aRejectedPath += StrFormat(_S(".%d"), aCopyIndex);

#ifdef _USE_WIDE_STRING
		const BOOL aMoved = MoveFileW(theFilePath.c_str(), aRejectedPath.c_str());
#else
		const BOOL aMoved = MoveFileA(theFilePath.c_str(), aRejectedPath.c_str());
#endif
		if (aMoved)
		{
			TodTrace("Preserved rejected save (%s)", theReason);
			return true;
		}

		const DWORD anError = GetLastError();
		if (anError == ERROR_FILE_EXISTS || anError == ERROR_ALREADY_EXISTS)
			continue;

		TodTrace("Could not preserve rejected save (%s): filesystem error %lu", theReason, anError);
		return false;
	}

	TodTrace("Could not preserve rejected save (%s): too many rejected copies", theReason);
	return false;
}

//0x4813D0
void SaveGameContext::SyncBytes(void* theDest, int theReadSize)
{
	int aReadSize = theReadSize;
	if (mReading)
	{
		if ((unsigned long)ByteLeftToRead() < 4)
		{
			mFailed = true;
		}

		aReadSize = mFailed ? 0 : mBuffer.ReadLong();
	}
	else
	{
		mBuffer.WriteLong(theReadSize);
	}

	if (mReading)
	{
		if (aReadSize != theReadSize || ByteLeftToRead() < theReadSize)
		{
			mFailed = true;
		}

		if (mFailed)
		{
			memset(theDest, 0, theReadSize);
		}
		else
		{
			mBuffer.ReadBytes((uchar*)theDest, theReadSize);
		}
	}
	else
	{
		mBuffer.WriteBytes((uchar*)theDest, theReadSize);
	}
}

//0x481470
void SaveGameContext::SyncInt(int& theInt)
{
	if (mReading)
	{
		if ((unsigned long)ByteLeftToRead() < 4)
		{
			mFailed = true;
		}

		theInt = mFailed ? 0 : mBuffer.ReadLong();
	}
	else
	{
		mBuffer.WriteLong(theInt);
	}
}

//0x4814C0
void SaveGameContext::SyncReanimationDef(ReanimatorDefinition*& theDefinition)
{
	if (mReading)
	{
		int aReanimType;
		SyncInt(aReanimType);
		if (aReanimType == (int)ReanimationType::REANIM_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aReanimType >= 0 && aReanimType < (int)ReanimationType::NUM_REANIMS)
		{
			ReanimatorEnsureDefinitionLoaded((ReanimationType)aReanimType, true);
			theDefinition = &gReanimatorDefArray[aReanimType];
		}
		else
		{
			theDefinition = nullptr;
			mFailed = true;
		}
	}
	else
	{
		int aReanimType = (int)ReanimationType::REANIM_NONE;
		for (int i = 0; i < (int)ReanimationType::NUM_REANIMS; i++)
		{
			ReanimatorDefinition* aDef = &gReanimatorDefArray[i];
			if (theDefinition == aDef)
			{
				aReanimType = i;
				break;
			}
		}
		SyncInt(aReanimType);
	}
}

//0x481560
void SaveGameContext::SyncParticleDef(TodParticleDefinition*& theDefinition)
{
	if (mReading)
	{
		int aParticleType;
		SyncInt(aParticleType);
		if (aParticleType == (int)ParticleEffect::PARTICLE_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aParticleType >= 0 && aParticleType < (int)ParticleEffect::NUM_PARTICLES)
		{
			theDefinition = &gParticleDefArray[aParticleType];
		}
		else
		{
			theDefinition = nullptr;
			mFailed = true;
		}
	}
	else
	{
		int aParticleType = (int)ParticleEffect::PARTICLE_NONE;
		for (int i = 0; i < (int)ParticleEffect::NUM_PARTICLES; i++)
		{
			TodParticleDefinition* aDef = &gParticleDefArray[i];
			if (theDefinition == aDef)
			{
				aParticleType = i;
				break;
			}
		}
		SyncInt(aParticleType);
	}
}

//0x4815F0
void SaveGameContext::SyncTrailDef(TrailDefinition*& theDefinition)
{
	if (mReading)
	{
		int aTrailType;
		SyncInt(aTrailType);
		if (aTrailType == TrailType::TRAIL_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aTrailType >= 0 && aTrailType < TrailType::NUM_TRAILS)
		{
			theDefinition = &gTrailDefArray[aTrailType];
		}
		else
		{
			theDefinition = nullptr;
			mFailed = true;
		}
	}
	else
	{
		int aTrailType = TrailType::TRAIL_NONE;
		for (int i = 0; i < TrailType::NUM_TRAILS; i++)
		{
			TrailDefinition* aDef = &gTrailDefArray[i];
			if (theDefinition == aDef)
			{
				aTrailType = i;
				break;
			}
		}
		SyncInt(aTrailType);
	}
}

//0x481690
void SaveGameContext::SyncImage(Image*& theImage)
{
	if (mReading)
	{
		ResourceId aResID;
		SyncInt((int&)aResID);
		if (mFailed)
		{
			theImage = nullptr;
			return;
		}
		if (aResID == Sexy::ResourceId::RESOURCE_ID_MAX)
		{
			theImage = nullptr;
		}
		else if ((int)aResID < 0 || aResID >= Sexy::ResourceId::RESOURCE_ID_MAX)
		{
			theImage = nullptr;
			mFailed = true;
		}
		else
		{
			theImage = GetImageById(aResID);
		}
	}
	else
	{
		ResourceId aResID;
		if (theImage != nullptr)
		{
			aResID = GetIdByImage(theImage);
		}
		else
		{
			aResID = Sexy::ResourceId::RESOURCE_ID_MAX;
		}
		SyncInt((int&)aResID);
	}
}

//0x481710
void SyncDataIDList(TodList<unsigned int>* theDataIDList, SaveGameContext& theContext, TodAllocator* theAllocator)
{
	try
	{
		if (theContext.mReading)
		{
			if (theDataIDList == nullptr || theAllocator == nullptr)
			{
				theContext.mFailed = true;
				return;
			}

			theDataIDList->mHead = nullptr;
			theDataIDList->mTail = nullptr;
			theDataIDList->mSize = 0;
			theDataIDList->SetAllocator(theAllocator);

			int aCount = 0;
			theContext.SyncInt(aCount);
			// Each ID is stored as a four-byte size prefix followed by four
			// bytes of payload. Validate before allocating list nodes.
			if (theContext.mFailed || aCount < 0 || aCount > DATA_ARRAY_MAX_SIZE ||
				aCount > theContext.ByteLeftToRead() / (2 * (int)sizeof(unsigned int)))
			{
				theContext.mFailed = true;
				return;
			}
			for (int i = 0; i < aCount; i++)
			{
				unsigned int aDataID = 0;
				theContext.SyncBytes(&aDataID, sizeof(aDataID));
				if (theContext.mFailed)
					return;
				theDataIDList->AddTail(aDataID);
			}
		}
		else
		{
			int aCount = theDataIDList->mSize;
			theContext.SyncInt(aCount);
			for (TodListNode<unsigned int>* aNode = theDataIDList->mHead; aNode != nullptr; aNode = aNode->mNext)
			{
				unsigned int aDataID = aNode->mValue;
				theContext.SyncBytes(&aDataID, sizeof(aDataID));
			}
		}
	}
	catch (std::exception&)
	{
		theContext.mFailed = true;
	}
}

//0x4817C0
void SyncParticleEmitter(TodParticleSystem* theParticleSystem, TodParticleEmitter* theParticleEmitter, SaveGameContext& theContext)
{
	int aEmitterDefIndex = 0;
	if (theContext.mReading)
	{
		theContext.SyncInt(aEmitterDefIndex);
		if (theContext.mFailed || theParticleSystem == nullptr || theParticleEmitter == nullptr ||
			theParticleSystem->mParticleDef == nullptr || theParticleSystem->mParticleHolder == nullptr ||
			aEmitterDefIndex < 0 ||
			aEmitterDefIndex >= theParticleSystem->mParticleDef->mEmitterDefCount)
		{
			theContext.mFailed = true;
			return;
		}
		theParticleEmitter->mParticleSystem = theParticleSystem;
		theParticleEmitter->mEmitterDef = &theParticleSystem->mParticleDef->mEmitterDefs[aEmitterDefIndex];
	}
	else
	{
		aEmitterDefIndex = static_cast<int>(theParticleEmitter->mEmitterDef - theParticleSystem->mParticleDef->mEmitterDefs);
		theContext.SyncInt(aEmitterDefIndex);
	}

	theContext.SyncImage(theParticleEmitter->mImageOverride);
	if (theContext.mFailed)
		return;
	SyncDataIDList((TodList<unsigned int>*) & theParticleEmitter->mParticleList, theContext, &theParticleSystem->mParticleHolder->mParticleListNodeAllocator);
	if (theContext.mFailed)
		return;
	for (TodListNode<ParticleID>* aNode = theParticleEmitter->mParticleList.mHead; aNode != nullptr; aNode = aNode->mNext)
	{
		TodParticle* aParticle = theParticleSystem->mParticleHolder->mParticles.DataArrayTryToGet((unsigned int)aNode->mValue);
		if (aParticle == nullptr)
		{
			theContext.mFailed = true;
			return;
		}
		if (theContext.mReading)
		{
			aParticle->mParticleEmitter = theParticleEmitter;
		}
	}
}

//0x481880
void SyncParticleSystem(Board* theBoard, TodParticleSystem* theParticleSystem, SaveGameContext& theContext)
{
	theContext.SyncParticleDef(theParticleSystem->mParticleDef);
	if (theContext.mFailed || theParticleSystem->mParticleDef == nullptr)
	{
		theContext.mFailed = true;
		return;
	}
	if (theContext.mReading)
	{
		theParticleSystem->mParticleHolder = theBoard->mApp->mEffectSystem->mParticleHolder;
	}
	if (theParticleSystem->mParticleHolder == nullptr)
	{
		theContext.mFailed = true;
		return;
	}

	SyncDataIDList((TodList<unsigned int>*) & theParticleSystem->mEmitterList, theContext, &theParticleSystem->mParticleHolder->mEmitterListNodeAllocator);
	if (theContext.mFailed)
		return;
	for (TodListNode<ParticleEmitterID>* aNode = theParticleSystem->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext)
	{
		TodParticleEmitter* aEmitter = theParticleSystem->mParticleHolder->mEmitters.DataArrayTryToGet((unsigned int)aNode->mValue);
		if (aEmitter == nullptr)
		{
			theContext.mFailed = true;
			return;
		}
		SyncParticleEmitter(theParticleSystem, aEmitter, theContext);
		if (theContext.mFailed)
			return;
	}
}

//0x4818F0
void SyncReanimation(Board* theBoard, Reanimation* theReanimation, SaveGameContext& theContext)
{
	if (theContext.mReading)
		// Never retain the serialized process-local allocation address. This also
		// makes a definition failure safe to unwind.
		theReanimation->mTrackInstances = nullptr;
	theContext.SyncReanimationDef(theReanimation->mDefinition);
	if (theContext.mFailed || theReanimation->mDefinition == nullptr)
	{
		theContext.mFailed = true;
		return;
	}
	if (theContext.mReading)
	{
		theReanimation->mReanimationHolder = theBoard->mApp->mEffectSystem->mReanimationHolder;
	}

	if (theReanimation->mDefinition->mTracks.count != 0)
	{
		const int aTrackCount = theReanimation->mDefinition->mTracks.count;
		if (aTrackCount < 0 || aTrackCount >
			(std::numeric_limits<int>::max)() / (int)sizeof(ReanimatorTrackInstance))
		{
			theContext.mFailed = true;
			return;
		}
		int aSize = aTrackCount * sizeof(ReanimatorTrackInstance);
		if (theContext.mReading)
		{
			theReanimation->mTrackInstances = (ReanimatorTrackInstance*)FindGlobalAllocator(aSize)->Calloc(aSize);
			if (theReanimation->mTrackInstances == nullptr)
			{
				theContext.mFailed = true;
				return;
			}
		}
		theContext.SyncBytes(theReanimation->mTrackInstances, aSize);
		if (theContext.mFailed)
			return;

		for (int aTrackIndex = 0; aTrackIndex < aTrackCount; aTrackIndex++)
		{
			ReanimatorTrackInstance& aTrackInstance = theReanimation->mTrackInstances[aTrackIndex];
			theContext.SyncImage(aTrackInstance.mImageOverride);
			if (theContext.mFailed)
				return;

			if (theContext.mReading)
			{
				aTrackInstance.mBlendTransform.mText = "";
				TOD_ASSERT(aTrackInstance.mBlendTransform.mFont == nullptr);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mImage == nullptr);
			}
			else
			{
				TOD_ASSERT(aTrackInstance.mBlendTransform.mText[0] == NULL);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mFont == nullptr);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mImage == nullptr);
			}
		}
	}
}

void SyncTrail(Board* theBoard, Trail* theTrail, SaveGameContext& theContext)
{
	theContext.SyncTrailDef(theTrail->mDefinition);
	if (theContext.mFailed)
		return;
	if (theContext.mReading)
	{
		theTrail->mTrailHolder = theBoard->mApp->mEffectSystem->mTrailHolder;
	}
}

template <typename T> static bool ValidateSerializedDataArrayBlock(
	const std::vector<unsigned char>& theBytes,
	unsigned int theFreeListHead,
	unsigned int theMaxUsedCount,
	unsigned int theSize)
{
	using DataArrayItem = typename DataArray<T>::DataArrayItem;
	auto GetItemId = [&](unsigned int theIndex)
	{
		unsigned int anId = 0;
		memcpy(
			&anId,
			theBytes.data() + (size_t)theIndex * sizeof(DataArrayItem) + offsetof(DataArrayItem, mID),
			sizeof(anId)
		);
		return anId;
	};

	unsigned int anActiveCount = 0;
	for (unsigned int i = 0; i < theMaxUsedCount; i++)
	{
		const unsigned int anId = GetItemId(i);
		if ((anId & DATA_ARRAY_KEY_MASK) != 0)
		{
			if ((anId & DATA_ARRAY_INDEX_MASK) != i)
				return false;
			anActiveCount++;
		}
	}
	if (anActiveCount != theSize)
		return false;

	std::vector<unsigned char> aVisited(theMaxUsedCount, 0);
	unsigned int aFreeCount = 0;
	unsigned int aFreeIndex = theFreeListHead;
	while (aFreeIndex != theMaxUsedCount)
	{
		if (aFreeIndex >= theMaxUsedCount || aVisited[aFreeIndex] != 0)
			return false;
		aVisited[aFreeIndex] = 1;
		const unsigned int aNextFreeIndex = GetItemId(aFreeIndex);
		if ((aNextFreeIndex & DATA_ARRAY_KEY_MASK) != 0 || aNextFreeIndex > theMaxUsedCount)
			return false;
		aFreeIndex = aNextFreeIndex;
		aFreeCount++;
	}

	return aFreeCount == theMaxUsedCount - theSize;
}

template <typename T> inline static void SyncDataArray(SaveGameContext& theContext, DataArray<T>& theDataArray)
{
	using DataArrayItem = typename DataArray<T>::DataArrayItem;
	if (!theContext.mReading)
	{
		theContext.SyncUint(theDataArray.mFreeListHead);
		theContext.SyncUint(theDataArray.mMaxUsedCount);
		theContext.SyncUint(theDataArray.mSize);
		theContext.SyncBytes(theDataArray.mBlock, theDataArray.mMaxUsedCount * sizeof(DataArrayItem));
		return;
	}

	unsigned int aFreeListHead = 0;
	unsigned int aMaxUsedCount = 0;
	unsigned int aSize = 0;
	theContext.SyncUint(aFreeListHead);
	theContext.SyncUint(aMaxUsedCount);
	theContext.SyncUint(aSize);
	if (theContext.mFailed || aMaxUsedCount > theDataArray.mMaxSize ||
		aSize > aMaxUsedCount || aFreeListHead > aMaxUsedCount ||
		aMaxUsedCount > (unsigned int)(std::numeric_limits<int>::max)() / sizeof(DataArrayItem))
	{
		theContext.mFailed = true;
		return;
	}

	const int aByteCount = (int)(aMaxUsedCount * sizeof(DataArrayItem));
	std::vector<unsigned char> aSerializedBytes((size_t)aByteCount);
	unsigned char aZeroByte = 0;
	theContext.SyncBytes(aByteCount == 0 ? &aZeroByte : aSerializedBytes.data(), aByteCount);
	if (theContext.mFailed || !ValidateSerializedDataArrayBlock<T>(
		aSerializedBytes, aFreeListHead, aMaxUsedCount, aSize))
	{
		theContext.mFailed = true;
		return;
	}

	if (aByteCount != 0)
		memcpy(theDataArray.mBlock, aSerializedBytes.data(), (size_t)aByteCount);
	theDataArray.mFreeListHead = aFreeListHead;
	theDataArray.mMaxUsedCount = aMaxUsedCount;
	theDataArray.mSize = aSize;
}

template <typename T> static void AbandonRejectedDataArray(DataArray<T>& theDataArray)
{
	// The load target is a freshly constructed board with empty arrays. If a
	// payload fails after raw object bytes have been copied, do not run object
	// destructors over half-deserialized pointers. The backing allocation remains
	// owned by the holder and is safely reused or released later.
	theDataArray.mFreeListHead = 0;
	theDataArray.mMaxUsedCount = 0;
	theDataArray.mSize = 0;
}

static void AbandonRejectedSerializedArrays(Board* theBoard)
{
	AbandonRejectedDataArray(theBoard->mZombies);
	AbandonRejectedDataArray(theBoard->mPlants);
	AbandonRejectedDataArray(theBoard->mProjectiles);
	AbandonRejectedDataArray(theBoard->mCoins);
	AbandonRejectedDataArray(theBoard->mLawnMowers);
	AbandonRejectedDataArray(theBoard->mGridItems);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mParticleHolder->mEmitters);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mParticleHolder->mParticles);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mTrailHolder->mTrails);
	AbandonRejectedDataArray(theBoard->mApp->mEffectSystem->mAttachmentHolder->mAttachments);
}

static void PrepareSerializedEffectObjectsForLoad(Board* theBoard)
{
	TodParticleHolder* aParticleHolder = theBoard->mApp->mEffectSystem->mParticleHolder;
	TodParticleSystem* aParticleSystem = nullptr;
	while (aParticleHolder->mParticleSystems.IterateNext(aParticleSystem))
	{
		aParticleSystem->mEmitterList.mHead = nullptr;
		aParticleSystem->mEmitterList.mTail = nullptr;
		aParticleSystem->mEmitterList.mSize = 0;
		aParticleSystem->mEmitterList.SetAllocator(&aParticleHolder->mEmitterListNodeAllocator);
	}

	TodParticleEmitter* anEmitter = nullptr;
	while (aParticleHolder->mEmitters.IterateNext(anEmitter))
	{
		anEmitter->mParticleList.mHead = nullptr;
		anEmitter->mParticleList.mTail = nullptr;
		anEmitter->mParticleList.mSize = 0;
		anEmitter->mParticleList.SetAllocator(&aParticleHolder->mParticleListNodeAllocator);
	}

	Reanimation* aReanimation = nullptr;
	while (theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations.IterateNext(aReanimation))
		aReanimation->mTrackInstances = nullptr;
}

static void ReleaseRejectedEffectLoadAllocations(Board* theBoard)
{
	TodParticleHolder* aParticleHolder = theBoard->mApp->mEffectSystem->mParticleHolder;
	TodParticleEmitter* anEmitter = nullptr;
	while (aParticleHolder->mEmitters.IterateNext(anEmitter))
		anEmitter->mParticleList.RemoveAll();

	TodParticleSystem* aParticleSystem = nullptr;
	while (aParticleHolder->mParticleSystems.IterateNext(aParticleSystem))
		aParticleSystem->mEmitterList.RemoveAll();

	Reanimation* aReanimation = nullptr;
	while (theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations.IterateNext(aReanimation))
	{
		if (aReanimation->mTrackInstances == nullptr)
			continue;

		ReanimatorDefinition* aDefinition = nullptr;
		for (unsigned int i = 0; i < gReanimatorDefCount; i++)
		{
			if (aReanimation->mDefinition == &gReanimatorDefArray[i])
			{
				aDefinition = aReanimation->mDefinition;
				break;
			}
		}
		if (aDefinition != nullptr && aDefinition->mTracks.count > 0 &&
			aDefinition->mTracks.count <=
				(std::numeric_limits<int>::max)() / (int)sizeof(ReanimatorTrackInstance))
		{
			const int aSize = aDefinition->mTracks.count * sizeof(ReanimatorTrackInstance);
			FindGlobalAllocator(aSize)->Free(aReanimation->mTrackInstances, aSize);
		}
		aReanimation->mTrackInstances = nullptr;
	}
}

//0x4819D0
void SyncBoard(SaveGameContext& theContext, Board* theBoard)
{
	theContext.SyncBytes(&theBoard->mPaused, sizeof(Board) - offsetof(Board, mPaused));
	if (theContext.mFailed)
		return;

	SyncDataArray(theContext, theBoard->mZombies);													//0x482190
	SyncDataArray(theContext, theBoard->mPlants);													//0x482280
	SyncDataArray(theContext, theBoard->mProjectiles);												//0x482370
	SyncDataArray(theContext, theBoard->mCoins);													//0x482460
	SyncDataArray(theContext, theBoard->mLawnMowers);												//0x482550
	SyncDataArray(theContext, theBoard->mGridItems);												//0x482650
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems);	//0x482740
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mParticleHolder->mEmitters);			//0x482830
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mParticleHolder->mParticles);			//0x482920
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations);	//0x482920
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mTrailHolder->mTrails);				//0x482650
	SyncDataArray(theContext, theBoard->mApp->mEffectSystem->mAttachmentHolder->mAttachments);		//0x482A10
	if (theContext.mFailed)
		return;
	if (theContext.mReading)
		PrepareSerializedEffectObjectsForLoad(theBoard);
	auto AbortPreparedEffectLoad = [&]()
	{
		if (theContext.mReading)
			ReleaseRejectedEffectLoadAllocations(theBoard);
	};

	{
		TodParticleSystem* aParticle = nullptr;
		while (theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems.IterateNext(aParticle))
		{
			SyncParticleSystem(theBoard, aParticle, theContext);
			if (theContext.mFailed)
			{
				AbortPreparedEffectLoad();
				return;
			}
		}
	}
	{
		Reanimation* aReanimation = nullptr;
		while (theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations.IterateNext(aReanimation))
		{
			SyncReanimation(theBoard, aReanimation, theContext);
			if (theContext.mFailed)
			{
				AbortPreparedEffectLoad();
				return;
			}
		}
	}
	{
		Trail* aTrail = nullptr;
		while (theBoard->mApp->mEffectSystem->mTrailHolder->mTrails.IterateNext(aTrail))
		{
			SyncTrail(theBoard, aTrail, theContext);
			if (theContext.mFailed)
			{
				AbortPreparedEffectLoad();
				return;
			}
		}
	}

	theContext.SyncBytes(theBoard->mCursorObject, sizeof(CursorObject));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}
	theContext.SyncBytes(theBoard->mCursorPreview, sizeof(CursorPreview));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}
	theContext.SyncBytes(theBoard->mAdvice, sizeof(MessageWidget));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}
	theContext.SyncBytes(theBoard->mSeedBank, sizeof(SeedBank));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}
	theContext.SyncBytes(theBoard->mChallenge, sizeof(Challenge));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}
	theContext.SyncBytes(theBoard->mApp->mMusic, sizeof(Music));
	if (theContext.mFailed)
	{
		AbortPreparedEffectLoad();
		return;
	}

	if (theContext.mReading)
	{
		if ((unsigned long)theContext.ByteLeftToRead() < 4)
		{
			theContext.mFailed = true;
		}

		if (theContext.mFailed || theContext.mBuffer.ReadLong() != SAVE_FILE_MAGIC_NUMBER)
		{
			theContext.mFailed = true;
		}
	}
	else
	{
		theContext.mBuffer.WriteLong(SAVE_FILE_MAGIC_NUMBER);
	}
	if (theContext.mFailed)
		AbortPreparedEffectLoad();
}

//0x481CE0
void FixBoardAfterLoad(Board* theBoard)
{
	{
		Plant* aPlant = nullptr;
		while (theBoard->mPlants.IterateNext(aPlant))
		{
			aPlant->mApp = theBoard->mApp;
			aPlant->mBoard = theBoard;
		}
	}
	{
		Zombie* aZombie = nullptr;
		while (theBoard->mZombies.IterateNext(aZombie))
		{
			aZombie->mApp = theBoard->mApp;
			aZombie->mBoard = theBoard;
		}
	}
	{
		Projectile* aProjectile = nullptr;
		while (theBoard->mProjectiles.IterateNext(aProjectile))
		{
			aProjectile->mApp = theBoard->mApp;
			aProjectile->mBoard = theBoard;
		}
	}
	{
		Coin* aCoin = nullptr;
		while (theBoard->mCoins.IterateNext(aCoin))
		{
			aCoin->mApp = theBoard->mApp;
			aCoin->mBoard = theBoard;
		}
	}
	{
		LawnMower* aLawnMower = nullptr;
		while (theBoard->mLawnMowers.IterateNext(aLawnMower))
		{
			aLawnMower->mApp = theBoard->mApp;
			aLawnMower->mBoard = theBoard;
		}
	}
	{
		GridItem* aGridItem = nullptr;
		while (theBoard->mGridItems.IterateNext(aGridItem))
		{
			aGridItem->mApp = theBoard->mApp;
			aGridItem->mBoard = theBoard;
		}
	}

	theBoard->mAdvice->mApp = theBoard->mApp;
	theBoard->mCursorObject->mApp = theBoard->mApp;
	theBoard->mCursorObject->mBoard = theBoard;
	theBoard->mCursorPreview->mApp = theBoard->mApp;
	theBoard->mCursorPreview->mBoard = theBoard;
	theBoard->mSeedBank->mApp = theBoard->mApp;
	theBoard->mSeedBank->mBoard = theBoard;
	for (int i = 0; i < SEEDBANK_MAX; i++)
	{
		theBoard->mSeedBank->mSeedPackets[i].mApp = theBoard->mApp;
		theBoard->mSeedBank->mSeedPackets[i].mBoard = theBoard;
	}
	theBoard->mChallenge->mApp = theBoard->mApp;
	theBoard->mChallenge->mBoard = theBoard;
	theBoard->mApp->mMusic->mApp = theBoard->mApp;
	theBoard->mApp->mMusic->mMusicInterface = theBoard->mApp->mMusicInterface;
}

bool MigrateBoardCoordinatesAfterLoad(Board* theBoard, const SaveGameCoordinateMetadata& theMetadata)
{
	if (theBoard == nullptr)
		return false;

	switch (theMetadata.mCoordinateSpace)
	{
	case SaveGameCoordinateSpace::BOARD_LOCAL_DESIGN_V1:
		// The raw Board tail, entity DataArrays, effects, cursor state and
		// challenge state all use the same stable board-local design units.
		// Presentation/canvas dimensions are deliberately not serialized. As a
		// result, legacy and current saves require no numeric remapping while the
		// viewport is free to change size and aspect ratio.
		if (theMetadata.mBoardWidth == BOARD_WIDTH && theMetadata.mBoardHeight == BOARD_HEIGHT)
			return true;

		TodTrace("Unsupported board-local save dimensions: %d x %d",
			theMetadata.mBoardWidth, theMetadata.mBoardHeight);
		return false;

	default:
		// A future simulation-space change must add an explicit migration here.
		// Never infer a transform from the current window size: doing so would
		// move grid entities and corrupt collision/gameplay state.
		TodTrace("Unsupported save coordinate space: %u", (unsigned int)theMetadata.mCoordinateSpace);
		return false;
	}
}

//0x481FE0
SaveGameLoadStatus LawnLoadGame(Board* theBoard, const SexyString& theFilePath)
{
	SaveGameContext aContext;
	aContext.mFailed = false;
	aContext.mReading = true;
	if (!gSexyAppBase->ReadBufferFromFile(theFilePath, &aContext.mBuffer, false))
	{
		return QuarantineRejectedSave(theFilePath, "read failure")
			? SaveGameLoadStatus::REJECTED_PRESERVED
			: SaveGameLoadStatus::REJECTED_UNPRESERVED;
	}

	SaveFileHeader aHeader;
	aContext.SyncBytes(&aHeader, sizeof(aHeader));
	if (!IsSupportedSaveFileHeader(aHeader))
	{
		return QuarantineRejectedSave(theFilePath, "incompatible header")
			? SaveGameLoadStatus::REJECTED_PRESERVED
			: SaveGameLoadStatus::REJECTED_UNPRESERVED;
	}

	// Music belongs to LawnApp rather than the temporary Board used for loading.
	// Restore it whenever the payload or an extension is rejected so a failed
	// save cannot leak serialized playback state into the next game.
	std::array<unsigned char, sizeof(Music)> aMusicSnapshot;
	memcpy(aMusicSnapshot.data(), theBoard->mApp->mMusic, sizeof(Music));
	auto RejectLoadedSave = [&](const char* theReason)
	{
		memcpy(theBoard->mApp->mMusic, aMusicSnapshot.data(), sizeof(Music));
		return QuarantineRejectedSave(theFilePath, theReason)
			? SaveGameLoadStatus::REJECTED_PRESERVED
			: SaveGameLoadStatus::REJECTED_UNPRESERVED;
	};

	SyncBoard(aContext, theBoard);
	if (aContext.mFailed)
	{
		AbandonRejectedSerializedArrays(theBoard);
	}

	// SyncBoard restores raw object bytes, including runtime-only pointers. Repair
	// those links even after a payload failure so that the temporary board can
	// always be torn down safely.
	FixBoardAfterLoad(theBoard);
	if (aContext.mFailed)
		return RejectLoadedSave("corrupt or incompatible payload");

	SaveGameCoordinateMetadata aCoordinateMetadata;
	const CoordinateExtensionReadStatus aCoordinateStatus = ReadCoordinateMetadata(aContext, aCoordinateMetadata);
	if (aCoordinateStatus == CoordinateExtensionReadStatus::MALFORMED)
	{
		return RejectLoadedSave("malformed coordinate metadata");
	}
	if (aCoordinateStatus == CoordinateExtensionReadStatus::UNSUPPORTED)
	{
		return RejectLoadedSave("unsupported coordinate metadata version");
	}

	if (!MigrateBoardCoordinatesAfterLoad(theBoard, aCoordinateMetadata))
	{
		return RejectLoadedSave("unsupported coordinate space");
	}

	TodTrace("Loaded save game");
	theBoard->mApp->mGameScene = GameScenes::SCENE_PLAYING;
	return SaveGameLoadStatus::LOADED;
}

//0x4820D0
bool LawnSaveGame(Board* theBoard, const SexyString& theFilePath)
{
	SaveGameContext aContext;
	aContext.mFailed = false;
	aContext.mReading = false;

	SaveFileHeader aHeader;
	aHeader.mMagicNumber = SAVE_FILE_MAGIC_NUMBER;
	aHeader.mBuildVersion = SAVE_FILE_VERSION;
	aHeader.mBuildDate = SAVE_FILE_DATE;

	aContext.SyncBytes(&aHeader, sizeof(aHeader));
	SyncBoard(aContext, theBoard);
	if (aContext.mFailed)
	{
		TodTrace("Refusing to write an incomplete save game");
		return false;
	}
	WriteCoordinateMetadata(aContext, GetLegacyCoordinateMetadata());
	return gSexyAppBase->WriteBufferToFile(theFilePath, &aContext.mBuffer);
}
