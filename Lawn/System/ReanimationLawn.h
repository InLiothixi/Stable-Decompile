#ifndef __REANIMATORCACHE_H__
#define __REANIMATORCACHE_H__

#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodList.h"
namespace Sexy
{
    class Graphics;
    class MemoryImage;
};
using namespace Sexy;

class LawnApp;
class ReanimCacheImageVariation
{
public:
    SeedType                mSeedType;
    DrawVariation           mDrawVariation;
    DrawFilterVariation     mFilterVariation;
    unsigned int            mDrawBitVariation;
    MemoryImage*            mImage;
};
typedef TodList<ReanimCacheImageVariation> ImageVariationList;

class Reanimation;
class ReanimatorCache
{
public:
	MemoryImage*			mPlantImages[SeedType::NUM_SEED_TYPES];
    ImageVariationList      mImageVariationList;
    MemoryImage*            mLawnMowers[LawnMowerType::NUM_MOWER_TYPES];
    MemoryImage*            mZombieImages[ZombieType::NUM_CACHED_ZOMBIE_TYPES];
    LawnApp*                mApp;

public:
    //ReanimatorCache() { ReanimatorCacheInitialize(); }
    //~ReanimatorCache() { ReanimatorCacheDispose(); }

    void                    ReanimatorCacheInitialize();
    void                    ReanimatorCacheDispose();
    void                    DrawCachedPlant(Graphics* g, float thePosX, float thePosY, SeedType theSeedType, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE, unsigned int theBitVariation = 0U);
    void                    DrawCachedMower(Graphics* g, float thePosX, float thePosY, LawnMowerType theMowerType, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE);
    void                    DrawCachedZombie(Graphics* g, float thePosX, float thePosY, ZombieType theZombieType, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE);
    MemoryImage*            MakeBlankMemoryImage(int theWidth, int theHeight);
    MemoryImage*            MakeCachedPlantFrame(SeedType theSeedType, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE, unsigned int theBitVariation = 0U);
    MemoryImage*            MakeCachedMowerFrame(LawnMowerType theMowerType);
    MemoryImage*            MakeCachedZombieFrame(ZombieType theZombieType);
    /*inline*/ void         GetPlantImageSize(SeedType theSeedType, int& theOffsetX, int& theOffsetY, int& theWidth, int& theHeight);
    void                    DrawReanimatorFrame(Graphics* g, float thePosX, float thePosY, ReanimationType theReanimationType, const char* theTrackName, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE, unsigned int theBitVariation = 0U);
    void                    UpdateReanimationForVariation(Reanimation* theReanim, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE);
};

#endif
