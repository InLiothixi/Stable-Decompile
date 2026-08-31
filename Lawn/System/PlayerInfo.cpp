#include "DataSync.h"
#include "PlayerInfo.h"
#include "SaveGame.h"
#include "../LawnCommon.h"
#include "../Widget/ChallengeScreen.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/Buffer.h"
#include "../../SexyAppFramework/SexyAppBase.h"
#include "../Widget/AchievementsScreen.h"
#include "../Plant.h"
#include "../../GameConstants.h"

static int gUserVersion = 12;

PlayerInfo::PlayerInfo()
{
	Reset();
}

#define SafeSync(function) \
	try { \
		function; \
	} \
	catch (...) { \
		/*Do Nothing*/\
	} \

//0x468310
void PlayerInfo::SyncSummary(DataSync& theSync)
{
	SafeSync(theSync.SyncString(mName));
	SafeSync(theSync.SyncLong(mUseSeq));
	SafeSync(theSync.SyncLong(mId));
}

//0x468390
void PlayerInfo::SyncDetails(DataSync& theSync)
{
	if (theSync.GetReader())
	{
		Reset();
	}

	int aVersion = gUserVersion;
	SafeSync(theSync.SyncLong(aVersion));
	SafeSync(theSync.SetVersion(aVersion));
	if (aVersion != gUserVersion)
	{
		return;
	}

	SafeSync(theSync.SyncLong(mLevel));
	SafeSync(theSync.SyncLong(mCoins));
	SafeSync(theSync.SyncLong(mFinishedAdventure));
	for (int i = 0; i < 100; i++)
	{
		SafeSync(theSync.SyncLong(mChallengeRecords[i]));
	}
	for (int i = 0; i < 80; i++)
	{
		SafeSync(theSync.SyncLong(mPurchases[i]));
	}
	SafeSync(theSync.SyncLong(mPlayTimeActivePlayer));
	SafeSync(theSync.SyncLong(mPlayTimeInactivePlayer));
	SafeSync(theSync.SyncLong(mHasUsedCheatKeys));
	SafeSync(theSync.SyncLong(mHasWokenStinky));
	SafeSync(theSync.SyncLong(mDidntPurchasePacketUpgrade));
	SafeSync(theSync.SyncLong(mLastStinkyChocolateTime));
	SafeSync(theSync.SyncLong(mStinkyPosX));
	SafeSync(theSync.SyncLong(mStinkyPosY));
	SafeSync(theSync.SyncLong(mHasUnlockedMinigames));
	SafeSync(theSync.SyncLong(mHasUnlockedPuzzleMode));
	SafeSync(theSync.SyncLong(mHasNewMiniGame));
	SafeSync(theSync.SyncLong(mHasNewScaryPotter));
	SafeSync(theSync.SyncLong(mHasNewIZombie));
	SafeSync(theSync.SyncLong(mHasNewSurvival));
	SafeSync(theSync.SyncLong(mHasUnlockedSurvivalMode));
	SafeSync(theSync.SyncLong(mNeedsMessageOnGameSelector));
	SafeSync(theSync.SyncLong(mNeedsMagicTacoReward));
	SafeSync(theSync.SyncLong(mHasSeenStinky));
	SafeSync(theSync.SyncLong(mHasSeenUpsell));
	SafeSync(theSync.SyncLong(mEarnedFromZenGarden);)
	SafeSync(theSync.SyncLong(mNumPottedPlants));
	
	TOD_ASSERT(mNumPottedPlants <= MAX_POTTED_PLANTS);
	for (int i = 0; i < mNumPottedPlants; i++)
	{
		SafeSync(theSync.SyncBytes(&mPottedPlant[i], sizeof(PottedPlant)));
	}

	// @Patoke: implemented
	for (int i = 0; i < (int)AchievementId::MAX_ACHIEVEMENTS; i++)
	{
		SafeSync(theSync.SyncBool(mEarnedAchievements[i]));
	}

	for (int i = 0; i < (int)AchievementId::MAX_ACHIEVEMENTS; i++)
	{
		SafeSync(theSync.SyncBool(mShownAchievements[i]));
	}

	for (int i = 0; i < (int)SeedType::SEED_LEFTPEATER; i++)
	{
		SafeSync(theSync.SyncBool(mPlantedPlants[i]));
	}
#ifdef _HAS_ZOMBATAR
	SafeSync(theSync.SyncBool(mAckZombatarTOS));
#endif
	SafeSync(theSync.SyncBool(mDidRIPMode));
	SafeSync(theSync.SyncLong(mRIPLevel));
}

//0x469400
void PlayerInfo::LoadDetails()
{
	try
	{
		Buffer aBuffer;
		SexyString aFileName = GetAppDataFolder() + StrFormat(_S("userdata/user%d.dat"), mId);
		if (!gSexyAppBase->ReadBufferFromFile(aFileName, &aBuffer, false))
		{
			return;
		}

		DataReader aReader;
		aReader.OpenMemory(aBuffer.GetDataPtr(), aBuffer.GetDataLen(), false);
		DataSync aSync(aReader);
		SyncDetails(aSync);
	}
	catch (DataReaderException&)
	{
		TodTrace("Failed to player data, resetting it\n");
		Reset();
	}
}

//0x4695F0
// GOTY @Patoke: 0x46D750
void PlayerInfo::SaveDetails()
{
	DataWriter aWriter;
	aWriter.OpenMemory();
	DataSync aSync(aWriter);
	SyncDetails(aSync);

	MkDir(GetAppDataFolder() + _S("userdata"));
	SexyString aFileName = GetAppDataFolder() + StrFormat(_S("userdata/user%d.dat"), mId);
	gSexyAppBase->WriteBytesToFile(aFileName, aWriter.GetDataPtr(), aWriter.GetDataLen());
}

//0x469810
void PlayerInfo::DeleteUserFiles()
{
	SexyString aFilename = GetAppDataFolder() + StrFormat(_S("userdata/user%d.dat"), mId);
	gSexyAppBase->EraseFile(aFilename);

	for (int i = 0; i < (int)GameMode::NUM_GAME_MODES; i++)
	{
		// Loading is architecture-specific, but deleting a profile must clean up both copies.
		gSexyAppBase->EraseFile(GetSavedGameNameForArchitecture((GameMode)i, mId, false));
		gSexyAppBase->EraseFile(GetSavedGameNameForArchitecture((GameMode)i, mId, true));
	}

	// Explicit deletion (including automatic oldest-profile pruning) removes all
	// replay saves and numbered rejected backups for both architectures. The
	// helper validates the complete filename grammar before deleting a match.
	DeleteSaveGameArtifactsForProfile(mId);
}

//0x469940
void PlayerInfo::Reset()
{
	mLevel = 1;
	mCoins = 0;
	mFinishedAdventure = 0;
	memset(mChallengeRecords, 0, sizeof(mChallengeRecords));
	memset(mPurchases, 0, sizeof(mPurchases));
	mPlayTimeActivePlayer = 0;
	mPlayTimeInactivePlayer = 0;
	mHasUsedCheatKeys = 0;
	mHasWokenStinky = 0;
	mDidntPurchasePacketUpgrade = 0;
	mLastStinkyChocolateTime = 0;
	mStinkyPosX = 0;
	mStinkyPosY = 0;
	mHasUnlockedMinigames = 0;
	mHasUnlockedPuzzleMode = 0;
	mHasNewMiniGame = 0;
	mHasNewScaryPotter = 0;
	mHasNewIZombie = 0;
	mHasNewSurvival = 0;
	mHasUnlockedSurvivalMode = 0;
	mNeedsMessageOnGameSelector = 0;
	mNeedsMagicTacoReward = 0;
	mHasSeenStinky = 0;
	mHasSeenUpsell = 0;
	mEarnedFromZenGarden = 0;
	memset(mPottedPlant, 0, sizeof(mPottedPlant));
	mNumPottedPlants = 0;
	memset(mEarnedAchievements, 0, sizeof(mEarnedAchievements));
	memset(mShownAchievements, 0, sizeof(mShownAchievements));
	memset(mPlantedPlants, 0, sizeof(mPlantedPlants));
#ifdef _HAS_ZOMBATAR
	mAckZombatarTOS = false;
#endif
	mDidRIPMode = false;
	mRIPLevel = 1;
}

void PlayerInfo::AddCoins(int theAmount)
{
	mCoins += theAmount;
	if (mCoins > 99999)
	{
		mCoins = 99999;
	}
	else if (mCoins < 0)
	{
		mCoins = 0;
	}
}

void PlayerInfo::ResetChallengeRecord(GameMode theGameMode)
{
	int aGameMode = (int)theGameMode - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	TOD_ASSERT(aGameMode >= 0 && aGameMode <= NUM_CHALLENGE_MODES);
	mChallengeRecords[aGameMode] = 0;
}

//0x469A00
void PottedPlant::InitializePottedPlant(SeedType theSeedType)
{
	memset(this, 0, sizeof(PottedPlant));
	mSeedType = theSeedType;
	mDrawVariation = DrawVariation::VARIATION_NORMAL;
	mLastWateredTime = 0;
	mFacing = (FacingDirection)RandRangeInt((int)FacingDirection::FACING_RIGHT, (int)FacingDirection::FACING_LEFT);
	mPlantAge = PottedPlantAge::PLANTAGE_SPROUT;
	mTimesFed = 0;
	mWhichZenGarden = GardenType::GARDEN_MAIN;
	mFeedingsPerGrow = RandRangeInt(3, 5);
	mPlantNeed = PottedPlantNeed::PLANTNEED_NONE;
	mLastNeedFulfilledTime = 0;
	mLastFertilizedTime = 0;
	mLastChocolateTime = 0;
}
