#ifndef __SAVEGAMECONTEXT_H__
#define __SAVEGAMECONTEXT_H__

#include <string>
#include "../../Sexy.TodLib/TodList.h"
#include "../../SexyAppFramework/Buffer.h"

class Board;
class Trail;
enum GameMode;
class Reanimation;
class TodParticleSystem;
class TodParticleEmitter;
class ReanimatorDefinition;
class TodParticleDefinition;
class TrailDefinition;
namespace Sexy
{
    class Image;
}
using namespace Sexy;

struct SaveFileHeader
{
    unsigned int    mMagicNumber;
    unsigned int    mBuildVersion;
    unsigned int    mBuildDate;
};

enum class SaveGameLoadStatus
{
    NONE,
    LOADED,
    REJECTED_PRESERVED,
    REJECTED_UNPRESERVED
};

enum class LegacySaveMigrationStatus
{
    NOT_APPLICABLE,
    TARGET_PRESENT,
    MIGRATED,
    USE_LEGACY_PATH,
    PROBE_FAILED
};

// Gameplay coordinates are serialized in stable board-local design units.
// Keep this independent from the window, presentation viewport and render-target
// dimensions so display-resolution changes never require rewriting entity data.
enum class SaveGameCoordinateSpace : unsigned int
{
    BOARD_LOCAL_DESIGN_V1 = 1U
};

struct SaveGameCoordinateMetadata
{
    SaveGameCoordinateSpace mCoordinateSpace;
    int                     mBoardWidth;
    int                     mBoardHeight;
};

class SaveGameContext
{
public:
    Buffer          mBuffer;            //+0x0
    bool            mFailed;            //+0x20
    bool            mReading;           //+0x21

public:
    inline int      ByteLeftToRead() { return (mBuffer.mDataBitSize - mBuffer.mReadBitPos + 7) / 8; }
    void            SyncBytes(void* theDest, int theReadSize);
    void            SyncInt(int& theInt);
    inline void     SyncUint(unsigned int& theInt) { SyncInt((signed int&)theInt); }
    void            SyncReanimationDef(ReanimatorDefinition*& theDefinition);
    void            SyncParticleDef(TodParticleDefinition*& theDefinition);
    void            SyncTrailDef(TrailDefinition*& theDefinition);
    void            SyncImage(Image*& theImage);
};

void                SyncDataIDList(TodList<unsigned int>* theDataIDList, SaveGameContext& theContext, TodAllocator* theAllocator);
void                SyncParticleEmitter(TodParticleSystem* theParticleSystem, TodParticleEmitter* theParticleEmitter, SaveGameContext& theContext);
void                SyncParticleSystem(Board* theBoard, TodParticleSystem* theParticleSystem, SaveGameContext& theContext);
void                SyncReanimation(Board* theBoard, Reanimation* theReanimation, SaveGameContext& theContext);
void                SyncTrail(Board* theBoard, Trail* theTrail, SaveGameContext& theContext);
void                SyncBoard(SaveGameContext& theContext, Board* theBoard);
void				FixBoardAfterLoad(Board* theBoard);
bool                MigrateBoardCoordinatesAfterLoad(Board* theBoard, const SaveGameCoordinateMetadata& theMetadata);
SaveGameLoadStatus  LawnLoadGame(Board* theBoard, const SexyString& theFilePath);
bool				LawnSaveGame(Board* theBoard, const SexyString& theFilePath);
LegacySaveMigrationStatus TryMigrateLegacyX64Save(const SexyString& theLegacyFilePath, const SexyString& theX64FilePath);
void                DeleteSaveGameArtifactsForProfile(int theProfileId);

#endif
