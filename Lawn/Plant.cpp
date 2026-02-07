
#include "Coin.h"
#include "Plant.h"
#include "Board.h"
#include "Zombie.h"
#include "Cutscene.h"
#include "GridItem.h"
#include "ZenGarden.h"
#include "Challenge.h"
#include "Projectile.h"
#include "SeedPacket.h"
#include "../LawnApp.h"
#include "CursorObject.h"
#include "../GameConstants.h"
#include "System/PlayerInfo.h"
#include "System/ReanimationLawn.h"
#include "../Sexy.TodLib/TodFoley.h"
#include "../Sexy.TodLib/TodDebug.h"
#include "../Sexy.TodLib/Attachment.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/TodParticle.h"
#include "../Sexy.TodLib/EffectSystem.h"
#include "../Sexy.TodLib/TodStringFile.h"
#include "Widget/AchievementsScreen.h"
#include "ToolTipWidget.h"
#include "../Sexy.TodLib/FilterEffect.h"

PlantDefinition gPlantDefs[SeedType::NUM_SEED_TYPES] = {  //0x69F2B0
    { SeedType::SEED_PEASHOOTER,        nullptr, ReanimationType::REANIM_PEASHOOTER,    0,  100,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("PEASHOOTER") },
    { SeedType::SEED_SUNFLOWER,         nullptr, ReanimationType::REANIM_SUNFLOWER,     1,  50,     750,    PlantSubClass::SUBCLASS_NORMAL,     2500,   _S("SUNFLOWER") },
    { SeedType::SEED_CHERRYBOMB,        nullptr, ReanimationType::REANIM_CHERRYBOMB,    3,  150,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("CHERRY_BOMB") },
    { SeedType::SEED_WALLNUT,           nullptr, ReanimationType::REANIM_WALLNUT,       2,  50,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("WALL_NUT") },
    { SeedType::SEED_POTATOMINE,        nullptr, ReanimationType::REANIM_POTATOMINE,    37, 25,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("POTATO_MINE") },
    { SeedType::SEED_SNOWPEA,           nullptr, ReanimationType::REANIM_SNOWPEA,       4,  175,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("SNOW_PEA") },
    { SeedType::SEED_CHOMPER,           nullptr, ReanimationType::REANIM_CHOMPER,       31, 150,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("CHOMPER") },
    { SeedType::SEED_REPEATER,          nullptr, ReanimationType::REANIM_REPEATER,      5,  200,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("REPEATER") },
    { SeedType::SEED_PUFFSHROOM,        nullptr, ReanimationType::REANIM_PUFFSHROOM,    6,  0,      750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("PUFF_SHROOM") },
    { SeedType::SEED_SUNSHROOM,         nullptr, ReanimationType::REANIM_SUNSHROOM,     7,  25,     750,    PlantSubClass::SUBCLASS_NORMAL,     2500,   _S("SUN_SHROOM") },
    { SeedType::SEED_FUMESHROOM,        nullptr, ReanimationType::REANIM_FUMESHROOM,    9,  75,     750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("FUME_SHROOM") },
    { SeedType::SEED_GRAVEBUSTER,       nullptr, ReanimationType::REANIM_GRAVE_BUSTER,  40, 75,     750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("GRAVE_BUSTER") },
    { SeedType::SEED_HYPNOSHROOM,       nullptr, ReanimationType::REANIM_HYPNOSHROOM,   10, 75,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("HYPNO_SHROOM") },
    { SeedType::SEED_SCAREDYSHROOM,     nullptr, ReanimationType::REANIM_SCRAREYSHROOM, 33, 25,     750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("SCAREDY_SHROOM") },
    { SeedType::SEED_ICESHROOM,         nullptr, ReanimationType::REANIM_ICESHROOM,     36, 75,     5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("ICE_SHROOM") },
    { SeedType::SEED_DOOMSHROOM,        nullptr, ReanimationType::REANIM_DOOMSHROOM,    20, 125,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("DOOM_SHROOM") },
    { SeedType::SEED_LILYPAD,           nullptr, ReanimationType::REANIM_LILYPAD,       19, 25,     750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("LILY_PAD") },
    { SeedType::SEED_SQUASH,            nullptr, ReanimationType::REANIM_SQUASH,        21, 50,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("SQUASH") },
    { SeedType::SEED_THREEPEATER,       nullptr, ReanimationType::REANIM_THREEPEATER,   12, 325,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("THREEPEATER") },
    { SeedType::SEED_TANGLEKELP,        nullptr, ReanimationType::REANIM_TANGLEKELP,    17, 25,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("TANGLE_KELP") },
    { SeedType::SEED_JALAPENO,          nullptr, ReanimationType::REANIM_JALAPENO,      11, 125,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("JALAPENO") },
    { SeedType::SEED_SPIKEWEED,         nullptr, ReanimationType::REANIM_SPIKEWEED,     22, 100,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("SPIKEWEED") },
    { SeedType::SEED_TORCHWOOD,         nullptr, ReanimationType::REANIM_TORCHWOOD,     29, 175,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("TORCHWOOD") },
    { SeedType::SEED_TALLNUT,           nullptr, ReanimationType::REANIM_TALLNUT,       28, 125,    3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("TALL_NUT") },
    { SeedType::SEED_SEASHROOM,         nullptr, ReanimationType::REANIM_SEASHROOM,     39, 0,      3000,   PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("SEA_SHROOM") },
    { SeedType::SEED_PLANTERN,          nullptr, ReanimationType::REANIM_PLANTERN,      38, 25,     3000,   PlantSubClass::SUBCLASS_NORMAL,     2500,   _S("PLANTERN") },
    { SeedType::SEED_CACTUS,            nullptr, ReanimationType::REANIM_CACTUS,        15, 125,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("CACTUS") },
    { SeedType::SEED_BLOVER,            nullptr, ReanimationType::REANIM_BLOVER,        18, 100,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("BLOVER") },
    { SeedType::SEED_SPLITPEA,          nullptr, ReanimationType::REANIM_SPLITPEA,      32, 125,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("SPLIT_PEA") },
    { SeedType::SEED_STARFRUIT,         nullptr, ReanimationType::REANIM_STARFRUIT,     30, 125,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("STARFRUIT") },
    { SeedType::SEED_PUMPKINSHELL,      nullptr, ReanimationType::REANIM_PUMPKIN,       25, 125,    3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("PUMPKIN") },
    { SeedType::SEED_MAGNETSHROOM,      nullptr, ReanimationType::REANIM_MAGNETSHROOM,  35, 100,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("MAGNET_SHROOM") },
    { SeedType::SEED_CABBAGEPULT,       nullptr, ReanimationType::REANIM_CABBAGEPULT,   13, 100,    750,    PlantSubClass::SUBCLASS_SHOOTER,    300,    _S("CABBAGE_PULT") },
    { SeedType::SEED_FLOWERPOT,         nullptr, ReanimationType::REANIM_FLOWER_POT,    33, 25,     750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("FLOWER_POT") },
    { SeedType::SEED_KERNELPULT,        nullptr, ReanimationType::REANIM_KERNELPULT,    13, 100,    750,    PlantSubClass::SUBCLASS_SHOOTER,    300,    _S("KERNEL_PULT") },
    { SeedType::SEED_INSTANT_COFFEE,    nullptr, ReanimationType::REANIM_COFFEEBEAN,    33, 75,     750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("COFFEE_BEAN") },
    { SeedType::SEED_GARLIC,            nullptr, ReanimationType::REANIM_GARLIC,        8,  50,     750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("GARLIC") },
    { SeedType::SEED_UMBRELLA,          nullptr, ReanimationType::REANIM_UMBRELLALEAF,  23, 100,    750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("UMBRELLA_LEAF") },
    { SeedType::SEED_MARIGOLD,          nullptr, ReanimationType::REANIM_MARIGOLD,      24, 50,     3000,   PlantSubClass::SUBCLASS_NORMAL,     2500,   _S("MARIGOLD") },
    { SeedType::SEED_MELONPULT,         nullptr, ReanimationType::REANIM_MELONPULT,     14, 300,    750,    PlantSubClass::SUBCLASS_SHOOTER,    300,    _S("MELON_PULT") },
    { SeedType::SEED_GATLINGPEA,        nullptr, ReanimationType::REANIM_GATLINGPEA,    5,  250,    5000,   PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("GATLING_PEA") },
    { SeedType::SEED_TWINSUNFLOWER,     nullptr, ReanimationType::REANIM_TWIN_SUNFLOWER,1,  150,    5000,   PlantSubClass::SUBCLASS_NORMAL,     2500,   _S("TWIN_SUNFLOWER") },
    { SeedType::SEED_GLOOMSHROOM,       nullptr, ReanimationType::REANIM_GLOOMSHROOM,   27, 150,    5000,   PlantSubClass::SUBCLASS_SHOOTER,    200,    _S("GLOOM_SHROOM") },
    { SeedType::SEED_CATTAIL,           nullptr, ReanimationType::REANIM_CATTAIL,       27, 225,    5000,   PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("CATTAIL") },
    { SeedType::SEED_WINTERMELON,       nullptr, ReanimationType::REANIM_WINTER_MELON,  27, 200,    5000,   PlantSubClass::SUBCLASS_SHOOTER,    300,    _S("WINTER_MELON") },
    { SeedType::SEED_GOLD_MAGNET,       nullptr, ReanimationType::REANIM_GOLD_MAGNET,   27, 50,     5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("GOLD_MAGNET") },
    { SeedType::SEED_SPIKEROCK,         nullptr, ReanimationType::REANIM_SPIKEROCK,     27, 125,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("SPIKEROCK") },
    { SeedType::SEED_COBCANNON,         nullptr, ReanimationType::REANIM_COBCANNON,     16, 500,    5000,   PlantSubClass::SUBCLASS_NORMAL,     600,    _S("COB_CANNON") },
    { SeedType::SEED_IMITATER,          nullptr, ReanimationType::REANIM_IMITATER,      33, 0,      750,    PlantSubClass::SUBCLASS_NORMAL,     0,      _S("IMITATER") },
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    { SeedType::SEED_TIMESTOPPER,       &Sexy::IMAGE_DUPLICATOR, ReanimationType::REANIM_NONE,  37, 50,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("TIME_STOPPER") },
    { SeedType::SEED_ICEBERGLETTUCE,    &Sexy::IMAGE_ICEBERG, ReanimationType::REANIM_NONE,     37, 200,    750,    PlantSubClass::SUBCLASS_SHOOTER,    300,    _S("ICEBERG_LETTUCE") },
    { SeedType::SEED_CHERRYHOVERBOMB,   &Sexy::IMAGE_CHERRYBOMB, ReanimationType::REANIM_NONE,  3,  175,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("CHERRY_HOVER_BOMB") },
    { SeedType::SEED_STINGER,           &Sexy::IMAGE_STINGER, ReanimationType::REANIM_NONE,     37, 125,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("STINGER") },
    { SeedType::SEED_DUPLICATORCYCLER,  &Sexy::IMAGE_DUPLICATOR, ReanimationType::REANIM_NONE,  37, 150,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("DUPLICATOR_CYCLER") },
    { SeedType::SEED_BEEHIVE,           &Sexy::IMAGE_BEEHIVE, ReanimationType::REANIM_NONE,     37, 250,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("BEEHIVE") },
    { SeedType::SEED_YAMPOLINE,         &Sexy::IMAGE_YAMPOLINE, ReanimationType::REANIM_NONE,   37, 125,    3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("YAMPOLINE") },
    { SeedType::SEED_NUKECUMBER,        &Sexy::IMAGE_NUKECUMBER, ReanimationType::REANIM_NONE,  37, 500,    5000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("NUKECUMBER") },
#endif
    { SeedType::SEED_EXPLODE_O_NUT,     nullptr, ReanimationType::REANIM_WALLNUT,       2,  50,     3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("EXPLODE_O_NUT") },
    { SeedType::SEED_GIANT_WALLNUT,     nullptr, ReanimationType::REANIM_WALLNUT,       2,  400,    3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("GIANT_WALLNUT") },
    { SeedType::SEED_SPROUT,            nullptr, ReanimationType::REANIM_ZENGARDEN_SPROUT,  33, 0,  3000,   PlantSubClass::SUBCLASS_NORMAL,     0,      _S("SPROUT") },
    { SeedType::SEED_LEFTPEATER,        nullptr, ReanimationType::REANIM_REPEATER,      5,  200,    750,    PlantSubClass::SUBCLASS_SHOOTER,    150,    _S("REPEATER") }
};

//0x401B20
Plant::Plant()
{
}
//0x45DB60
void Plant::PlantInitialize(int theGridX, int theGridY, SeedType theSeedType, SeedType theImitaterType)
{
    mPlantCol = theGridX;
    mRow = theGridY;
    if (mBoard)
    {
        mX = mBoard->GridToPixelX(theGridX, theGridY);
        mY = mBoard->GridToPixelY(theGridX, theGridY);
    }
    mAnimCounter = 0;
    mAnimPing = true;
    mFrame = 0;
    mShootingCounter = 0;
    mShakeOffsetX = 0.0f;
    mShakeOffsetY = 0.0f;
    mFrameLength = RandRangeInt(12, 18);
    mTargetX = -1;
    mTargetY = -1;
    mStartRow = mRow;
    mNumFrames = 5;
    mState = PlantState::STATE_NOTREADY;
    mDead = false;
    mSquished = false;
    mSeedType = theSeedType;
    mImitaterType = theImitaterType;
    mPlantHealth = 300;
    mDoSpecialCountdown = 0;
    mDisappearCountdown = 200;
    mStateCountdown = 0;
    mParticleID = ParticleSystemID::PARTICLESYSTEMID_NULL;
    mBodyReanimID = ReanimationID::REANIMATIONID_NULL;
    mHeadReanimID = ReanimationID::REANIMATIONID_NULL;
    mHeadReanimID2 = ReanimationID::REANIMATIONID_NULL;
    mHeadReanimID3 = ReanimationID::REANIMATIONID_NULL;
    mBlinkReanimID = ReanimationID::REANIMATIONID_NULL;
    mLightReanimID = ReanimationID::REANIMATIONID_NULL;
    mSleepingReanimID = ReanimationID::REANIMATIONID_NULL;
    mBlinkCountdown = 0;
    mRecentlyEatenCountdown = 0;
    mEatenFlashCountdown = 0;
    mBeghouledFlashCountdown = 0;
    mWidth = 80;
    mHeight = 80;
    memset(mMagnetItems, 0, sizeof(mMagnetItems));
    const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
    mIsAsleep = false;
    mWakeUpCounter = 0;
    mOnBungeeState = PlantOnBungeeState::NOT_ON_BUNGEE;
    mPottedPlantIndex = -1;
    mLaunchRate = aPlantDef.mLaunchRate;
    mSubclass = aPlantDef.mSubClass;
    mRenderOrder = CalcRenderOrder();
    mDuplicatorSeedType = SeedType::SEED_NONE;
    mDuplicatorImitaterType = SeedType::SEED_NONE;
    mBurnedCounter = -1;
    mPoweredCounter = -1;
    mSpecialReanimID = ReanimationID::REANIMATIONID_NULL;
    mRad = 0.0f;

    if (IsOnBoard())
        mLastStandFlagPlaced = mBoard->mChallenge->mSurvivalStage;

    Reanimation* aBodyReanim = nullptr;
    if (aPlantDef.mReanimationType != ReanimationType::REANIM_NONE)
    {
        float aOffsetY = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);
        ReanimationType aReanimType = aPlantDef.mReanimationType;
#ifdef _DS_MINIGAMES
        if (aReanimType == ReanimationType::REANIM_PEASHOOTER && mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
        {
            aReanimType = ReanimationType::REANIM_HEATWAVE_SHOOTER;
        }
#endif
        aBodyReanim = mApp->AddReanimation(0.0f, aOffsetY, mRenderOrder + 1, aReanimType);
        aBodyReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
        aBodyReanim->mAnimRate = RandRangeFloat(10.0f, 15.0f);

        if (aBodyReanim->TrackExists("anim_idle"))
            aBodyReanim->SetFramesForLayer("anim_idle");

        if (!mApp->GetDialog(Dialogs::DIALOG_ALMANAC) && mApp->IsWallnutBowlingLevel() && aBodyReanim->TrackExists("_ground"))
        {
            aBodyReanim->SetFramesForLayer("_ground");
            if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_EXPLODE_O_NUT)
                aBodyReanim->mAnimRate = RandRangeFloat(12.0f, 18.0f);
            else if (mSeedType == SeedType::SEED_GIANT_WALLNUT)
                aBodyReanim->mAnimRate = RandRangeFloat(6.0f, 10.0f);

            {
                const char* aTrackToPlay = "anim_blink";
                int aHit = Rand(10);
                if (aHit < 1 && aBodyReanim->TrackExists("anim_blink_twitch"))
                {
                    aTrackToPlay = "anim_blink_twitch";
                }
                else
                {
                    aTrackToPlay = aHit < 7 ? "anim_blink_twice" : "anim_blink_thrice";
                }

                Reanimation* aBlinkReanim = mApp->mEffectSystem->mReanimationHolder->AllocReanimation(0.0f, 0.0f, 0, aBodyReanim->mReanimationType);
                aBlinkReanim->SetFramesForLayer(aTrackToPlay);
                aBlinkReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_FULL_LAST_FRAME_AND_HOLD;
                aBlinkReanim->mAnimRate = 0.0f;
                aBlinkReanim->mAnimTime = RandRangeFloat(0.0f, 1.0f);
                aBlinkReanim->mColorOverride = aBodyReanim->mColorOverride;

                if (aBodyReanim->TrackExists("anim_face"))
                {
                    aBlinkReanim->AttachToAnotherReanimation(aBodyReanim, "anim_face");
                }
                else if (aBodyReanim->TrackExists("anim_idle"))
                {
                    aBlinkReanim->AttachToAnotherReanimation(aBodyReanim, "anim_idle");
                }
                else
                {
                    TodTrace("Missing anim_idle for blink");
                }

                aBlinkReanim->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }

        aBodyReanim->mIsAttachment = true;
        mBodyReanimID = mApp->ReanimationGetID(aBodyReanim);
        mBlinkCountdown = 400 + Sexy::Rand(400);

#ifdef _HAS_ROOF_SLOPE_ANGLE
        if (mBoard && mBoard->StageHasRoof())
        {
            if (mPlantCol < 5)
            {
                int nextColoumn = mPlantCol + 1;
                if (mSeedType == SeedType::SEED_COBCANNON) nextColoumn++;
                const float x2 = mBoard->GridToPixelX(nextColoumn, mRow);
                const float y2 = mBoard->GridToPixelY(nextColoumn, mRow);
                mRad = -atan2(y2 - mY, x2 - mX);
                float rotatedHeightX = sin(mRad) * aOffsetY;
                const float offsetX = -cos(mRad) * 15 + rotatedHeightX;
                const float offsetY = sin(mRad) * 40;
                aBodyReanim->mOffsetX = offsetX;
                aBodyReanim->mOffsetY = offsetY;
                TodScaleRotateTransformMatrix(aBodyReanim->mOverlayMatrix, 0.0f, 0.0f, mRad, 1.0f, 1.0f);
                UpdateReanim();
            }
        }
#else
        if (mBoard && mBoard->StageHasRoof() && mSeedType == SeedType::SEED_COBCANNON)
        {
            if (mPlantCol < 5)
            {
                int nextColoumn = mPlantCol + 2;
                const float x2 = mBoard->GridToPixelX(nextColoumn, mRow);
                const float y2 = mBoard->GridToPixelY(nextColoumn, mRow);
                mRad = -atan2(y2 - mY, x2 - mX);
                float rotatedHeightX = sin(mRad) * aOffsetY;
                const float offsetX = -cos(mRad) * 15 + rotatedHeightX;
                const float offsetY = sin(mRad) * 40;
                aBodyReanim->mOffsetX = offsetX;
                aBodyReanim->mOffsetY = offsetY;
                TodScaleRotateTransformMatrix(aBodyReanim->mOverlayMatrix, 0.0f, 0.0f, mRad, 1.0f, 1.0f);
                UpdateReanim();
            }
        }
#endif
    }

    if (IsNocturnal(mSeedType) && mBoard && !mBoard->StageIsNight())
        SetSleeping(true);

    if (mLaunchRate > 0)
    {
        if (MakesSun())
        {
            mLaunchCounter = RandRangeInt(300, mLaunchRate / 2);
        }
        else
        {
            mLaunchCounter = RandRangeInt(0, mLaunchRate);
        }
    }
    else
    {
        mLaunchCounter = 0;
    }

    if (mSeedType == SeedType::SEED_CATTAIL)
    {
        mLaunchCounter = 51;
        mShootingCounter = 50;
    }


    switch (theSeedType)
    {
    case SeedType::SEED_BLOVER:
    {
        mDoSpecialCountdown = 50;
        mStateCountdown = 50;

        if (IsInPlay())
        {
            aBodyReanim->SetFramesForLayer("anim_blow");
            aBodyReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
            aBodyReanim->mAnimRate = 20.0f;
        }
        else
        {
            aBodyReanim->SetFramesForLayer("anim_idle");
            aBodyReanim->mAnimRate = 10.0f;
        }

        break;
    }
    case SeedType::SEED_PEASHOOTER:
    case SeedType::SEED_SNOWPEA:
    case SeedType::SEED_REPEATER:
    case SeedType::SEED_LEFTPEATER:
    case SeedType::SEED_GATLINGPEA:
        if (aBodyReanim)
        {
            aBodyReanim->mAnimRate = RandRangeFloat(15.0f, 20.0f);
            ReanimationType aReanimType = aPlantDef.mReanimationType;
#ifdef _DS_MINIGAMES
            if (mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
                aReanimType = ReanimationType::REANIM_HEATWAVE_SHOOTER;
#endif
            Reanimation* aHeadReanim = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aReanimType);
            aHeadReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
            aHeadReanim->mAnimRate = aBodyReanim->mAnimRate;
            aHeadReanim->SetFramesForLayer("anim_head_idle");
            mHeadReanimID = mApp->ReanimationGetID(aHeadReanim);

            if (aBodyReanim->TrackExists("anim_stem"))
                aHeadReanim->AttachToAnotherReanimation(aBodyReanim, "anim_stem");
            else if (aBodyReanim->TrackExists("anim_idle"))
                aHeadReanim->AttachToAnotherReanimation(aBodyReanim, "anim_idle");
        }
        break;
    case SeedType::SEED_SPLITPEA:
    {
        TOD_ASSERT(aBodyReanim);

        aBodyReanim->mAnimRate = RandRangeFloat(15.0f, 20.0f);
        Reanimation* aHeadReanim1 = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aPlantDef.mReanimationType);
        aHeadReanim1->mLoopType = ReanimLoopType::REANIM_LOOP;
        aHeadReanim1->mAnimRate = aBodyReanim->mAnimRate;
        aHeadReanim1->SetFramesForLayer("anim_head_idle");
        aHeadReanim1->AttachToAnotherReanimation(aBodyReanim, "anim_idle");
        mHeadReanimID = mApp->ReanimationGetID(aHeadReanim1);

        Reanimation* aHeadReanim2 = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aPlantDef.mReanimationType);
        aHeadReanim2->mLoopType = ReanimLoopType::REANIM_LOOP;
        aHeadReanim2->mAnimRate = aBodyReanim->mAnimRate;
        aHeadReanim2->SetFramesForLayer("anim_splitpea_idle");
        aHeadReanim2->AttachToAnotherReanimation(aBodyReanim, "anim_idle");
        mHeadReanimID2 = mApp->ReanimationGetID(aHeadReanim2);

        break;
    }
    case SeedType::SEED_THREEPEATER:
    {
        TOD_ASSERT(aBodyReanim);

        aBodyReanim->mAnimRate = RandRangeFloat(15.0f, 20.0f);
        Reanimation* aHeadReanim1 = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aPlantDef.mReanimationType);
        aHeadReanim1->mLoopType = ReanimLoopType::REANIM_LOOP;
        aHeadReanim1->mAnimRate = aBodyReanim->mAnimRate;
        aHeadReanim1->SetFramesForLayer("anim_head_idle1");
        aHeadReanim1->AttachToAnotherReanimation(aBodyReanim, "anim_head1");
        mHeadReanimID = mApp->ReanimationGetID(aHeadReanim1);

        Reanimation* aHeadReanim2 = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aPlantDef.mReanimationType);
        aHeadReanim2->mLoopType = ReanimLoopType::REANIM_LOOP;
        aHeadReanim2->mAnimRate = aBodyReanim->mAnimRate;
        aHeadReanim2->SetFramesForLayer("anim_head_idle2");
        aHeadReanim2->AttachToAnotherReanimation(aBodyReanim, "anim_head2");
        mHeadReanimID2 = mApp->ReanimationGetID(aHeadReanim2);

        Reanimation* aHeadReanim3 = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, aPlantDef.mReanimationType);
        aHeadReanim3->mLoopType = ReanimLoopType::REANIM_LOOP;
        aHeadReanim3->mAnimRate = aBodyReanim->mAnimRate;
        aHeadReanim3->SetFramesForLayer("anim_head_idle3");
        aHeadReanim3->AttachToAnotherReanimation(aBodyReanim, "anim_head3");
        mHeadReanimID3 = mApp->ReanimationGetID(aHeadReanim3);

        break;
    }
    case SeedType::SEED_WALLNUT:
        mPlantHealth = 4000;
        mBlinkCountdown = 1000 + Sexy::Rand(1000);
        break;
    case SeedType::SEED_EXPLODE_O_NUT:
        mPlantHealth = 4000;
        mBlinkCountdown = 1000 + Sexy::Rand(1000);
        aBodyReanim->mColorOverride = Color(255, 64, 64);
        break;
    case SeedType::SEED_GIANT_WALLNUT:
        mPlantHealth = 12000;
        mBlinkCountdown = 1000 + Sexy::Rand(1000);
        break;
    case SeedType::SEED_TALLNUT:
        mPlantHealth = 8000;
        mHeight = 80;
        mBlinkCountdown = 1000 + Sexy::Rand(1000);
        break;
    case SeedType::SEED_GARLIC:
        TOD_ASSERT(aBodyReanim);
        mPlantHealth = 400;
        aBodyReanim->SetTruncateDisappearingFrames();
        break;
    case SeedType::SEED_GOLD_MAGNET:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->SetTruncateDisappearingFrames();
        break;
    case SeedType::SEED_IMITATER:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->mAnimRate = RandRangeFloat(25.0f, 30.0f);
        mStateCountdown = 200;
        break;
    case SeedType::SEED_CHERRYBOMB:
    case SeedType::SEED_JALAPENO:
    {
        TOD_ASSERT(aBodyReanim);

        if (IsInPlay())
        {
            mDoSpecialCountdown = 100;

            aBodyReanim->SetFramesForLayer("anim_explode");
            aBodyReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;

            mApp->PlayFoley(FoleyType::FOLEY_REVERSE_EXPLOSION);
        }

        break;
    }
    case SeedType::SEED_POTATOMINE:
    {
        TOD_ASSERT(aBodyReanim);

        aBodyReanim->mAnimRate = 12.0f;

        if (IsInPlay())
        {
            aBodyReanim->AssignRenderGroupToTrack("anim_glow", RENDER_GROUP_HIDDEN);
            mStateCountdown = 1500;

            if (mApp->IsIZombieLevel()) {
                aBodyReanim->SetFramesForLayer("anim_armed");
                mState = PlantState::STATE_POTATO_ARMED;

                float aRate = RandRangeFloat(12.0f, 15.0f);
                Reanimation* aLightReanim = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, GetPlantDefinition(mSeedType).mReanimationType);
                aLightReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
                aLightReanim->mAnimRate = aRate - 2.0f;
                aLightReanim->SetFramesForLayer("anim_glow");
                aLightReanim->mFrameCount = 10;
                aLightReanim->ShowOnlyTrack("anim_glow");
                aLightReanim->SetTruncateDisappearingFrames("anim_glow", false);
                mLightReanimID = mApp->ReanimationGetID(aLightReanim);
                aLightReanim->AttachToAnotherReanimation(aBodyReanim, "anim_light");
            }
        }
        else
        {
            aBodyReanim->SetFramesForLayer("anim_armed");
            mState = PlantState::STATE_POTATO_ARMED;

            float aRate = RandRangeFloat(12.0f, 15.0f);
            Reanimation* aLightReanim = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, GetPlantDefinition(mSeedType).mReanimationType);
            aLightReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
            aLightReanim->mAnimRate = aRate - 2.0f;
            aLightReanim->SetFramesForLayer("anim_glow");
            aLightReanim->mFrameCount = 10;
            aLightReanim->ShowOnlyTrack("anim_glow");
            aLightReanim->SetTruncateDisappearingFrames("anim_glow", false);
            mLightReanimID = mApp->ReanimationGetID(aLightReanim);
            aLightReanim->AttachToAnotherReanimation(aBodyReanim, "anim_light");
        }

        break;
    }
    case SeedType::SEED_GRAVEBUSTER:
    {
        TOD_ASSERT(aBodyReanim);

        if (IsInPlay())
        {
            aBodyReanim->SetFramesForLayer("anim_land");
            aBodyReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;

            mState = PlantState::STATE_GRAVEBUSTER_LANDING;
            mApp->PlayFoley(FoleyType::FOLEY_GRAVEBUSTERCHOMP);
        }

        break;
    }
    case SeedType::SEED_SUNSHROOM:
    {
        TOD_ASSERT(aBodyReanim);

        aBodyReanim->mFrameBasePose = 6;

        if (IsInPlay())
        {
            mX += Sexy::Rand(10) - 5;
            mY += Sexy::Rand(10) - 5;
        }
        else if (mIsAsleep)
        {
            aBodyReanim->SetFramesForLayer("anim_bigsleep");
        }
        else
        {
            aBodyReanim->SetFramesForLayer("anim_bigidle");
        }

        mState = PlantState::STATE_SUNSHROOM_SMALL;

#ifdef DO_FIX_BUGS
        if (!IsInPlay() && !mIsAsleep)
            mState = PlantState::STATE_SUNSHROOM_BIG;
#endif

        mStateCountdown = 12000;

        break;
    }
    case SeedType::SEED_PUFFSHROOM:
    case SeedType::SEED_SEASHROOM:
        if (IsInPlay())
        {
            mX += Sexy::Rand(10) - 5;
            mY += Sexy::Rand(6) - 3;
        }
        break;
    case SeedType::SEED_PUMPKINSHELL:
    {
        mPlantHealth = 4000;
        mWidth = 120;

        TOD_ASSERT(aBodyReanim);
        aBodyReanim->AssignRenderGroupToTrack("Pumpkin_back", 1);
        break;
    }
    case SeedType::SEED_CHOMPER:
        mState = PlantState::STATE_READY;
        break;
    case SeedType::SEED_PLANTERN:
    {
        mStateCountdown = 50;

        if (!IsOnBoard() || mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
        {
            AddAttachedParticle(mX + 40, mY + 40, (int)RenderLayer::RENDER_LAYER_FOG + 1, ParticleEffect::PARTICLE_LANTERN_SHINE);
        }
        if (IsInPlay())
        {
            mApp->PlaySample(Sexy::SOUND_PLANTERN);
        }

        break;
    }
    case SeedType::SEED_TORCHWOOD:
        break;
    case SeedType::SEED_MARIGOLD:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->mAnimRate = RandRangeFloat(15.0f, 20.0f);
        break;
    case SeedType::SEED_CACTUS:
        mState = PlantState::STATE_CACTUS_LOW;
        break;
    case SeedType::SEED_INSTANT_COFFEE:
        mDoSpecialCountdown = 100;
        break;
    case SeedType::SEED_SCAREDYSHROOM:
        mState = PlantState::STATE_READY;
        break;
    case SeedType::SEED_COBCANNON:
        if (IsInPlay())
        {
            mState = PlantState::STATE_COBCANNON_ARMING;
            mStateCountdown = 500;

            TOD_ASSERT(aBodyReanim);
            aBodyReanim->SetFramesForLayer("anim_unarmed_idle");
        }
        break;
    case SeedType::SEED_KERNELPULT:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->AssignRenderGroupToPrefix("Cornpult_butter", RENDER_GROUP_HIDDEN);
#ifdef _HAS_KERNELPULT_BUTTER_IDLE
        if (IsOnBoard() && mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN && Sexy::Rand(4) == 0)
        {
            Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
            aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
            aBodyReanim->AssignRenderGroupToPrefix("Cornpult_butter", RENDER_GROUP_NORMAL);
            aBodyReanim->AssignRenderGroupToPrefix("Cornpult_kernal", RENDER_GROUP_HIDDEN);
            mState = PlantState::STATE_KERNELPULT_BUTTER;
        }
#endif
        break;
    case SeedType::SEED_MAGNETSHROOM:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->SetTruncateDisappearingFrames();
        break;
    case SeedType::SEED_SPIKEROCK:
        mPlantHealth = 450;
        TOD_ASSERT(aBodyReanim);
        break;
    case SeedType::SEED_SPROUT:
        break;
    case SeedType::SEED_FLOWERPOT:
        if (IsInPlay())
        {
            mState = PlantState::STATE_FLOWERPOT_INVULNERABLE;
            mStateCountdown = 100;
        }
        break;
    case SeedType::SEED_LILYPAD:
        if (IsInPlay())
        {
            mState = PlantState::STATE_LILYPAD_INVULNERABLE;
            mStateCountdown = 100;
        }
        break;
    case SeedType::SEED_TANGLEKELP:
        TOD_ASSERT(aBodyReanim);
        aBodyReanim->SetTruncateDisappearingFrames();
        break;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_TIMESTOPPER:
        if (IsInPlay())
        {
            mDoSpecialCountdown = 100;
        }
        break;
    case SeedType::SEED_CHERRYHOVERBOMB:
    {
        if (IsInPlay())
        {
            mDoSpecialCountdown = 100;
            mApp->PlayFoley(FoleyType::FOLEY_REVERSE_EXPLOSION);
        }

        break;
    }
    case SeedType::SEED_DUPLICATORCYCLER:
    {
        mDoSpecialCountdown = 300;
        if (IsInPlay()) 
        {
            if (mBoard->mSeedBank->mNumPackets > 0)
            {
                int targetIndex = Rand(mBoard->mSeedBank->mNumPackets);
                mDuplicatorSeedType = mBoard->mSeedBank->mSeedPackets[targetIndex].mPacketType;
                mDuplicatorImitaterType = mBoard->mSeedBank->mSeedPackets[targetIndex].mImitaterType;
            }
        }
        break;
    }
    case SeedType::SEED_NUKECUMBER:
    {
        if (IsInPlay())
        {
            mDoSpecialCountdown = 100;
            mApp->PlayFoley(FoleyType::FOLEY_REVERSE_EXPLOSION);
        }
        break;
    }
    case SeedType::SEED_ICEBERGLETTUCE:
    {
        mAnimCounter = Rand(mFrameLength * mNumFrames);
        break;
    }
#endif
    case SeedType::SEED_SUNFLOWER:
    {
        if (mApp->IsIZombieLevel())
        {
            mApp->ReanimationGet(mBodyReanimID)->mAnimTime = 0.15f;
        }
        break;
    }
    }
    
    if (
#ifdef DO_FIX_BUGS
        !((LawnApp*)gSexyAppBase)->GetDialog(Dialogs::DIALOG_ALMANAC) &&
#endif
        (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BIG_TIME) &&
        (theSeedType == SeedType::SEED_WALLNUT || theSeedType == SeedType::SEED_SUNFLOWER || theSeedType == SeedType::SEED_MARIGOLD))
    {
        mPlantHealth *= 2;
    }
    mPlantMaxHealth = mPlantHealth;

    if (mSeedType != SeedType::SEED_FLOWERPOT && IsOnBoard())
    {
        TOD_ASSERT(mBoard);
        {
            Plant* aFlowerPot = mBoard->GetFlowerPotAt(mPlantCol, mRow);
            if (aFlowerPot)
                mApp->ReanimationGet(aFlowerPot->mBodyReanimID)->mAnimRate = 0.0f;
        }
        if (mSeedType == SEED_COBCANNON)
        {
            Plant* aFlowerPot = mBoard->GetFlowerPotAt(mPlantCol + 1, mRow);
            if (aFlowerPot)
                mApp->ReanimationGet(aFlowerPot->mBodyReanimID)->mAnimRate = 0.0f;
        }
    }
}

//0x45E7C0
int Plant::CalcRenderOrder()
{
    PLANT_ORDER anOrder = PLANT_ORDER::PLANT_ORDER_NORMAL;
    RenderLayer aLayer = RenderLayer::RENDER_LAYER_PLANT;

    SeedType aSeedType = mSeedType;
    if (mSeedType == SeedType::SEED_IMITATER && mImitaterType != SeedType::SEED_NONE)
        aSeedType = mImitaterType;
    
    if (mApp && mApp->IsWallnutBowlingLevel())
    {
        aLayer = RenderLayer::RENDER_LAYER_PROJECTILE;
    }
    else if (aSeedType == SeedType::SEED_PUMPKINSHELL)
    {
        anOrder = PLANT_ORDER::PLANT_ORDER_PUMPKIN;
    }
    else if (IsFlying(aSeedType))
    {
        anOrder = PLANT_ORDER::PLANT_ORDER_FLYER;
    }
    else if (aSeedType == SeedType::SEED_FLOWERPOT || (aSeedType == SeedType::SEED_LILYPAD && mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN))
    {
        anOrder = PLANT_ORDER::PLANT_ORDER_LILYPAD;
    }

    return Board::MakeRenderOrder(aLayer, mRow, anOrder * 5 - mX + 800);
}

//0x45E860
void Plant::SetSleeping(bool theIsAsleep)
{
    if (mIsAsleep == theIsAsleep || NotOnGround())
        return;

    mIsAsleep = theIsAsleep;
    if (theIsAsleep)
    {
        float aPosX = mX + 50.0f;
        float aPosY = mY + 40.0f;
        if (mSeedType == SeedType::SEED_FUMESHROOM)
            aPosX += 12.0f;
        else if (mSeedType == SeedType::SEED_SCAREDYSHROOM)
            aPosY -= 20.0f;
        else if (mSeedType == SeedType::SEED_GLOOMSHROOM)
            aPosY -= 12.0f;

        Reanimation* aSleepReanim = mApp->AddReanimation(aPosX, aPosY, mRenderOrder + 2, ReanimationType::REANIM_SLEEPING);
        aSleepReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
        aSleepReanim->mAnimRate = RandRangeFloat(6.0f, 8.0f);
        aSleepReanim->mAnimTime = RandRangeFloat(0.0f, 0.9f);
        mSleepingReanimID = mApp->ReanimationGetID(aSleepReanim);
    }
    else
    {
        mApp->RemoveReanimation(mSleepingReanimID);
        mSleepingReanimID = ReanimationID::REANIMATIONID_NULL;
    }

    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim == nullptr)
        return;

    if (theIsAsleep)
    {
        if (!IsInPlay() && mSeedType == SeedType::SEED_SUNSHROOM)
        {
            aBodyReanim->SetFramesForLayer("anim_bigsleep");
        }
        else if (aBodyReanim->TrackExists("anim_sleep"))
        {
            float aAnimTime = aBodyReanim->mAnimTime;
            aBodyReanim->StartBlend(20);
            aBodyReanim->SetFramesForLayer("anim_sleep");
            aBodyReanim->mAnimTime = aAnimTime;
        }
        else
        {
            aBodyReanim->mAnimRate = 1.0f;
        }

        EndBlink();
    }
    else
    {
        if (!IsInPlay() && mSeedType == SeedType::SEED_SUNSHROOM)
        {
            aBodyReanim->SetFramesForLayer("anim_bigidle");
        }
        else if (aBodyReanim->TrackExists("anim_idle"))
        {
            float aAnimTime = aBodyReanim->mAnimTime;
            aBodyReanim->StartBlend(20);
            aBodyReanim->SetFramesForLayer("anim_idle");
            aBodyReanim->mAnimTime = aAnimTime;
        }

        if (aBodyReanim->mAnimRate < 2.0f && IsInPlay())
            aBodyReanim->mAnimRate = RandRangeFloat(10.0f, 15.0f);
    }
}

//0x45EB10
int Plant::GetDamageRangeFlags(PlantWeapon thePlantWeapon)
{
    switch (mSeedType)
    {
    case SeedType::SEED_CACTUS:
        return thePlantWeapon == PlantWeapon::WEAPON_SECONDARY ? 1 : 2;
    case SeedType::SEED_CHERRYBOMB:
    case SeedType::SEED_JALAPENO:
    case SeedType::SEED_COBCANNON:
    case SeedType::SEED_DOOMSHROOM:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_CHERRYHOVERBOMB:
    case SeedType::SEED_NUKECUMBER:
#endif
        return 127;
    case SeedType::SEED_MELONPULT:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_ICEBERGLETTUCE:
#endif
    case SeedType::SEED_CABBAGEPULT:
    case SeedType::SEED_KERNELPULT:
    case SeedType::SEED_WINTERMELON:
        return 13;
    case SeedType::SEED_POTATOMINE:
        return 77;
    case SeedType::SEED_SQUASH:
        return 13;
    case SeedType::SEED_PUFFSHROOM:
    case SeedType::SEED_SEASHROOM:
    case SeedType::SEED_FUMESHROOM:
    case SeedType::SEED_GLOOMSHROOM:
    case SeedType::SEED_CHOMPER:
        return 9;
    case SeedType::SEED_CATTAIL:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_BEEHIVE:
#endif
        return 11;
    case SeedType::SEED_TANGLEKELP:
        return 5;
    case SeedType::SEED_GIANT_WALLNUT:
        return 17;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_STINGER:
        return 2;
#endif
    default:
        return 1;
    }
}

//0x45EBD0
bool Plant::IsOnHighGround()
{
    return mBoard && mBoard->mGridSquareType[mPlantCol][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND;
}

//0x45EC00
void Plant::SpikeRockTakeDamage()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);

    SpikeweedAttack();

    mPlantHealth -= 50;
    mEatenFlashCountdown = max(mEatenFlashCountdown, 25);
    if (mPlantHealth <= 300)
    {
        aBodyReanim->AssignRenderGroupToTrack("bigspike3", RENDER_GROUP_HIDDEN);
    }
    if (mPlantHealth <= 150)
    {
        aBodyReanim->AssignRenderGroupToTrack("bigspike2", RENDER_GROUP_HIDDEN);
    }
    if (mPlantHealth <= 0)
    {
        //mApp->PlayFoley(FoleyType::FOLEY_SQUISH);
        //Die();

        mPlantHealth = 1;
        mBoard->mPlantsEaten++;
        Squish();
    }
}

bool Plant::IsSpiky()
{
    return mSeedType == SeedType::SEED_SPIKEWEED || mSeedType == SeedType::SEED_SPIKEROCK;
}

//0x45ED00
void Plant::DoRowAreaDamage(int theDamage, unsigned int theDamageFlags)
{
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
    Rect aAttackRect = GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        int aDiffY = (aZombie->mZombieType == ZombieType::ZOMBIE_BOSS) ? 0 : (aZombie->mRow - mRow);
        if (mSeedType == SeedType::SEED_GLOOMSHROOM)
        {
            if (aDiffY < -1 || aDiffY > 1)
                continue;
        }
        else if (aDiffY)
            continue;

        if (aZombie->mOnHighGround == IsOnHighGround() && (aZombie->EffectedByDamage(aDamageRangeFlags) 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
                || aZombie->mZombieType == ZombieType::ZOMBIE_DOG && (mSeedType == SeedType::SEED_YAMPOLINE || IsSpiky()) && aZombie->EffectedByDamage(13)
#endif
                
            ))
        {
            if (aZombie->mZombiePhase == ZombiePhase::PHASE_ZAMBONI_FLAT && IsSpiky()) {
                continue;
            }

            Rect aZombieRect = aZombie->GetZombieRect();
            if (aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_POST_VAULT || aZombie->mZombieType == ZombieType::ZOMBIE_POGO && aZombie->mZombiePhase == ZombiePhase::PHASE_ZOMBIE_NORMAL &&
                (mSeedType == SeedType::SEED_SPIKEWEED || mSeedType == SeedType::SEED_SPIKEROCK))
                aZombieRect.mWidth += 50;
            if (GetRectOverlap(aAttackRect, aZombieRect) > 0)
            {
                int aDamage = theDamage;
                if ((aZombie->mZombieType == ZombieType::ZOMBIE_ZAMBONI || aZombie->mZombieType == ZombieType::ZOMBIE_CATAPULT) &&
                    (TestBit(theDamageFlags, DamageFlags::DAMAGE_SPIKE)))
                {
                    if (Rand(1000) == 0) {
                        aZombie->mZombiePhase = ZombiePhase::PHASE_ZAMBONI_FLAT;
                    }
                    else {
                        aDamage = 1800;
                    }

                    if (aZombie->mZombiePhase != ZombiePhase::PHASE_ZAMBONI_FLAT)
                    {
                        if (mSeedType == SeedType::SEED_SPIKEROCK)
                        {
                            SpikeRockTakeDamage();
                        }
                        else
                        {
                            Die();
                        }
                    }
                }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
                if (mSeedType == SeedType::SEED_YAMPOLINE && aZombie->mZombieType != ZombieType::ZOMBIE_BOSS && aZombie->mYampolineCounter == 0 && 
                    !aZombie->IsDeadOrDying() && aZombie->mHasHead && aZombie->mZombieType != ZombieType::ZOMBIE_BOSS && 
                    aZombie->mZombieType != ZombieType::ZOMBIE_CATAPULT && aZombie->mZombieType != ZombieType::ZOMBIE_ZAMBONI)
                {
                    aZombie->mYampolineCounter = 90;
                    aZombie->UpdateAnimSpeed();

                    if (aZombie->IsWalkingBackwards())  
                    {
                        aZombie->mYampolineVelX = -150;

                        Plant* aTarget = nullptr;
                        aTarget = mBoard->GetTopPlantAt(mPlantCol + 1, mRow, PlantPriority::TOPPLANT_EATING_ORDER);
                        if (aTarget && aTarget->mSeedType == SeedType::SEED_COBCANNON) {
                            aZombie->mYampolineVelX -= 80;
                        }
                    }
                    else                                
                    {
                        aZombie->mYampolineVelX = 150;

                        Plant* aTarget = nullptr;
                        aTarget = mBoard->GetTopPlantAt(mPlantCol - 1, mRow, PlantPriority::TOPPLANT_EATING_ORDER);
                        if (aTarget && aTarget->mSeedType == SeedType::SEED_COBCANNON) {
                            aZombie->mYampolineVelX += 80;
                        }
                    }

                    if (aZombie->mZombieType == ZombieType::ZOMBIE_DOG_WALKER) {
                        aZombie->UpdateZombieDogWalker();
                    } else if (aZombie->mZombieType == ZombieType::ZOMBIE_DOG) {
                        aZombie->UpdateZombieDog();
                    }

                    aZombie->mYampolineVelX /= 90;
                    aZombie->mYampolineVelZ = 3.6f;
                    
                       
                    continue;
                }
#endif

                aZombie->TakeDamage(aDamage, theDamageFlags);
                mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
            }
        }
    }
}

//0x45EEA0
TodParticleSystem* Plant::AddAttachedParticle(int thePosX, int thePosY, int theRenderPosition, ParticleEffect theEffect)
{
    TodParticleSystem* aParticle = mApp->ParticleTryToGet(mParticleID);
    if (aParticle)
        aParticle->ParticleSystemDie();

    TodParticleSystem* aNewParticle = mApp->AddTodParticle(thePosX, thePosY, theRenderPosition, theEffect);
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aNewParticle)
    {
        if (aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)  
        {
            aNewParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mParticleID = mApp->ParticleGetID(aNewParticle);
    }

    return aNewParticle;
}

//0x45EF10
bool Plant::FindTargetAndFire(int theRow, PlantWeapon thePlantWeapon)
{
    Zombie* aZombie = FindTargetZombie(theRow, thePlantWeapon);
    if (aZombie == nullptr)
        return false;

    EndBlink();
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    Reanimation* aHeadReanim = mApp->ReanimationTryToGet(mHeadReanimID);

    if (mSeedType == SeedType::SEED_SPLITPEA && thePlantWeapon == PlantWeapon::WEAPON_SECONDARY)
    {
        Reanimation* aHeadReanim2 = mApp->ReanimationGet(mHeadReanimID2);
        aHeadReanim2->StartBlend(20);
        aHeadReanim2->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
        aHeadReanim2->mAnimRate = 45.0f;
        aHeadReanim2->SetFramesForLayer("anim_splitpea_shooting");
        mShootingCounter = 51;
    }
    else if (aHeadReanim && aHeadReanim->TrackExists("anim_shooting"))
    {
        const char* aTrackToPlay = "anim_shooting";

#ifdef _DS_MINIGAMES
        if (mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
        {
            if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED)
                aTrackToPlay = "anim_shooting_exhausted";
            else if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED_2)
                aTrackToPlay = "anim_shooting_exhausted2";
        }
#endif
        aHeadReanim->StartBlend(20);
        aHeadReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
        aHeadReanim->mAnimRate = 35.0f;
        aHeadReanim->SetFramesForLayer(aTrackToPlay);
        
        if ((mSeedType == SeedType::SEED_REPEATER || mSeedType == SeedType::SEED_LEFTPEATER))
        {
            aHeadReanim->mAnimRate = 45.0f;
            mShootingCounter = 51;
        }
        else if (mSeedType == SeedType::SEED_SPLITPEA)
        {
            mShootingCounter = 51;
        }
        else if (mSeedType == SeedType::SEED_GATLINGPEA)
        {
            aHeadReanim->mAnimRate = 38.0f;
            mShootingCounter = 100;
        }
        else
        {
            mShootingCounter = 33;
        }
#ifdef _MOBILE_MINIGAMES
        if (mState == PlantState::STATE_HEAT_WAVE_POWERED)
            mShootingCounter = 26;
#endif
    }
    else if (mState == PlantState::STATE_CACTUS_HIGH)
    {
        PlayBodyReanim("anim_shootinghigh", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 35.0f);
        mShootingCounter = 23;
    }
    else if (mSeedType == SeedType::SEED_GLOOMSHROOM)
    {
        PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 14.0f);
        mShootingCounter = 200;
    }
    else if (mSeedType == SeedType::SEED_CATTAIL)
    {
        PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 30.0f);
        mShootingCounter = 50;
    }
    else if (aBodyReanim && aBodyReanim->TrackExists("anim_shooting"))
    {
        PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 35.0f);

        switch (mSeedType)
        {
        case SeedType::SEED_FUMESHROOM:     mShootingCounter = 50;  break;
        case SeedType::SEED_PUFFSHROOM:     mShootingCounter = 29;  break;
        case SeedType::SEED_SCAREDYSHROOM:  mShootingCounter = 25;  break;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        case SeedType::SEED_ICEBERGLETTUCE:
#endif
        case SeedType::SEED_CABBAGEPULT:    mShootingCounter = 32;  break;
        case SeedType::SEED_MELONPULT:
        case SeedType::SEED_WINTERMELON:    mShootingCounter = 36;  break;
        case SeedType::SEED_KERNELPULT:
        {
#ifndef _HAS_KERNELPULT_BUTTER_IDLE
            if (Sexy::Rand(4) == 0)
            {

            
                aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_butter", RENDER_GROUP_NORMAL);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_kernal", RENDER_GROUP_HIDDEN);
                mState = PlantState::STATE_KERNELPULT_BUTTER;
            }
#endif
            mShootingCounter = 30;
            break;
        }
        case SeedType::SEED_CACTUS:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        case SeedType::SEED_STINGER:
        case SeedType::SEED_BEEHIVE:        mShootingCounter = 35;  break;
#endif
        default:                            mShootingCounter = 29;  break;
        }
    }
    else
        Fire(aZombie, theRow, thePlantWeapon);

    return true;
}

//0x45F2A0
void Plant::LaunchThreepeater()
{
    int rowAbove = mRow - 1;
    int rowBelow = mRow + 1;

    if ((FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY)) ||
        (mBoard->RowCanHaveZombies(rowAbove) && FindTargetZombie(rowAbove, PlantWeapon::WEAPON_PRIMARY)) ||
        (mBoard->RowCanHaveZombies(rowBelow) && FindTargetZombie(rowBelow, PlantWeapon::WEAPON_PRIMARY)))
    {
        Reanimation* aHeadReanim1 = mApp->ReanimationGet(mHeadReanimID);
        Reanimation* aHeadReanim2 = mApp->ReanimationGet(mHeadReanimID2);
        Reanimation* aHeadReanim3 = mApp->ReanimationGet(mHeadReanimID3);

        if (mBoard->RowCanHaveZombies(rowBelow))
        {
            aHeadReanim1->StartBlend(10);
            aHeadReanim1->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
            aHeadReanim1->mAnimRate = 20.0f;
            aHeadReanim1->SetFramesForLayer("anim_shooting1");
        }

        aHeadReanim2->StartBlend(10);
        aHeadReanim2->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
        aHeadReanim2->mAnimRate = 20.0f;
        aHeadReanim2->SetFramesForLayer("anim_shooting2");

        if (mBoard->RowCanHaveZombies(rowAbove))
        {
            aHeadReanim3->StartBlend(10);
            aHeadReanim3->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
            aHeadReanim3->mAnimRate = 20.0f;
            aHeadReanim3->SetFramesForLayer("anim_shooting3");
        }

        mShootingCounter = 35;
    }
}

//0x45F470
bool Plant::FindStarFruitTarget()
{
    if (mRecentlyEatenCountdown > 0)
        return true;

    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
    int aCenterStarX = mX + 40;
    int aCenterStarY = mY + 40;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        Rect aZombieRect = aZombie->GetZombieRect();
        if (aZombie->EffectedByDamage(aDamageRangeFlags))
        {
            if (aZombie->mZombieType == ZombieType::ZOMBIE_BOSS && mPlantCol >= 5)
                return true;

            if (aZombie->mRow == mRow)
            {
                if (aZombieRect.mX + aZombieRect.mWidth < aCenterStarX)
                    return true;
            }
            else
            {
                if (aZombie->mZombieType == ZombieType::ZOMBIE_DIGGER)
                    aZombieRect.mX += 10;

                float aProjectileTime = Distance2D(aCenterStarX, aCenterStarY, aZombieRect.mX + aZombieRect.mWidth / 2, aZombieRect.mY + aZombieRect.mHeight / 2) / 3.33f;
                int aZombieHitX = aZombie->ZombieTargetLeadX(aProjectileTime) - aZombieRect.mWidth / 2;
                if ((aZombieHitX + aZombieRect.mWidth > aCenterStarX) && (aZombieHitX < aCenterStarX))
                    return true;

                int aCenterZombieX = aZombieHitX + aZombieRect.mWidth / 2;
                int aCenterZombieY = aZombieRect.mY + aZombieRect.mHeight / 2;
                float angle = RAD_TO_DEG(atan2(aCenterZombieY - aCenterStarY, aCenterZombieX - aCenterStarX));
                if (abs(aZombie->mRow - mRow) < 2)
                {
                    if ((angle > 20.0f && angle < 40.0f) || (angle < -25.0f && angle > -45.0f))
                        return true;
                }
                else
                {
                    if ((angle > 25.0f && angle < 35.0f) || (angle < -28.0f && angle > -38.0f))
                        return true;
                }
            }
        }
    }

    return false;
}

//0x45F6E0
void Plant::LaunchStarFruit()
{
    if (FindStarFruitTarget())
    {
        PlayBodyReanim("anim_shoot", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 28.0f);
        mShootingCounter = 40;
    }
}

//0x45F720
void Plant::StarFruitFire()
{
    mApp->PlayFoley(FoleyType::FOLEY_THROW);

    float aShootAngleX = cos(DEG_TO_RAD(30.0f)) * 3.33f;
    float aShootAngleY = sin(DEG_TO_RAD(30.0f)) * 3.33f;
    for (int i = 0; i < 5; i++)
    {
        Projectile* aProjectile = mBoard->AddProjectile(mX + 25, mY + 25, mRenderOrder - 1, mRow, ProjectileType::PROJECTILE_STAR);
        {
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                aProjectile->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }
        aProjectile->mDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
        aProjectile->mMotionType = ProjectileMotion::MOTION_STAR;

        switch (i)
        {
        case 0:     aProjectile->mVelX = -3.33f;         aProjectile->mVelY = 0.0f;             break;
        case 1:     aProjectile->mVelX = 0.0f;          aProjectile->mVelY = 3.33f;             break;
        case 2:     aProjectile->mVelX = 0.0f;          aProjectile->mVelY = -3.33f;            break;
        case 3:     aProjectile->mVelX = aShootAngleX;  aProjectile->mVelY = aShootAngleY;      break;
        case 4:     aProjectile->mVelX = aShootAngleX;  aProjectile->mVelY = -aShootAngleY;     break;
        default:    TOD_ASSERT();                                                               break;
        }
    }
}

//0x45F8A0
void Plant::UpdateShooter()
{
    mLaunchCounter--;
    if (mLaunchCounter <= 0)
    {
        mLaunchCounter = mLaunchRate - Sexy::Rand(15);
#ifdef _MOBILE_MINIGAMES
        if (mState == PlantState::STATE_HEAT_WAVE_POWERED)
            mLaunchCounter = 26;
#endif

        if (mSeedType == SeedType::SEED_THREEPEATER)
        {
            LaunchThreepeater();
        }
        else if (mSeedType == SeedType::SEED_STARFRUIT)
        {
            LaunchStarFruit();
        }
        else if (mSeedType == SeedType::SEED_SPLITPEA)
        {
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_SECONDARY);
        }
        else if (mSeedType == SeedType::SEED_CACTUS)
        {
            if (mState == PlantState::STATE_CACTUS_HIGH)
            {
                FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
            }
            else if (mState == PlantState::STATE_CACTUS_LOW)
            {
                FindTargetAndFire(mRow, PlantWeapon::WEAPON_SECONDARY);
            }
        }
        else
        {
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
        }
    }

    if (mLaunchCounter == 50 && mSeedType == SeedType::SEED_CATTAIL)
    {
        FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
    }

    /*if (mLaunchCounter == 25)
    {
        if (mSeedType == SeedType::SEED_REPEATER || mSeedType == SeedType::SEED_LEFTPEATER)
        {
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
        }
        else if (mSeedType == SeedType::SEED_SPLITPEA)
        {
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_PRIMARY);
            FindTargetAndFire(mRow, PlantWeapon::WEAPON_SECONDARY);
        }
    }*/
}

//0x45F980
bool Plant::MakesSun()
{
    return mSeedType == SeedType::SEED_SUNFLOWER || mSeedType == SeedType::SEED_TWINSUNFLOWER || mSeedType == SeedType::SEED_SUNSHROOM;
}

//0x45F9A0
void Plant::UpdateProductionPlant()
{
    if (!IsInPlay() || mApp->IsIZombieLevel() || mApp->mGameMode == GameMode::GAMEMODE_UPSELL || mApp->mGameMode == GameMode::GAMEMODE_INTRO)
        return;

    if (mBoard->HasLevelAwardDropped())
        return;

    if (mSeedType == SeedType::SEED_MARIGOLD && mBoard->mCurrentWave == mBoard->mNumWaves)
    {
        if (mState != PlantState::STATE_MARIGOLD_ENDING)
        {
            mState = PlantState::STATE_MARIGOLD_ENDING;
            mStateCountdown = 6000;
        }
        else if (mStateCountdown <= 0)
            return;
    }

    if (mApp->IsLastStand() && mBoard->mChallenge->mChallengeState != ChallengeState::STATECHALLENGE_LAST_STAND_ONSLAUGHT)
        return;

    mLaunchCounter--;
    if (mLaunchCounter <= 100)
    {
        int aFlashCountdown = TodAnimateCurve(100, 0, mLaunchCounter, 0, 100, TodCurves::CURVE_LINEAR);
        mEatenFlashCountdown = max(mEatenFlashCountdown, aFlashCountdown);
    }
    if (mLaunchCounter <= 0)
    {
        mLaunchCounter = RandRangeInt(mLaunchRate - 150, mLaunchRate);
        mApp->PlayFoley(FoleyType::FOLEY_SPAWN_SUN);

        if (mSeedType == SeedType::SEED_SUNSHROOM)
        {
            if (mState == PlantState::STATE_SUNSHROOM_SMALL)
            {
                Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SMALLSUN, CoinMotion::COIN_MOTION_FROM_PLANT);
                Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                    aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                   
                }
            }
            else
            {
                Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SUN, CoinMotion::COIN_MOTION_FROM_PLANT);
                Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                    aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                }
            }
        }
        else if (mSeedType == SeedType::SEED_SUNFLOWER)
        {
            Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SUN, CoinMotion::COIN_MOTION_FROM_PLANT);
            Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }
        else if (mSeedType == SeedType::SEED_TWINSUNFLOWER)
        {
            Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SUN, CoinMotion::COIN_MOTION_FROM_PLANT);
            Coin* aCoin2 = mBoard->AddCoin(mX, mY, CoinType::COIN_SUN, CoinMotion::COIN_MOTION_FROM_PLANT);
            Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
            Reanimation* aCoinReanim2 = mApp->ReanimationTryToGet(aCoin2->mReanimationID);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aBodyReanim && aCoinReanim && aCoinReanim2 && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                aCoin2->mFilterEffect = aCoinReanim2->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }
        else if (mSeedType == SeedType::SEED_MARIGOLD)
        {
            Coin* aCoin = mBoard->AddCoin(mX, mY, (Sexy::Rand(100) < 10) ? CoinType::COIN_GOLD : CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
            Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }

        if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BIG_TIME)
        {
            if (mSeedType == SeedType::SEED_SUNFLOWER)
            {
                Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SUN, CoinMotion::COIN_MOTION_FROM_PLANT);
                Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                    aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                }
            }
            else if (mSeedType == SeedType::SEED_MARIGOLD)
            {
                Coin* aCoin = mBoard->AddCoin(mX, mY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
                Reanimation* aCoinReanim = mApp->ReanimationTryToGet(aCoin->mReanimationID);
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim && aCoinReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                    aCoin->mFilterEffect = aCoinReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                }
            }
        }
    }
}

//0x45FB70
void Plant::UpdateSunShroom()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (mState == PlantState::STATE_SUNSHROOM_SMALL)
    {
        if (mStateCountdown == 0)
        {
            PlayBodyReanim("anim_grow", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 10, 12.0f);
            mState = PlantState::STATE_SUNSHROOM_GROWING;
            mApp->PlayFoley(FoleyType::FOLEY_PLANTGROW);
        }

        UpdateProductionPlant();
    }
    else if (mState == PlantState::STATE_SUNSHROOM_GROWING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            PlayBodyReanim("anim_bigidle", ReanimLoopType::REANIM_LOOP, 10, RandRangeFloat(12.0f, 15.0f));
            mState = PlantState::STATE_SUNSHROOM_BIG;
        }
    }
    else
    {
        UpdateProductionPlant();
    }
}

//0x45FC70
void Plant::UpdateGraveBuster()
{
    if (mState == PlantState::STATE_GRAVEBUSTER_LANDING)
    {
        if (mApp->ReanimationGet(mBodyReanimID)->mLoopCount > 0)
        {
            PlayBodyReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 10, 12.0f);
            mStateCountdown = 400;
            mState = PlantState::STATE_GRAVEBUSTER_EATING;
            AddAttachedParticle(mX + 40, mY + 40, mRenderOrder + 4, ParticleEffect::PARTICLE_GRAVE_BUSTER);
        }
    }
    else if (mState == PlantState::STATE_GRAVEBUSTER_EATING && mStateCountdown == 0)
    {
        GridItem* aGraveStone = mBoard->GetGraveStoneAt(mPlantCol, mRow);
        if (aGraveStone)
        {
            aGraveStone->GridItemDie();
            mBoard->mGravesCleared++;
#ifdef _HAS_UNUSED_ACHIEVEMENTS
            if (mBoard->mGravesCleared == 10) {
                ReportAchievement::GiveAchievement(mApp, AchievementId::DisrespectTheDead, true);
            }
#endif
        }

        mApp->AddTodParticle(mX + 40, mY + 40, mRenderOrder + 4, ParticleEffect::PARTICLE_GRAVE_BUSTER_DIE);
        Die();
        mBoard->DropLootPiece(mX + 40, mY, 12);
    }
}

//0x45FD90
void Plant::PlayBodyReanim(const char* theTrackName, ReanimLoopType theLoopType, int theBlendTime, float theAnimRate)
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);

    if (theBlendTime > 0)
        aBodyReanim->StartBlend(theBlendTime);
    if (theAnimRate > 0.0f)
        aBodyReanim->mAnimRate = theAnimRate;

    aBodyReanim->mLoopType = theLoopType;
    aBodyReanim->mLoopCount = 0;
    aBodyReanim->SetFramesForLayer(theTrackName);
}

//0x45FE20
void Plant::UpdatePotato()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);

    if (mState == PlantState::STATE_NOTREADY)
    {
        if (mStateCountdown == 0)
        {
            mApp->AddTodParticle(mX + mWidth / 2, mY + mHeight / 2, mRenderOrder, ParticleEffect::PARTICLE_POTATO_MINE_RISE);
            PlayBodyReanim("anim_rise", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 18.0f);
            mState = PlantState::STATE_POTATO_RISING;
            mApp->PlayFoley(FoleyType::FOLEY_DIRT_RISE);
        }
    }
    else if (mState == PlantState::STATE_POTATO_RISING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            float aRate = RandRangeFloat(12.0f, 15.0f);
            PlayBodyReanim("anim_armed", ReanimLoopType::REANIM_LOOP, 0, aRate);

            Reanimation* aLightReanim = mApp->AddReanimation(0.0f, 0.0f, mRenderOrder + 2, GetPlantDefinition(mSeedType).mReanimationType);
            aLightReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
            aLightReanim->mAnimRate = aRate - 2.0f;
            aLightReanim->SetFramesForLayer("anim_glow");
            aLightReanim->mFrameCount = 10;
            aLightReanim->ShowOnlyTrack("anim_glow");
            aLightReanim->SetTruncateDisappearingFrames("anim_glow", false);
            mLightReanimID = mApp->ReanimationGetID(aLightReanim);
            aLightReanim->AttachToAnotherReanimation(aBodyReanim, "anim_light");
            if (aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
                aLightReanim->mFilterEffect = FilterEffect::FILTER_EFFECT_WASHED_OUT;

            mState = PlantState::STATE_POTATO_ARMED;
            mBlinkCountdown = 400 + Sexy::Rand(4000);
        }
    }
    else if (mState == PlantState::STATE_POTATO_ARMED)
    {
        if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY))
        {
            DoSpecial();
        }
        else
        {
            Reanimation* aLightReanim = mApp->ReanimationTryToGet(mLightReanimID);
            if (aLightReanim)
            {
                aLightReanim->mFrameCount = TodAnimateCurve(200, 50, DistanceToClosestZombie(), 10, 3, TodCurves::CURVE_LINEAR);
            }
        }
    }
}

//0x460060
void Plant::UpdateTanglekelp()
{
    if (mState != PlantState::STATE_TANGLEKELP_GRABBING)
    {
        Zombie* aZombie = FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY);
        if (aZombie)
        {
            mApp->PlayFoley(FoleyType::FOLEY_FLOOP);
            mState = PlantState::STATE_TANGLEKELP_GRABBING;
            mStateCountdown = 100;
            aZombie->PoolSplash(false);

            float aVinesPosX = -13.0f;
            float aVinesPosY = 15.0f;
            if (aZombie->mZombieType == ZombieType::ZOMBIE_SNORKEL)
            {
                aVinesPosX = -43.0f;
                aVinesPosY = 55.0f;
            }
            if (aZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_RIDING)
            {
                aVinesPosX = -20.0f;
                aVinesPosY = 37.0f;
            }
            Reanimation* aGrabReanim = aZombie->AddAttachedReanim(aVinesPosX, aVinesPosY, ReanimationType::REANIM_TANGLEKELP);
            if (aGrabReanim)
            {
                aGrabReanim->SetFramesForLayer("anim_grab");
                aGrabReanim->mAnimRate = 24.0f;
                aGrabReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;

                if (mImitaterType == SeedType::SEED_IMITATER)
                {
                    aGrabReanim->mFilterEffect = FilterEffect::FILTER_EFFECT_WASHED_OUT;
                }
            }

            mTargetZombieID = mBoard->ZombieGetID(aZombie);
        }
    }
    else
    {
        if (mStateCountdown == 50)
        {
            Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
            if (aZombie)
            {
                aZombie->DragUnder();
                aZombie->PoolSplash(false);
            }
        }

        if (mStateCountdown == 20)
        {
            int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PARTICLE, mRow, 0);
            Reanimation* aSplashReanim = mApp->AddReanimation(mX - 23, mY + 7, aRenderPosition, ReanimationType::REANIM_SPLASH);
            aSplashReanim->OverrideScale(1.3f, 1.3f);

            mApp->AddTodParticle(mX + 31, mY + 64, aRenderPosition, ParticleEffect::PARTICLE_PLANTING_POOL);
            mApp->PlayFoley(FoleyType::FOLEY_ZOMBIE_ENTERING_WATER);
        }

        if (mStateCountdown == 0)
        {
            Die();

            Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
            if (aZombie)
            {
                aZombie->DieWithLoot();
            }
        }
    }
}

//0x460320
void Plant::SpikeweedAttack()
{
    TOD_ASSERT(IsSpiky());

    if (mState != PlantState::STATE_SPIKEWEED_ATTACKING)
    {
        PlayBodyReanim("anim_attack", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 18.0f);
        mApp->PlaySample(SOUND_THROW);
        
        mState = PlantState::STATE_SPIKEWEED_ATTACKING;
        mStateCountdown = 100;
    }
}

//0x460370
void Plant::UpdateSpikeweed()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (mState == PlantState::STATE_SPIKEWEED_ATTACKING)
    {
        if (mStateCountdown == 0)
        {
            mState = PlantState::STATE_NOTREADY;
        }
        else if (mSeedType == SeedType::SEED_SPIKEROCK)
        {
            if (mStateCountdown == 69 || mStateCountdown == 33)
            {
                DoRowAreaDamage(20, 33U);
            }
        }
        else if (mStateCountdown == 75)
        {
            DoRowAreaDamage(20, 33U);
        }

        if (aBodyReanim->mLoopCount > 0)
        {
            PlayIdleAnim(RandRangeFloat(12.0f, 15.0f));
        }
    }
    else if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY))
    {
        SpikeweedAttack();
    }
}

void Plant::UpdateYampoline()
{
    if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY))
    {
        DoRowAreaDamage(0, 33U);
        mApp->PlayFoley(FoleyType::FOLEY_POLEVAULT);
    }
}

//0x460420
void Plant::UpdateScaredyShroom()
{
    if (mShootingCounter > 0)
        return;

    bool aHasZombieNearby = false;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        Rect aZombieRect = aZombie->GetZombieRect();
        int aDiffY = (aZombie->mZombieType == ZombieType::ZOMBIE_BOSS) ? 0 : (aZombie->mRow - mRow);
        if (!aZombie->mMindControlled && !aZombie->IsDeadOrDying() && aDiffY <= 1 && aDiffY >= -1 && GetOvalRectOverlap(mX + 40, mY + 40.0f, 80, 120, aZombieRect))
        {
            aHasZombieNearby = true;
            break;
        }
    }

    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (mState == PlantState::STATE_READY)
    {
        if (aHasZombieNearby)
        {
            mState = PlantState::STATE_SCAREDYSHROOM_LOWERING;
            PlayBodyReanim("anim_scared", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 10, 10.0f);
        }
    }
    else if (mState == PlantState::STATE_SCAREDYSHROOM_LOWERING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_SCAREDYSHROOM_SCARED;
            PlayBodyReanim("anim_scaredidle", ReanimLoopType::REANIM_LOOP, 10, 0.0f);
        }
    }
    else if (mState == PlantState::STATE_SCAREDYSHROOM_SCARED)
    {
        if (!aHasZombieNearby)
        {
            mState = PlantState::STATE_SCAREDYSHROOM_RAISING;

            float aAnimRate = RandRangeFloat(7.0f, 12.0f);
            PlayBodyReanim("anim_grow", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 10, aAnimRate);
        }
    }
    else if (mState == PlantState::STATE_SCAREDYSHROOM_RAISING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_READY;

            float aAnimRate = RandRangeFloat(10.0f, 15.0f);
            PlayIdleAnim(aAnimRate);
        }
    }

    if (mState != PlantState::STATE_READY)
    {
        mLaunchCounter = mLaunchRate;
    }
}

//0x460610
void Plant::UpdateTorchwood()
{
    Rect aAttackRect = GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);

    Projectile* aProjectile = nullptr;
    while (mBoard->IterateProjectiles(aProjectile))
    {
        if ((aProjectile->mRow == mRow) && 
            (aProjectile->mProjectileType == ProjectileType::PROJECTILE_PEA || aProjectile->mProjectileType == ProjectileType::PROJECTILE_SNOWPEA))
        {
            Rect aProjectileRect = aProjectile->GetProjectileRect();
            if (GetRectOverlap(aAttackRect, aProjectileRect) >= 10)
            {
                aProjectile->mFilterEffect = FilterEffect::FILTER_EFFECT_NONE;
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim) {
                    if (aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
                        aProjectile->mFilterEffect = aBodyReanim->mFilterEffect;
                    }
                }
                if (aProjectile->mProjectileType == ProjectileType::PROJECTILE_PEA)
                {
                    aProjectile->ConvertToFireball(mPlantCol);
                    if (aProjectile->mMotionType == ProjectileMotion::MOTION_BACKWARDS) {
                        aProjectile->mRenderOrder = Board::MakeRenderOrder(RENDER_LAYER_PLANT, mRow, RENDER_LAYER_PLANT * 5 - 0 + 800);
                    }
                }
                else if (aProjectile->mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
                {
                    aProjectile->ConvertToPea(mPlantCol);
                }
            }
        }
    }
}

//0x4606F0
void Plant::DoSquashDamage()
{
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
    Rect aAttackRect = GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        if ((aZombie->mRow == mRow || aZombie->mZombieType == ZombieType::ZOMBIE_BOSS) && aZombie->EffectedByDamage(aDamageRangeFlags))
        {
            Rect aZombieRect = aZombie->GetZombieRect();
            if (GetRectOverlap(aAttackRect, aZombieRect) > (aZombie->mZombieType == ZombieType::ZOMBIE_FOOTBALL 
#ifdef _HAS_NEW_GIGA_ZOMBIES
                || aZombie->mZombieType == ZombieType::ZOMBIE_BLACK_FOOTBALL 
#endif
                ? -20 : 0))
            {
                aZombie->TakeDamage(1800, aZombie->mInPool ? 18U : 0U); //18U 

               /* if (aZombie->CanLoseBodyParts() && aZombie->mHasArm && aZombie->mBodyHealth < 2 * aZombie->mBodyMaxHealth / 3)
                {
                    aZombie->DropArm(0U);
                }*/
            }
        }
    }
}

//0x4607E0
Zombie* Plant::FindSquashTarget()
{
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);

    Rect aAttackRect = GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);

    int midX = mX + (mWidth / 2);
    int halfWidth = aAttackRect.mWidth / 2;
    aAttackRect.mX = midX - halfWidth;

    int aClosestRange = 0;
    Zombie* aClosestZombie = nullptr;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        if ((aZombie->mRow != mRow && aZombie->mZombieType != ZombieType::ZOMBIE_BOSS) ||
            !aZombie->mHasHead || aZombie->IsTangleKelpTarget() ||
            !aZombie->EffectedByDamage(aDamageRangeFlags))
        {
            continue;
        }

        bool IsZombieLeaping =
            (aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_PRE_VAULT && aZombie->GetZombieRect().mX >= mX + 20) ||
            (aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_IN_VAULT ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_INTO_POOL ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_INTO_POOL ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_RIDING ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_IN_JUMP ||
                aZombie->IsBobsledTeamWithSled());

        if (IsZombieLeaping)
            continue;

        Rect aZombieRect = aZombie->GetZombieRect();
        int aRange = -GetRectOverlap(aAttackRect, aZombieRect);
        if (aRange > (aZombie->mIsEating ? 110 : 70))
            continue;

        int aPlantX = aAttackRect.mX;
        if (aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_POST_VAULT ||
            aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_PRE_VAULT ||
            aZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_WALKING_IN_POOL ||
            aZombie->mZombieType == ZombieType::ZOMBIE_IMP ||
            aZombie->mZombieType == ZombieType::ZOMBIE_FOOTBALL ||
            mApp->IsScaryPotterLevel())
        {
            aPlantX -= 60;
        }

        if (mBoard->ZombieGetID(aZombie) == mTargetZombieID)
            return aZombie; 

        if (!aClosestZombie || aRange < aClosestRange)
        {
            aClosestZombie = aZombie;
            aClosestRange = aRange;
        }
    }

    return aClosestZombie;

}

//0x4609D0
void Plant::UpdateSquash()
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    TOD_ASSERT(aBodyReanim);

    if (mState == PlantState::STATE_NOTREADY)
    {
        Zombie* aZombie = FindSquashTarget();
        if (aZombie)
        {
            mTargetZombieID = mBoard->ZombieGetID(aZombie);
            mTargetX = aZombie->ZombieTargetLeadX(0.0f) - mWidth / 2;
            mState = PlantState::STATE_SQUASH_LOOK;
            mStateCountdown = 80;
            PlayBodyReanim(mTargetX < mX ? "anim_lookleft" : "anim_lookright", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 10, 24.0f);
            mApp->PlayFoley(FoleyType::FOLEY_SQUASH_HMM);
        }
    }
    else if (mState == PlantState::STATE_SQUASH_LOOK)
    {
        if (mStateCountdown <= 0)
        {
            PlayBodyReanim("anim_jumpup", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 24.0f);
            mState = PlantState::STATE_SQUASH_PRE_LAUNCH;
            mStateCountdown = 30;
        }
    }
    else if (mState == PlantState::STATE_SQUASH_PRE_LAUNCH)
    {
        if (mStateCountdown <= 0)
        {
            Zombie* aZombie = FindSquashTarget();
            if (aZombie)
            {
                mTargetX = aZombie->ZombieTargetLeadX(30.0f) - mWidth / 2;
            }

            mState = PlantState::STATE_SQUASH_RISING;
            mStateCountdown = 50;
            mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PARTICLE, mRow, 0);
        }
    }
    else
    {
        int aTargetCol = mBoard->PixelToGridXKeepOnBoard(mTargetX, mY);
        int aDestY = mBoard->GridToPixelY(aTargetCol, mRow) + 8;

        if (mState == PlantState::STATE_SQUASH_RISING)
        {
            mX = TodAnimateCurve(50, 20, mStateCountdown, mBoard->GridToPixelX(mPlantCol, mStartRow), mTargetX, TodCurves::CURVE_EASE_IN_OUT);
            mY = TodAnimateCurve(50, 20, mStateCountdown, mBoard->GridToPixelY(mPlantCol, mStartRow), aDestY - 120, TodCurves::CURVE_EASE_IN_OUT);

            if (mStateCountdown == 0)
            {
                PlayBodyReanim("anim_jumpdown", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 60.0f);
                mState = PlantState::STATE_SQUASH_FALLING;
                mStateCountdown = 10;
            }
        }
        else if (mState == PlantState::STATE_SQUASH_FALLING)
        {
            mY = TodAnimateCurve(10, 0, mStateCountdown, aDestY - 120, aDestY, TodCurves::CURVE_EASE_IN_OUT);

            if (mStateCountdown == 5)
            {
                DoSquashDamage();
            }

            if (mStateCountdown == 0)
            {
                if (mBoard->IsPoolSquare(aTargetCol, mRow))
                {
                    mApp->AddReanimation(mX - 11, mY + 20, mRenderOrder + 1, ReanimationType::REANIM_SPLASH);
                    mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
                    mApp->PlaySample(SOUND_ZOMBIESPLASH);

                    Die();
                }
                else
                {
                    mState = PlantState::STATE_SQUASH_DONE_FALLING;
                    mStateCountdown = 100;

                    mBoard->ShakeBoard(1, 4);
                    mApp->PlayFoley(FoleyType::FOLEY_THUMP);
                    float aOffsetY = mBoard->StageHasRoof() ? 69.0f : 80.0f;
                    mApp->AddTodParticle(mX + 40, mY + aOffsetY, mRenderOrder + 4, ParticleEffect::PARTICLE_DUST_SQUASH);
                }
            }
        }
        else if (mState == PlantState::STATE_SQUASH_DONE_FALLING)
        {
            if (mStateCountdown == 0)
            {
                Die();
            }
        }
    }
}

//0x460DD0
void Plant::UpdateDoomShroom()
{
    if (mIsAsleep || mState == PlantState::STATE_DOINGSPECIAL)
        return;

    mState = PlantState::STATE_DOINGSPECIAL;
    mDoSpecialCountdown = 100;

    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    TOD_ASSERT(aBodyReanim);

    aBodyReanim->SetFramesForLayer("anim_explode");
    aBodyReanim->mAnimRate = 23.0f;
    aBodyReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
    aBodyReanim->SetShakeOverride("DoomShroom_head1", 1.0f);
    aBodyReanim->SetShakeOverride("DoomShroom_head2", 2.0f);
    aBodyReanim->SetShakeOverride("DoomShroom_head3", 2.0f);
    mApp->PlayFoley(FoleyType::FOLEY_REVERSE_EXPLOSION);
}

void Plant::UpdateIceShroom()
{
    if (!mIsAsleep && mState != PlantState::STATE_DOINGSPECIAL)
    {
        mState = PlantState::STATE_DOINGSPECIAL;
        mDoSpecialCountdown = 100;
    }
}

//0x460F00
void Plant::UpdateBlover()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (aBodyReanim->mLoopCount > 0 && aBodyReanim->mLoopType != ReanimLoopType::REANIM_LOOP)
    {
        aBodyReanim->SetFramesForLayer("anim_loop");
        aBodyReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
    }

    if (mState != PlantState::STATE_DOINGSPECIAL && mStateCountdown == 0)
    {
        DoSpecial();
    }
}

void Plant::UpdateFlowerPot()
{
    if (mState == PlantState::STATE_FLOWERPOT_INVULNERABLE && mStateCountdown == 0)
        mState = PlantState::STATE_NOTREADY;
}

void Plant::UpdateLilypad()
{
    if (mState == PlantState::STATE_LILYPAD_INVULNERABLE && mStateCountdown == 0)
        mState = PlantState::STATE_NOTREADY;
}

//0x460F60
void Plant::UpdateCoffeeBean()
{
    if (mState == PlantState::STATE_DOINGSPECIAL)
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        if (aBodyReanim->mLoopCount > 0)
        {
            Die();
        }
    }
}

//0x460FA0
void Plant::UpdateUmbrella()
{
    if (mState == PlantState::STATE_UMBRELLA_TRIGGERED)
    {
        if (mStateCountdown == 0)
        {
            //mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PROJECTILE, mRow + 1, 0);
            mState = PlantState::STATE_UMBRELLA_REFLECTING;
        }
    }
    else if (mState == PlantState::STATE_UMBRELLA_REFLECTING)
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        if (aBodyReanim->mLoopCount > 0)
        {
            //PlayIdleAnim(0.0f);
            mState = PlantState::STATE_NOTREADY;
            //mRenderOrder = CalcRenderOrder();
        }
    }

    Reanimation* aSpecialReanim = mApp->ReanimationTryToGet(mSpecialReanimID);
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (aSpecialReanim) 
    {
        if (aBodyReanim)
        {
            aSpecialReanim->mColorOverride = aBodyReanim->mColorOverride;
            aSpecialReanim->mExtraAdditiveColor = aBodyReanim->mExtraAdditiveColor;
            aSpecialReanim->mExtraOverlayColor = aBodyReanim->mExtraOverlayColor;
            aSpecialReanim->mEnableExtraAdditiveDraw = aBodyReanim->mEnableExtraAdditiveDraw;
            aSpecialReanim->mEnableExtraOverlayDraw = aBodyReanim->mEnableExtraOverlayDraw;
        }

        if (aSpecialReanim->mLoopCount > 0)
        {
            if (aSpecialReanim->IsAnimPlaying("anim_block"))
            {
                aSpecialReanim->StartBlend(20);
                aSpecialReanim->SetFramesForLayer("anim_idle");
                aSpecialReanim->mLoopCount = 0;
                aSpecialReanim->mAnimTime = aBodyReanim->mAnimTime;
                aSpecialReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
                aSpecialReanim->mAnimRate = aBodyReanim->mAnimRate;
                aSpecialReanim->mRenderOrder = aBodyReanim->mRenderOrder;
            }
            else if (aSpecialReanim->IsAnimPlaying("anim_idle"))
            {
                mApp->RemoveReanimation(mSpecialReanimID);
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                if (aBodyReanim)
                {
                    for (int i = 0; i < aBodyReanim->mDefinition->mTracks.count; i++)
                    {
                        aBodyReanim->mTrackInstances[i].mRenderGroup = RENDER_GROUP_NORMAL;
                    }
                }
            }
        }
    }
}

//0x461020
void Plant::UpdateCobCannon()
{
    if (mState == PlantState::STATE_COBCANNON_ARMING)
    {
        if (mStateCountdown == 0)
        {
            mState = PlantState::STATE_COBCANNON_LOADING;
            PlayBodyReanim("anim_charge", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);
        }
    }
    else if (mState == PlantState::STATE_COBCANNON_LOADING)
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        if (aBodyReanim->ShouldTriggerTimedEvent(0.5f))
        {
            mApp->PlayFoley(FoleyType::FOLEY_SHOOP);
        }
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_COBCANNON_READY;
            PlayIdleAnim(12.0f);
        }
    }
    else if (mState == PlantState::STATE_COBCANNON_READY)
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        ReanimatorTrackInstance* aCobTrack = aBodyReanim->GetTrackInstanceByName("CobCannon_cob");
        aCobTrack->mTrackColor = GetFlashingColor(mBoard->mMainCounter, 75);
    }
    else if (mState == PlantState::STATE_COBCANNON_FIRING)
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        if (aBodyReanim->ShouldTriggerTimedEvent(0.48f))
        {
            mApp->PlayFoley(FoleyType::FOLEY_COB_LAUNCH);
        }
    }
}

//0x4611F0
void Plant::UpdateCactus()
{
    if (mShootingCounter > 0)
        return;

    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    if (mState == PlantState::STATE_CACTUS_RISING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_CACTUS_HIGH;
            PlayBodyReanim("anim_idlehigh", ReanimLoopType::REANIM_LOOP, 20, 0.0f);
            if (mApp->IsIZombieLevel())
            {
                aBodyReanim->mAnimRate = 0;
            }

            mLaunchCounter = 1;
        }
    }
    else if (mState == PlantState::STATE_CACTUS_HIGH)
    {
        if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY) == nullptr)
        {
            mState = PlantState::STATE_CACTUS_LOWERING;
            PlayBodyReanim("anim_lower", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, aBodyReanim->mDefinition->mFPS);
        }
    }
    else if (mState == PlantState::STATE_CACTUS_LOWERING)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_CACTUS_LOW;
            PlayIdleAnim(0.0f);
        }
    }
    else if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY))
    {
        mState = PlantState::STATE_CACTUS_RISING;
        PlayBodyReanim("anim_rise", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, aBodyReanim->mDefinition->mFPS);
        mApp->PlayFoley(FoleyType::FOLEY_PLANTGROW);
    }
}

//0x461320
void Plant::UpdateChomper()
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (mState == PlantState::STATE_READY)
    {
        if (FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY))
        {
            PlayBodyReanim("anim_bite", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 24.0f);
            mState = PlantState::STATE_CHOMPER_BITING;
            mStateCountdown = 70;
        }
    }
    else if (mState == PlantState::STATE_CHOMPER_BITING)
    {
        if (mStateCountdown == 0)
        {
            mApp->PlayFoley(FoleyType::FOLEY_BIGCHOMP);

            Zombie* aZombie = FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY);
            bool doBite = false;
            if (aZombie)
            {
                if (aZombie->mZombieType == ZombieType::ZOMBIE_GARGANTUAR || aZombie->mZombieType == ZombieType::ZOMBIE_REDEYE_GARGANTUAR || 
                    aZombie->mZombieType == ZombieType::ZOMBIE_BOSS || aZombie->mZombieType == ZombieType::ZOMBIE_ZAMBONI || 
                    aZombie->mZombieType == ZombieType::ZOMBIE_CATAPULT || 
                    aZombie->mZombiePhase >= PHASE_BOBSLED_SLIDING && aZombie->mZombiePhase <= PHASE_BOBSLED_CRASHING)
                {
                    doBite = true;
                }
            }
            bool doMiss = false;
            if (aZombie == nullptr)
            {
                doMiss = true;
            }
            else if (!aZombie->IsImmobilizied())
            {
                if (aZombie->IsBouncingPogo() ||
                    aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_IN_VAULT || aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_PRE_VAULT)
                {
                    doMiss = true;
                }
            }

            if (doBite)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
                aZombie->TakeDamage(40, 0U);
                mState = PlantState::STATE_CHOMPER_BITING_MISSED;
            }
            else if (doMiss)
            {
                mState = PlantState::STATE_CHOMPER_BITING_MISSED;
            }
            else
            {
                aZombie->DropHelm(0U);
                aZombie->DropShield(0U);
                aZombie->DropFlag();
                if (aZombie->mZombieType == ZOMBIE_POLEVAULTER)
                {
                    if (aZombie->mZombiePhase == PHASE_POLEVAULTER_PRE_VAULT || aZombie->mZombiePhase == PHASE_POLEVAULTER_IN_VAULT)
                        aZombie->DropZombiePole();
                    aZombie->DropPole();          
                }
                else if (aZombie->mZombieType == ZOMBIE_NEWSPAPER)
                {
                    aZombie->DropNewsPaperGlasses();
                }
                else if (aZombie->mZombieType == ZOMBIE_POGO)
                {
                    aZombie->DropPogoGlasses();
                }
                aZombie->DieWithLoot();
                mState = PlantState::STATE_CHOMPER_BITING_GOT_ONE;
            }
        }
    }
    else if (mState == PlantState::STATE_CHOMPER_BITING_GOT_ONE)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            PlayBodyReanim("anim_chew", ReanimLoopType::REANIM_LOOP, 0, 15.0f);
            if (mApp->IsIZombieLevel())
            {
                aBodyReanim->mAnimRate = 0;
            }

            mState = PlantState::STATE_CHOMPER_DIGESTING;
            mStateCountdown = 4000;
        }
    }
    else if (mState == PlantState::STATE_CHOMPER_DIGESTING)
    {
        if (mStateCountdown == 0)
        {
            PlayBodyReanim("anim_swallow", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);
            mState = PlantState::STATE_CHOMPER_SWALLOWING;
        }
    }
    else if ((mState == PlantState::STATE_CHOMPER_SWALLOWING || mState == PlantState::STATE_CHOMPER_BITING_MISSED) && aBodyReanim->mLoopCount > 0)
    {
        PlayIdleAnim(aBodyReanim->mDefinition->mFPS);
        mState = PlantState::STATE_READY;
    }
}

//0x4615D0
MagnetItem* Plant::GetFreeMagnetItem()
{
    if (mSeedType == SeedType::SEED_GOLD_MAGNET)
    {
        for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
        {
            if (mMagnetItems[i].mItemType == MagnetItemType::MAGNET_ITEM_NONE)
            {
                return &mMagnetItems[i];
            }
        }

        return nullptr;
    }

    return &mMagnetItems[0];
}

//0x460610
void Plant::MagnetShroomAttactItem(Zombie* theZombie)
{
    mState = PlantState::STATE_MAGNETSHROOM_SUCKING;
    mStateCountdown = 1500;
    PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);
    mApp->PlayFoley(FoleyType::FOLEY_MAGNETSHROOM);

    MagnetItem* aMagnetItem = GetFreeMagnetItem();
    if (theZombie->mHelmType == HelmType::HELMTYPE_BUCKET)
    {
        int aDamageIndex = theZombie->GetHelmDamageIndex();

        theZombie->mHelmHealth = 0;
        theZombie->mHelmType = HelmType::HELMTYPE_NONE;
        theZombie->GetTrackPosition("anim_bucket", aMagnetItem->mPosX, aMagnetItem->mPosY);
        theZombie->ReanimShowPrefix("anim_bucket", RENDER_GROUP_HIDDEN);
        theZombie->ReanimShowPrefix("anim_hair", RENDER_GROUP_NORMAL);

        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_BUCKET1->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_BUCKET1->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 25.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f) + 20.0f;
        aMagnetItem->mItemType = (MagnetItemType)((int)MagnetItemType::MAGNET_ITEM_BUCKET_1 + aDamageIndex);
    }
    else if (theZombie->mHelmType == HelmType::HELMTYPE_FOOTBALL)
    {
        int aDamageIndex = theZombie->GetHelmDamageIndex();

        theZombie->mHelmHealth = 0;
        theZombie->mHelmType = HelmType::HELMTYPE_NONE;
        theZombie->GetTrackPosition("zombie_football_helmet", aMagnetItem->mPosX, aMagnetItem->mPosY);
        theZombie->ReanimShowPrefix("zombie_football_helmet", RENDER_GROUP_HIDDEN);
        theZombie->ReanimShowPrefix("anim_hair", RENDER_GROUP_NORMAL);

        aMagnetItem->mPosX += 37.0f;
        aMagnetItem->mPosY -= 60.0f;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 20.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f) + 20.0f;
        aMagnetItem->mItemType = (MagnetItemType)((int)MagnetItemType::MAGNET_ITEM_FOOTBALL_HELMET_1 + aDamageIndex);
    }
    else if (theZombie->mShieldType == ShieldType::SHIELDTYPE_DOOR)
    {
        int aDamageIndex = theZombie->GetShieldDamageIndex();

        theZombie->DetachShield();
        theZombie->mZombiePhase = ZombiePhase::PHASE_ZOMBIE_NORMAL;
        if (!theZombie->mIsEating)
        {
            TOD_ASSERT(theZombie->mZombieHeight == ZombieHeight::HEIGHT_ZOMBIE_NORMAL);
            theZombie->StartWalkAnim(0);
        }
        theZombie->GetTrackPosition("anim_screendoor", aMagnetItem->mPosX, aMagnetItem->mPosY);

        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_SCREENDOOR1->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_SCREENDOOR1->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 30.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f);
        aMagnetItem->mItemType = (MagnetItemType)((int)MagnetItemType::MAGNET_ITEM_DOOR_1 + aDamageIndex);
    }
    else if (theZombie->mShieldType == ShieldType::SHIELDTYPE_TRASHCAN)
    {
        int aDamageIndex = theZombie->GetShieldDamageIndex();

        theZombie->DetachShield();
        theZombie->mZombiePhase = ZombiePhase::PHASE_ZOMBIE_NORMAL;
        if (!theZombie->mIsEating)
        {
            TOD_ASSERT(theZombie->mZombieHeight == ZombieHeight::HEIGHT_ZOMBIE_NORMAL);
            theZombie->StartWalkAnim(0);
        }
        theZombie->GetTrackPosition("anim_screendoor", aMagnetItem->mPosX, aMagnetItem->mPosY);

        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_TRASHCAN1->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_TRASHCAN1->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 30.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f);
        aMagnetItem->mItemType = (MagnetItemType)((int)MagnetItemType::MAGNET_ITEM_TRASHCAN_1 + aDamageIndex);
    }
    else if (theZombie->mShieldType == ShieldType::SHIELDTYPE_LADDER)
    {
        int aDamageIndex = theZombie->GetShieldDamageIndex();

        theZombie->DetachShield();

        aMagnetItem->mPosX = theZombie->mPosX + 31.0f;
        aMagnetItem->mPosY = theZombie->mPosY + 20.0f;
        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_LADDER_5->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_LADDER_5->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 30.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f);
        aMagnetItem->mItemType = (MagnetItemType)((int)MagnetItemType::MAGNET_ITEM_LADDER_1 + aDamageIndex);
    }
    else if (theZombie->mZombieType == ZombieType::ZOMBIE_POGO)
    {
        theZombie->PogoBreak(16U);
        // ZombieDrawPosition aDrawPos;
        // theZombie->GetDrawPos(aDrawPos);
        theZombie->GetTrackPosition("Zombie_pogo_stick", aMagnetItem->mPosX, aMagnetItem->mPosY);

        aMagnetItem->mPosX += 40.0f - IMAGE_REANIM_ZOMBIE_LADDER_5->GetWidth() / 2;
        aMagnetItem->mPosY += 84.0f - IMAGE_REANIM_ZOMBIE_LADDER_5->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 30.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f);
        aMagnetItem->mItemType = theZombie->mHasArm ? MagnetItemType::MAGNET_ITEM_POGO_1 : MagnetItemType::MAGNET_ITEM_POGO_3;
    }
    else if (theZombie->mZombiePhase == ZombiePhase::PHASE_JACK_IN_THE_BOX_RUNNING)
    {
        theZombie->StopZombieSound();
        theZombie->PickRandomSpeed();
        theZombie->mZombiePhase = ZombiePhase::PHASE_ZOMBIE_NORMAL;
        theZombie->ReanimShowPrefix("Zombie_jackbox_box", RENDER_GROUP_HIDDEN);
        theZombie->ReanimShowPrefix("Zombie_jackbox_handle", RENDER_GROUP_HIDDEN);
        theZombie->GetTrackPosition("Zombie_jackbox_box", aMagnetItem->mPosX, aMagnetItem->mPosY);

        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_JACKBOX_BOX->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_JACKBOX_BOX->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 20.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f) + 15.0f;
        aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_JACK_IN_THE_BOX;
    }
    else if (theZombie->mZombieType == ZombieType::ZOMBIE_DIGGER)
    {
        theZombie->DiggerLoseAxe();
        theZombie->GetTrackPosition("Zombie_digger_pickaxe", aMagnetItem->mPosX, aMagnetItem->mPosY);

        aMagnetItem->mPosX -= IMAGE_REANIM_ZOMBIE_DIGGER_PICKAXE->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_REANIM_ZOMBIE_DIGGER_PICKAXE->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 45.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f) + 15.0f;
        aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_PICK_AXE;
    }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    else if (theZombie->mZombiePhase == ZombiePhase::PHASE_PROPELLER_FLYING)
    {
        theZombie->LandFlyer(0U);
        aMagnetItem->mPosX = theZombie->mPosX + 80;
        aMagnetItem->mPosY = theZombie->mPosY - theZombie->mAltitude;

        aMagnetItem->mPosX -= IMAGE_PROPELLER->GetWidth() / 2;
        aMagnetItem->mPosY -= IMAGE_PROPELLER->GetHeight() / 2;
        aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 5.0f;
        aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f) + 15.0f;
        aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_PROPELLER;
    }
#endif
}

//0x461CD0
bool Plant::DrawMagnetItemsOnTop()
{
    switch (mSeedType)
    {
        case SeedType::SEED_GOLD_MAGNET:
        {
            for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
            {
                if (mMagnetItems[i].mItemType != MagnetItemType::MAGNET_ITEM_NONE)
                {
                    return true;
                }
            }
            break;
        }
        case SeedType::SEED_MAGNETSHROOM:
        {
            for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
            {
                MagnetItem* aMagnetItem = &mMagnetItems[i];
                if (aMagnetItem->mItemType != MagnetItemType::MAGNET_ITEM_NONE)
                {
                    SexyVector2 aVectorToPlant(mX + aMagnetItem->mDestOffsetX - aMagnetItem->mPosX, mY + aMagnetItem->mDestOffsetY - aMagnetItem->mPosY);
                    if (aVectorToPlant.Magnitude() > 20.0f)
                    {
                        return true;
                    }
                }
            }
            break;
        }
    }

    return false;
}

//0x461D90
void Plant::UpdateMagnetShroom()
{
    for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
    {
        MagnetItem* aMagnetItem = &mMagnetItems[i];
        if (aMagnetItem->mItemType != MagnetItemType::MAGNET_ITEM_NONE)
        {
            SexyVector2 aVectorToPlant(mX + aMagnetItem->mDestOffsetX - aMagnetItem->mPosX, mY + aMagnetItem->mDestOffsetY - aMagnetItem->mPosY);
            if (aVectorToPlant.Magnitude() > 20.0f)
            {
                aMagnetItem->mPosX += aVectorToPlant.x * 0.05f;
                aMagnetItem->mPosY += aVectorToPlant.y * 0.05f;
            }
        }
    }

    if (mState == PlantState::STATE_MAGNETSHROOM_CHARGING)
    {
        if (mStateCountdown == 0)
        {
            mState = PlantState::STATE_READY;

            float aAnimRate = RandRangeFloat(10.0f, 15.0f);
            PlayBodyReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 30, aAnimRate);
            if (mApp->IsIZombieLevel())
            {
                Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                aBodyReanim->mAnimRate = 0.0f;
            }

            mMagnetItems[0].mItemType = MagnetItemType::MAGNET_ITEM_NONE;
        }
    }
    else if (mState == PlantState::STATE_MAGNETSHROOM_SUCKING)
    {
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aBodyReanim->mLoopCount > 0)
        {
            PlayBodyReanim("anim_nonactive_idle2", ReanimLoopType::REANIM_LOOP, 20, 2.0f);
            if (mApp->IsIZombieLevel())
            {
                aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
                aBodyReanim->mAnimRate = 0.0f;
            }

            mState = PlantState::STATE_MAGNETSHROOM_CHARGING;
        }
    }
    else
    {
        float aClosestDistance = 0.0f;
        Zombie* aClosestZombie = nullptr;

        Zombie* aZombie = nullptr;
        while (mBoard->IterateZombies(aZombie))
        {
            int aDiffY = aZombie->mRow - mRow;
            Rect aZombieRect = aZombie->GetZombieRect();

            if (aZombie->mMindControlled)
                continue;

            if (!aZombie->mHasHead)
                continue;

            if (aZombie->mZombieHeight != ZombieHeight::HEIGHT_ZOMBIE_NORMAL || aZombie->mZombiePhase == ZombiePhase::PHASE_RISING_FROM_GRAVE)
                continue;

            if (aZombie->IsDeadOrDying())
                continue;

            if (aZombieRect.mX > BOARD_WIDTH || aDiffY > 2 || aDiffY < -2)
                continue;

            if (aZombie->mZombiePhase == ZombiePhase::PHASE_DIGGER_TUNNELING ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_DIGGER_STUNNED ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_DIGGER_WALKING ||
                aZombie->mZombieType == ZombieType::ZOMBIE_POGO)
            {
                if (!aZombie->mHasObject)
                    continue;
            }
            else if (!(aZombie->mHelmType == HelmType::HELMTYPE_BUCKET ||
                aZombie->mHelmType == HelmType::HELMTYPE_FOOTBALL ||
                aZombie->mShieldType == ShieldType::SHIELDTYPE_DOOR ||
                aZombie->mShieldType == ShieldType::SHIELDTYPE_TRASHCAN ||
                aZombie->mShieldType == ShieldType::SHIELDTYPE_LADDER ||
                aZombie->mZombiePhase == ZombiePhase::PHASE_JACK_IN_THE_BOX_RUNNING 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
                || aZombie->mZombiePhase == ZombiePhase::PHASE_PROPELLER_FLYING
#endif
                ))
                continue;

            int aRadius = aZombie->mIsEating ? 320 : 270;
            if (GetOvalRectOverlap(mX + 40, mY + 40, aRadius - 40, aRadius, aZombieRect))
            {
                float aDistance = Distance2D(mX, mY, aZombieRect.mX, aZombieRect.mY);
                aDistance += abs(aDiffY) * 80.0f;

                if (aClosestZombie == nullptr || aDistance < aClosestDistance)
                {
                    aClosestZombie = aZombie;
                    aClosestDistance = aDistance;
                }
            }
        }

        if (aClosestZombie)
        {
            MagnetShroomAttactItem(aClosestZombie);
            return;
        }

        ////////////////////

        float aClosestLadderDist = 0.0f;
        GridItem* aClosestLadder = nullptr;
        
        GridItem* aGridItem = nullptr;
        while (mBoard->IterateGridItems(aGridItem))
        {
            if (aGridItem->mGridItemType == GridItemType::GRIDITEM_LADDER)
            {
                int aDiffX = abs(aGridItem->mGridX - mPlantCol);
                int aDiffY = abs(aGridItem->mGridY - mRow);
                int aSquareDistance = max(aDiffX, aDiffY);
                if (aSquareDistance <= 2)
                {
                    float aDistance = aSquareDistance + aDiffY * 0.05f;
                    if (aClosestLadder == nullptr || aDistance < aClosestLadderDist)
                    {
                        aClosestLadder = aGridItem;
                        aClosestLadderDist = aDistance;
                    }
                }
            }
        }

        if (aClosestLadder)
        {
            mState = PlantState::STATE_MAGNETSHROOM_SUCKING;
            mStateCountdown = 1500;
            PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);
            mApp->PlayFoley(FoleyType::FOLEY_MAGNETSHROOM);

            aClosestLadder->GridItemDie();

            MagnetItem* aMagnetItem = GetFreeMagnetItem();
            aMagnetItem->mPosX = mBoard->GridToPixelX(aClosestLadder->mGridX, aClosestLadder->mGridY) + 40;
            aMagnetItem->mPosY = mBoard->GridToPixelY(aClosestLadder->mGridX, aClosestLadder->mGridY);
            aMagnetItem->mDestOffsetX = RandRangeFloat(-10.0f, 10.0f) + 10.0f;
            aMagnetItem->mDestOffsetY = RandRangeFloat(-10.0f, 10.0f);
            aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_LADDER_PLACED;
        }
    }
}

//0x462390
Coin* Plant::FindGoldMagnetTarget()
{
    Coin* aClosestCoin = nullptr;
    float aClosestDistance = 0.0f;

    Coin* aCoin = nullptr;
    while (mBoard->IterateCoins(aCoin))
    {
        if (aCoin->IsMoney() && aCoin->mCoinMotion != CoinMotion::COIN_MOTION_FROM_PRESENT && !aCoin->mIsBeingCollected && aCoin->mCoinAge >= 50)
        {
            float aDistance = Distance2D(mX + mWidth / 2, mY + mHeight / 2, aCoin->mPosX + aCoin->mWidth / 2, aCoin->mPosY + aCoin->mHeight / 2);
            if (aClosestCoin == nullptr || aDistance < aClosestDistance)
            {
                aClosestCoin = aCoin;
                aClosestDistance = aDistance;
            }
        }
    }

    return aClosestCoin;
}

//0x4624B0
void Plant::GoldMagnetFindTargets()
{
    if (GetFreeMagnetItem() == nullptr)
    {
        TOD_ASSERT();
        return;
    }

    for (;;)
    {
        MagnetItem* aMagnetItem = GetFreeMagnetItem();
        if (aMagnetItem == nullptr)
            break;

        Coin* aCoin = FindGoldMagnetTarget();
        if (aCoin == nullptr)
            break;

        aMagnetItem->mPosX = aCoin->mPosX + 15.0f;
        aMagnetItem->mPosY = aCoin->mPosY + 15.0f;
        aMagnetItem->mDestOffsetX = RandRangeFloat(20.0f, 40.0f);
        aMagnetItem->mDestOffsetY = RandRangeFloat(-20.0f, 0.0f) + 20.0f;

        switch (aCoin->mType)
        {
        case CoinType::COIN_SILVER:     aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_SILVER_COIN;   break;
        case CoinType::COIN_GOLD:       aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_GOLD_COIN;     break;
        case CoinType::COIN_DIAMOND:    aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_DIAMOND;       break;
        default:                        TOD_ASSERT();                                                       return;
        }
        
        aCoin->Die();
    }
}

//0x4625D0
bool Plant::IsAGoldMagnetAboutToSuck()
{
    Plant* aPlant = nullptr;
    while (mBoard->IteratePlants(aPlant))
    {
        if (!aPlant->NotOnGround() && aPlant->mSeedType == SeedType::SEED_GOLD_MAGNET && aPlant->mState == PlantState::STATE_MAGNETSHROOM_SUCKING)
        {
            Reanimation* aBodyReanim = mApp->ReanimationGet(aPlant->mBodyReanimID);
            if (aBodyReanim->mAnimTime < 0.5f)
            {
                return true;
            }
        }
    }

    return false;
}

//0x462680
void Plant::UpdateGoldMagnetShroom()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);

    bool aIsSuckingCoin = false;
    for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
    {
        MagnetItem* aMagnetItem = &mMagnetItems[i];
        if (aMagnetItem->mItemType != MagnetItemType::MAGNET_ITEM_NONE)
        {
            SexyVector2 aVectorToPlant(mX + aMagnetItem->mDestOffsetX - aMagnetItem->mPosX, mY + aMagnetItem->mDestOffsetY - aMagnetItem->mPosY);
            float aDistance = aVectorToPlant.Magnitude();
            if (aDistance < 20.0f)
            {
                CoinType aCoinType;
                switch (aMagnetItem->mItemType)
                {
                case MagnetItemType::MAGNET_ITEM_SILVER_COIN:   aCoinType = CoinType::COIN_SILVER;      break;
                case MagnetItemType::MAGNET_ITEM_GOLD_COIN:     aCoinType = CoinType::COIN_GOLD;        break;
                case MagnetItemType::MAGNET_ITEM_DIAMOND:       aCoinType = CoinType::COIN_DIAMOND;     break;
                default:                                        TOD_ASSERT();                           return;
                }

                int aValue = Coin::GetCoinValue(aCoinType);
                mApp->mPlayerInfo->AddCoins(aValue);
                mBoard->mCoinsCollected += aValue;

                if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_DIAMOND)
                    mApp->PlaySample(SOUND_DIAMOND);
                else
                    mApp->PlayFoley(FoleyType::FOLEY_COIN);

                aMagnetItem->mItemType = MagnetItemType::MAGNET_ITEM_NONE;
            }
            else
            {
                float aSpeed = TodAnimateCurveFloatTime(30.0f, 0.0f, aDistance, 0.02f, 0.05f, TodCurves::CURVE_LINEAR);
                aMagnetItem->mPosX += aVectorToPlant.x * aSpeed;
                aMagnetItem->mPosY += aVectorToPlant.y * aSpeed;

                aIsSuckingCoin = true;
            }
        }
    }

    if (mState == PlantState::STATE_MAGNETSHROOM_CHARGING)
    {
        if (mStateCountdown == 0)
        {
            mState = PlantState::STATE_READY;
        }
    }
    else if (mState == PlantState::STATE_MAGNETSHROOM_SUCKING)
    {
        if (aBodyReanim->ShouldTriggerTimedEvent(0.4f))
        {
            mApp->PlayFoley(FoleyType::FOLEY_MAGNETSHROOM);
            GoldMagnetFindTargets();
        }

        if (aBodyReanim->mLoopCount > 0 && !aIsSuckingCoin)
        {
            PlayIdleAnim(14.0f);
            mState = PlantState::STATE_MAGNETSHROOM_CHARGING;
            mStateCountdown = RandRangeInt(200, 300);
        }
    }
    else if (!IsAGoldMagnetAboutToSuck() && Sexy::Rand(50) == 0 && FindGoldMagnetTarget())
    {
        mBoard->ShowCoinBank();
        mState = PlantState::STATE_MAGNETSHROOM_SUCKING;
        PlayBodyReanim("anim_attract", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);
    }
}

//0x4629F0
void Plant::RemoveEffects()
{
    mApp->RemoveParticle(mParticleID);
    mApp->RemoveReanimation(mBodyReanimID);
    mApp->RemoveReanimation(mHeadReanimID);
    mApp->RemoveReanimation(mHeadReanimID2);
    mApp->RemoveReanimation(mHeadReanimID3);
    mApp->RemoveReanimation(mLightReanimID);
    mApp->RemoveReanimation(mBlinkReanimID);
    mApp->RemoveReanimation(mSleepingReanimID);
    mApp->RemoveReanimation(mSpecialReanimID);
}

//0x462B80
void Plant::Squish()
{
    if (NotOnGround())
        return;

    if (mSeedType == SeedType::SEED_IMITATER)
    {
        TodParticleSystem* aParticle = mApp->AddTodParticle(mX + 40, mY + 40, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_IMITATER_MORPH);
        mSeedType = mImitaterType;
        mImitaterType = SeedType::SEED_IMITATER;
        mRenderOrder = CalcRenderOrder();
    }

    if (!mIsAsleep)
    {
        if (mSeedType == SeedType::SEED_CHERRYBOMB || mSeedType == SeedType::SEED_JALAPENO ||
            mSeedType == SeedType::SEED_DOOMSHROOM || mSeedType == SeedType::SEED_ICESHROOM || 
            mSeedType == SeedType::SEED_EXPLODE_O_NUT 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
            || mSeedType == SeedType::SEED_TIMESTOPPER || mSeedType == SeedType::SEED_CHERRYHOVERBOMB ||
            mSeedType == SeedType::SEED_NUKECUMBER 
#endif
            )
        {
            DoSpecial();
            return;
        }
        else if (mSeedType == SeedType::SEED_POTATOMINE && mState != PlantState::STATE_NOTREADY)
        {
            DoSpecial();
            return;
        }
    }

    if (mSeedType == SeedType::SEED_SQUASH && mState != PlantState::STATE_NOTREADY)
        return;

    int offsetLayer = 0;
    if (mSeedType == SEED_PUMPKINSHELL) offsetLayer += 1;
    else if (mSeedType == SEED_FLOWERPOT) offsetLayer -= 1;
    mRenderOrder = Board::MakeRenderOrder(RenderLayer(RenderLayer::RENDER_LAYER_GRAVE_STONE + offsetLayer), mRow, 8);
    mSquished = true;
    mDisappearCountdown = 500;
    mApp->PlayFoley(FoleyType::FOLEY_SQUISH);
    RemoveEffects();

    GridItem* aLadder = mBoard->GetLadderAt(mPlantCol, mRow);
    if (aLadder)
    {
        aLadder->GridItemDie();
    }

    if (mApp->IsIZombieLevel())
    {
        mBoard->mChallenge->IZombiePlantDropRemainingSun(this);
    }
}

//0x462CE0
void Plant::UpdateBowling()
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim && aBodyReanim->TrackExists("_ground"))
    {
        float aSpeed = aBodyReanim->GetTrackVelocity("_ground");
        if (mSeedType == SeedType::SEED_GIANT_WALLNUT)
        {
            aSpeed *= 2;
        }

        mX -= aSpeed;
        if (mX > 800 + mWidth * (aBodyReanim->mOverlayMatrix.m00 - 1))
            Die();
    }

    if (mState == PlantState::STATE_BOWLING_UP)
    {
        mY -= 2;
    }
    else if (mState == PlantState::STATE_BOWLING_DOWN)
    {
        mY += 2;
    }
    int aDistToGrid = mBoard->GridToPixelY(0, mRow) - mY;
    if (aDistToGrid < -2 || aDistToGrid > 2)
        return;

    PlantState aNewState = mState;
    if (mState == PlantState::STATE_BOWLING_UP && mRow <= 0)
    {
        aNewState = PlantState::STATE_BOWLING_DOWN;
    }
    else if (mState == PlantState::STATE_BOWLING_DOWN && mRow >= 4)
    {
        aNewState = PlantState::STATE_BOWLING_UP;
    }

    Zombie* aZombie = FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY);
    if (aZombie)
    {
        int aPosX = mX + mWidth / 2;
        int aPosY = mY + mHeight / 2;

        if (mSeedType == SeedType::SEED_EXPLODE_O_NUT)
        {
            /*mApp->PlayFoley(FoleyType::FOLEY_CHERRYBOMB);
            mApp->PlaySample(SOUND_BOWLINGIMPACT2);

            int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY) | 32U;
            mBoard->KillAllZombiesInRadius(mRow, aPosX, aPosY, 90, 1, true, aDamageRangeFlags);
            TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_POWIE);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) 
            {
                aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
            }
            mBoard->ShakeBoard(3, -4);

            Die();*/
            mApp->PlaySample(SOUND_BOWLINGIMPACT2);
            DoSpecial();

            return;
        }

        mApp->PlayFoley(FoleyType::FOLEY_BOWLINGIMPACT);
        mBoard->ShakeBoard(1, -2);

        if (mSeedType == SeedType::SEED_GIANT_WALLNUT)
        {
            aZombie->TakeDamage(1800, aZombie->mAltitude < 0 ? 18U : 0U);
        }
        else if (aZombie->mShieldType == ShieldType::SHIELDTYPE_DOOR && mState != PlantState::STATE_NOTREADY)
        {
            aZombie->TakeDamage(1800, 0U);
        }
        else if (aZombie->mShieldType != ShieldType::SHIELDTYPE_NONE)
        {
            aZombie->TakeShieldDamage(400, 0U);
        }
        else if (aZombie->mHelmType != HelmType::HELMTYPE_NONE)
        {
            if (aZombie->mHelmType == HelmType::HELMTYPE_BUCKET)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SHIELD_HIT);
            }
            else if (aZombie->mHelmType == HelmType::HELMTYPE_TRAFFIC_CONE)
            {
                mApp->PlayFoley(FoleyType::FOLEY_PLASTIC_HIT);
            }
            
            aZombie->TakeHelmDamage(900, 0U);
        }
        else
        {
            aZombie->TakeDamage(1800, 0U);
        }

        if ((!mApp->IsFirstTimeAdventureMode() || mApp->mPlayerInfo->GetLevel() > 10) && mSeedType == SeedType::SEED_WALLNUT)
        {
            mLaunchCounter++;
            if (mLaunchCounter == 2)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SPAWN_SUN);
                mBoard->AddCoin(aPosX, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
            }
            else if (mLaunchCounter == 3)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SPAWN_SUN);
                mBoard->AddCoin(aPosX - 5.0f, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
                mBoard->AddCoin(aPosX + 5.0f, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
            }
            else if (mLaunchCounter == 4)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SPAWN_SUN);
                mBoard->AddCoin(aPosX - 10.0f, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
                mBoard->AddCoin(aPosX, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
                mBoard->AddCoin(aPosX + 10.0f, aPosY, CoinType::COIN_SILVER, CoinMotion::COIN_MOTION_COIN);
            }
            else if (mLaunchCounter >= 5)
            {
                mApp->PlayFoley(FoleyType::FOLEY_SPAWN_SUN);
                mBoard->AddCoin(aPosX, aPosY, CoinType::COIN_GOLD, CoinMotion::COIN_MOTION_COIN);
                if (!mApp->IsIZombieLevel())
                    ReportAchievement::GiveAchievement(mApp, AchievementId::RollSomeHeads, true); // @Patoke: add achievement
            }
        }

        if (mSeedType != SeedType::SEED_GIANT_WALLNUT)
        {
            if (mRow == 4 || mState == PlantState::STATE_BOWLING_DOWN)
            {
                aNewState = PlantState::STATE_BOWLING_UP;
            }
            else if (mRow == 0 || mState == PlantState::STATE_BOWLING_UP)
            {
                aNewState = PlantState::STATE_BOWLING_DOWN;
            }
            else
            {
                aNewState = Sexy::Rand(2) ? PlantState::STATE_BOWLING_UP : PlantState::STATE_BOWLING_DOWN;
            }
        }
    }

    if (aNewState == PlantState::STATE_BOWLING_UP)
    {
        mRow--;
        mState = PlantState::STATE_BOWLING_UP;
        mRenderOrder = CalcRenderOrder();
    }
    else if (aNewState == PlantState::STATE_BOWLING_DOWN)
    {
        mState = PlantState::STATE_BOWLING_DOWN;
        mRenderOrder = CalcRenderOrder();
        mRow++;
    }
}

//0x463150
void Plant::UpdateAbilities()
{
    if (!IsInPlay() || mBurnedCounter != -1)
        return;

    if (mState == PlantState::STATE_DOINGSPECIAL || mSquished)
    {
        mDisappearCountdown--;
        if (mDisappearCountdown < 0)
        {
            Die();
            return;
        }
    }

    if (mWakeUpCounter > 0)
    {
        mWakeUpCounter--;
        if (mWakeUpCounter == 60)
        {
            mApp->PlayFoley(FoleyType::FOLEY_WAKEUP);
        }
        if (mWakeUpCounter == 0)
        {
            SetSleeping(false);
        }
    }

    if (mIsAsleep || mSquished || mOnBungeeState != PlantOnBungeeState::NOT_ON_BUNGEE)
        return;

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    if (mSeedType == SeedType::SEED_DUPLICATORCYCLER && IsInPlay() && mBoard->mCursorObject->mDuplicatorPlantID == (PlantID)mBoard->mPlants.DataArrayGetID(this))
        return;
#endif
    
    UpdateShooting();

    if (mStateCountdown > 0)
        mStateCountdown--;

    if (mApp->IsWallnutBowlingLevel())
    {
        UpdateBowling();
        return;
    }

    if (mSeedType == SeedType::SEED_SQUASH)                                                     UpdateSquash();
    else if (mSeedType == SeedType::SEED_DOOMSHROOM)                                            UpdateDoomShroom();
    else if (mSeedType == SeedType::SEED_ICESHROOM)                                             UpdateIceShroom();
    else if (mSeedType == SeedType::SEED_CHOMPER)                                               UpdateChomper();
    else if (mSeedType == SeedType::SEED_BLOVER)                                                UpdateBlover();
    else if (mSeedType == SeedType::SEED_FLOWERPOT)                                             UpdateFlowerPot();
    else if (mSeedType == SeedType::SEED_LILYPAD)                                               UpdateLilypad();
    else if (mSeedType == SeedType::SEED_IMITATER)                                              UpdateImitater();
    else if (mSeedType == SeedType::SEED_INSTANT_COFFEE)                                        UpdateCoffeeBean();
    else if (mSeedType == SeedType::SEED_UMBRELLA)                                              UpdateUmbrella();
    else if (mSeedType == SeedType::SEED_COBCANNON)                                             UpdateCobCannon();
    else if (mSeedType == SeedType::SEED_CACTUS)                                                UpdateCactus();
    else if (mSeedType == SeedType::SEED_MAGNETSHROOM)                                          UpdateMagnetShroom();
    else if (mSeedType == SeedType::SEED_GOLD_MAGNET)                                           UpdateGoldMagnetShroom();
    else if (mSeedType == SeedType::SEED_SUNSHROOM)                                             UpdateSunShroom();
    else if (MakesSun() || mSeedType == SeedType::SEED_MARIGOLD)                                UpdateProductionPlant();
    else if (mSeedType == SeedType::SEED_GRAVEBUSTER)                                           UpdateGraveBuster();
    else if (mSeedType == SeedType::SEED_TORCHWOOD)                                             UpdateTorchwood();
    else if (mSeedType == SeedType::SEED_POTATOMINE)                                            UpdatePotato();
    else if (mSeedType == SeedType::SEED_SPIKEWEED || mSeedType == SeedType::SEED_SPIKEROCK)    UpdateSpikeweed();
    else if (mSeedType == SeedType::SEED_TANGLEKELP)                                            UpdateTanglekelp();
    else if (mSeedType == SeedType::SEED_SCAREDYSHROOM)                                         UpdateScaredyShroom();
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    else if (mSeedType == SeedType::SEED_YAMPOLINE)                                             UpdateYampoline();
#endif

    if (mSubclass == PlantSubClass::SUBCLASS_SHOOTER)
    {
#ifdef _DS_MINIGAMES
        if (mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
        {
            if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED_2)
                return;

            if ((mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED) && 
                mBoard->mMainCounter % 2 == 1 && mBoard->mMainCounter != 0)
                return;
        }
#endif

        UpdateShooter();
    }
    if (mDoSpecialCountdown > 0)
    {
        mDoSpecialCountdown--;
        if (mDoSpecialCountdown == 0)
        {
            DoSpecial();
        }
    }
}

//0x463420
bool Plant::IsPartOfUpgradableTo(SeedType theUpgradedType)
{
    if (theUpgradedType == SeedType::SEED_COBCANNON && mSeedType == SeedType::SEED_KERNELPULT)
    {
        return mBoard->IsValidCobCannonSpot(mPlantCol, mRow) || mBoard->IsValidCobCannonSpot(mPlantCol - 1, mRow);
    }

    return IsUpgradableTo(theUpgradedType);
}

//0x463470
bool Plant::IsUpgradableTo(SeedType theUpgradedType)
{
    if (theUpgradedType == SeedType::SEED_GATLINGPEA && mSeedType == SeedType::SEED_REPEATER)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_WINTERMELON && mSeedType == SeedType::SEED_MELONPULT)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_TWINSUNFLOWER && mSeedType == SeedType::SEED_SUNFLOWER)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_SPIKEROCK && mSeedType == SeedType::SEED_SPIKEWEED)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_COBCANNON && mSeedType == SeedType::SEED_KERNELPULT)
    {
        return mBoard->IsValidCobCannonSpot(mPlantCol, mRow);
    }
    if (theUpgradedType == SeedType::SEED_GOLD_MAGNET && mSeedType == SeedType::SEED_MAGNETSHROOM)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_GLOOMSHROOM && mSeedType == SeedType::SEED_FUMESHROOM)
    {
        return true;
    }
    if (theUpgradedType == SeedType::SEED_CATTAIL && mSeedType == SeedType::SEED_LILYPAD)
    {
        Plant* aPlant = mBoard->GetTopPlantAt(mPlantCol, mRow, PlantPriority::TOPPLANT_ONLY_NORMAL_POSITION);
        return aPlant == nullptr || aPlant->mSeedType != SeedType::SEED_CATTAIL;
    }
    return false;
}

//0x4635C0
void Plant::UpdateReanimColor()
{
    if (!IsOnBoard())
        return;

    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim == nullptr)
        return;

    SeedType aSeedType = mBoard->GetSeedTypeInCursor();
    Color aColorOverride;

    bool isOnGlove = false;
    if (mBoard->mCursorObject->mCursorType == CursorType::CURSOR_TYPE_PLANT_FROM_GLOVE)
    {
        Plant* aPlant = mBoard->mPlants.DataArrayTryToGet((unsigned int)mBoard->mCursorObject->mGlovePlantID);
        if (aPlant)
        {
            if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN && aPlant->mPlantCol == mPlantCol && aPlant->mRow == mRow
#ifdef _DS_MINIGAMES
                || mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE && aPlant == this
#endif
                )
                isOnGlove = true;
        }
    }

    if (mBurnedCounter != -1)
    {
        aColorOverride = Color::Black;
    }
    else if (isOnGlove)
    {
        aColorOverride = Color(128, 128, 128);
    }
    else if (IsPartOfUpgradableTo(aSeedType) && mBoard->CanPlantAt(mPlantCol, mRow, aSeedType) == PLANTING_OK)
    {
        aColorOverride = GetFlashingColor(mBoard->mMainCounter, 90);
    }
    else if (aSeedType == SeedType::SEED_COBCANNON && mSeedType == SeedType::SEED_KERNELPULT && mBoard->CanPlantAt(mPlantCol - 1, mRow, aSeedType) == PLANTING_OK)
    {
        aColorOverride = GetFlashingColor(mBoard->mMainCounter, 90);
    }
    else if (mSeedType == SeedType::SEED_EXPLODE_O_NUT)
    {
        aColorOverride = Color(255, 64, 64);
    }
    else
    {
        aColorOverride = Color(255, 255, 255);
    }

    aBodyReanim->mColorOverride = aColorOverride;

    if (mBurnedCounter != -1)
    {
        aBodyReanim->mEnableExtraAdditiveDraw = false;
    }
    else if (mHighlighted)
    {
        aBodyReanim->mExtraAdditiveColor = Color(255, 255, 255, 196);
        aBodyReanim->mEnableExtraAdditiveDraw = true;
        if (mImitaterType == SeedType::SEED_IMITATER)
        {
            aBodyReanim->mExtraAdditiveColor = Color(255, 255, 255, 92);
        }
    }
    else if (mBeghouledFlashCountdown > 0)
    {
        int anAlpha = TodAnimateCurve(50, 0, mBeghouledFlashCountdown % 50, 0, 128, TodCurves::CURVE_BOUNCE);
        aBodyReanim->mExtraAdditiveColor = Color(255, 255, 255, anAlpha);
        aBodyReanim->mEnableExtraAdditiveDraw = true;
    }
    else if (mEatenFlashCountdown > 0)
    {
        int aGrayness = ClampInt(mEatenFlashCountdown * 3, 0, mImitaterType == SeedType::SEED_IMITATER ? 128 : 255);
        aBodyReanim->mExtraAdditiveColor = Color(aGrayness, aGrayness, aGrayness);
        aBodyReanim->mEnableExtraAdditiveDraw = true;
    }
    else
    {
        aBodyReanim->mEnableExtraAdditiveDraw = false;
    }

    if (mBurnedCounter != -1)
    {
        aBodyReanim->mEnableExtraOverlayDraw = false;
    }
    if (mBeghouledFlashCountdown > 0)
    {
        int anAlpha = TodAnimateCurve(50, 0, mBeghouledFlashCountdown % 50, 0, 128, TodCurves::CURVE_BOUNCE);
        aBodyReanim->mExtraOverlayColor = Color(255, 255, 255, anAlpha);
        aBodyReanim->mEnableExtraOverlayDraw = true;
    }
    else
    {
        aBodyReanim->mEnableExtraOverlayDraw = false;
    }

    aBodyReanim->PropogateColorToAttachments();
}

bool Plant::IsOnBoard()
{
    if (!mIsOnBoard)
        return false;

    TOD_ASSERT(mBoard);
    return true;
}

//0x4638F0
bool Plant::IsInPlay()
{
    return IsOnBoard() && mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN && mApp->mGameMode != GameMode::GAMEMODE_TREE_OF_WISDOM;
}

//0x463920
void Plant::UpdateReanim()
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim == nullptr)
        return;

    UpdateReanimColor();

    float aOffsetX = mShakeOffsetX;
    float aOffsetY = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow) + mShakeOffsetY;
    float aScaleX = 1.0f, aScaleY = 1.0f;
    if (
#ifdef DO_FIX_BUGS
        !mApp->GetDialog(Dialogs::DIALOG_ALMANAC) &&
#endif
        (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BIG_TIME) &&
        (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_SUNFLOWER || mSeedType == SeedType::SEED_MARIGOLD))
    {
        aScaleX = 1.5f;
        aScaleY = 1.5f;
        aOffsetX -= 20.0f;
        aOffsetY -= 40.0f;
    }
    if (mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        aScaleX = 2.0f;
        aScaleY = 2.0f;
        aOffsetX -= 42.7f + (mApp->IsWallnutBowlingLevel() ? 33.3f : 0.0f);
        aOffsetY -= 64.0f;
    }
    if (mSeedType == SeedType::SEED_INSTANT_COFFEE)
    {
        aScaleX = 0.8f;
        aScaleY = 0.8f;
        aOffsetX += 12.0f;
        aOffsetY += 10.0f;
    }
    if (mSeedType == SeedType::SEED_POTATOMINE)
    {
        aScaleX = 0.8f;
        aScaleY = 0.8f;
        aOffsetX += 12.0f;
        aOffsetY += 12.0f;
    }
    if (mState == PlantState::STATE_GRAVEBUSTER_EATING)
    {
        aOffsetY += TodAnimateCurveFloat(400, 0, mStateCountdown, 0.0f, 30.0f, TodCurves::CURVE_LINEAR);
    }
    if (mWakeUpCounter > 0)
    {
        float aScaleFactor = TodAnimateCurveFloat(70, 0, mWakeUpCounter, 1.0f, 0.8f, TodCurves::CURVE_EASE_SIN_WAVE);
        aScaleY *= aScaleFactor;
        aOffsetY += 80.0f - 80.0f * aScaleFactor;
    }
    if (mPoweredCounter >= 1430 && mPoweredCounter <= 1500)
    {
        float aScaleFactor = TodAnimateCurveFloat(1500, 1430, mPoweredCounter, 1.0f, 0.8f, TodCurves::CURVE_EASE_SIN_WAVE);
        aScaleY *= aScaleFactor;
        aOffsetY += 80.0f - 80.0f * aScaleFactor;

    }

    // @DrunkenCat
    if (mBoard && (mSeedType == SeedType::SEED_FUMESHROOM || mSeedType == SeedType::SEED_GLOOMSHROOM) && mBoard->StageHasRoof()) {
        TodParticleSystem* aParticle = mApp->ParticleTryToGet(mParticleID);
        if (aParticle) {
            for (TodListNode<ParticleEmitterID>* aNode = aParticle->mEmitterList.mHead; aNode != nullptr; aNode = aNode->mNext)
            {
                TodParticleEmitter* aEmitter = aParticle->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
                for (TodListNode<ParticleID>* aNode = aEmitter->mParticleList.mHead; aNode != nullptr; aNode = aNode->mNext)
                {
                    TodParticle* aParti = aEmitter->mParticleSystem->mParticleHolder->mParticles.DataArrayGet((unsigned int)aNode->mValue);
                    float aSlopeHeightChange = mBoard->GetPosYBasedOnRow(aParti->mPosition.x, mRow) + ((mPlantCol > 4) ? 21.f : 47.f) - aEmitter->mSystemCenter.y;
                    aParti->mPosition.y += aSlopeHeightChange;
                    aParti->mVelocity.y = -aSlopeHeightChange;
                }
            }
        }
    }

    aBodyReanim->Update();

    if (mSeedType == SeedType::SEED_LEFTPEATER)
    {
        aOffsetX += 80.0f * aScaleX;
        aScaleX *= -1.0f;
    }

    if (mPottedPlantIndex != -1)
    {
        PottedPlant* aPottedPlant = &mApp->mPlayerInfo->mPottedPlant[mPottedPlantIndex];

        if (aPottedPlant->mFacing == PottedPlant::FacingDirection::FACING_LEFT)
        {
            aOffsetX += 80.0f * aScaleX;
            aScaleX *= -1.0f;
        }

        float aOffsetXStart, aOffsetXEnd;
        float aOffsetYStart, aOffsetYEnd;
        float aScaleStart, aScaleEnd;
        if (aPottedPlant->mPlantAge == PottedPlantAge::PLANTAGE_SMALL)
        {
            aOffsetXStart = 20.0f;
            aOffsetXEnd = 20.0f;
            aOffsetYStart = 40.0f;
            aOffsetYEnd = 40.0f;
            aScaleStart = 0.5f;
            aScaleEnd = 0.5f;
        }
        else if (aPottedPlant->mPlantAge == PottedPlantAge::PLANTAGE_MEDIUM)
        {
            aOffsetXStart = 20.0f;
            aOffsetXEnd = 10.0f;
            aOffsetYStart = 40.0f;
            aOffsetYEnd = 20.0f;
            aScaleStart = 0.5f;
            aScaleEnd = 0.75f;
        }
        else
        {
            aOffsetXStart = 10.0f;
            aOffsetXEnd = 0.0f;
            aOffsetYStart = 20.0f;
            aOffsetYEnd = 0.0f;
            aScaleStart = 0.75f;
            aScaleEnd = 1.0f;
        }

        float aAnimatedOffsetX = TodAnimateCurveFloat(100, 0, mStateCountdown, aOffsetXStart, aOffsetXEnd, TodCurves::CURVE_LINEAR);
        float aAnimatedOffsetY = TodAnimateCurveFloat(100, 0, mStateCountdown, aOffsetYStart, aOffsetYEnd, TodCurves::CURVE_LINEAR);
        float aAnimatedScale = TodAnimateCurveFloat(100, 0, mStateCountdown, aScaleStart, aScaleEnd, TodCurves::CURVE_LINEAR);

        aOffsetX += aAnimatedOffsetX * aScaleX;
        aOffsetY += aAnimatedOffsetY * aScaleY;
        aScaleX *= aAnimatedScale;
        aScaleY *= aAnimatedScale;
        aOffsetX += mApp->mZenGarden->ZenPlantOffsetX(aPottedPlant);
        aOffsetY += mApp->mZenGarden->PlantPottedDrawHeightOffset(mSeedType, aScaleY);
    }

    aBodyReanim->SetPosition(aOffsetX, aOffsetY);
    aBodyReanim->OverrideScale(aScaleX, aScaleY);
}

//0x463E40
void Plant::Update()
{
    bool doUpdate = false;
    if (IsOnBoard() && mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO && mApp->IsWallnutBowlingLevel())
        doUpdate = true;
    else if (IsOnBoard() && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
        doUpdate = true;
    else if (IsOnBoard() && mBoard->mCutScene->ShouldRunUpsellBoard())
        doUpdate = true;
    else if (!IsOnBoard() || mApp->mGameScene == GameScenes::SCENE_PLAYING)
        doUpdate = true;

    if (doUpdate)
    {
        if (mBurnedCounter != -1)   
        {
            UpdateBurn();
            return;
        }

        if (mPoweredCounter != -1) 
            UpdatePowered();

        UpdateAbilities();
        Animate();

        if (mPlantHealth < 0)
            Die();

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        if (mBoard && mSeedType == SeedType::SEED_TIMESTOPPER && mDoSpecialCountdown == -1 && mBoard->mTimeStopCounter == 0)
            Die();
#endif
        
        UpdateReanim();
    }
}

//0x463EF0
bool Plant::NotOnGround()
{
    if (mSeedType == SeedType::SEED_SQUASH)
    {
        if (mState == PlantState::STATE_SQUASH_RISING || mState == PlantState::STATE_SQUASH_FALLING || mState == PlantState::STATE_SQUASH_DONE_FALLING)
            return true;
    }

    return mSquished || mOnBungeeState == PlantOnBungeeState::RISING_WITH_BUNGEE || mDead;
}

//0x463F30
Reanimation* Plant::AttachBlinkAnim(Reanimation* theReanimBody)
{
    const PlantDefinition& aPlantDef = GetPlantDefinition(mSeedType);
    LawnApp* aApp = (LawnApp*)gSexyAppBase;
    Reanimation* aAnimToAttach = theReanimBody;
    const char* aTrackToPlay = "anim_blink";
    const char* aTrackToAttach = nullptr;

    if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_TALLNUT || 
        mSeedType == SeedType::SEED_EXPLODE_O_NUT || mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        int aHit = Rand(10);
        if (aHit < 1 && theReanimBody->TrackExists("anim_blink_twitch"))
        {
            aTrackToPlay = "anim_blink_twitch";
        }
        else
        {
            aTrackToPlay = aHit < 7 ? "anim_blink_twice" : "anim_blink_thrice";
        }
    }
    else if (mSeedType == SeedType::SEED_THREEPEATER)
    {
        int aHit = Rand(3);
        if (aHit == 0)
        {
            aTrackToPlay = "anim_blink1";
            aTrackToAttach = "anim_face1";
            ReanimatorTrackInstance* aTrackInstance = theReanimBody->GetTrackInstanceByName("anim_head1");
            aAnimToAttach = FindReanimAttachment(aTrackInstance->mAttachmentID);
        }
        else if (aHit == 1)
        {
            aTrackToPlay = "anim_blink2";
            aTrackToAttach = "anim_face2";
            ReanimatorTrackInstance* aTrackInstance = theReanimBody->GetTrackInstanceByName("anim_head2");
            aAnimToAttach = FindReanimAttachment(aTrackInstance->mAttachmentID);
        }
        else
        {
            aTrackToPlay = "anim_blink3";
            aTrackToAttach = "anim_face3";
            ReanimatorTrackInstance* aTrackInstance = theReanimBody->GetTrackInstanceByName("anim_head3");
            aAnimToAttach = FindReanimAttachment(aTrackInstance->mAttachmentID);
        }
    }
    else if (mSeedType == SeedType::SEED_SPLITPEA)
    {
        if (Rand(2) == 0)
        {
            aTrackToPlay = "anim_blink";
            aTrackToAttach = "anim_face";
            aAnimToAttach = mApp->ReanimationTryToGet(mHeadReanimID);
        }
        else
        {
            aTrackToPlay = "anim_blink2";
            aTrackToAttach = "anim_face2";
            aAnimToAttach = mApp->ReanimationTryToGet(mHeadReanimID2);
        }
    }
    else if (mSeedType == SeedType::SEED_TWINSUNFLOWER)
    {
        if (Rand(2) == 0)
        {
            aTrackToPlay = "anim_blink";
            aTrackToAttach = "anim_face";
        }
        else
        {
            aTrackToPlay = "anim_blink2";
            aTrackToAttach = "anim_face2";
        }
    }
    else if (mSeedType == SeedType::SEED_PEASHOOTER || mSeedType == SeedType::SEED_SNOWPEA || mSeedType == SeedType::SEED_REPEATER || mSeedType == SeedType::SEED_LEFTPEATER || mSeedType == SeedType::SEED_GATLINGPEA)
    {
        if (theReanimBody->TrackExists("anim_stem"))
        {
            ReanimatorTrackInstance* aTrackInstance = theReanimBody->GetTrackInstanceByName("anim_stem");
            aAnimToAttach = FindReanimAttachment(aTrackInstance->mAttachmentID);
        }
        else if (theReanimBody->TrackExists("anim_idle"))
        {
            ReanimatorTrackInstance* aTrackInstance = theReanimBody->GetTrackInstanceByName("anim_idle");
            aAnimToAttach = FindReanimAttachment(aTrackInstance->mAttachmentID);
        }
    }

    if (aAnimToAttach == nullptr)
    {
        TodTrace("Missing head anim");
        return nullptr;
    }

    if (!theReanimBody->TrackExists(aTrackToPlay))
        return nullptr;
    
    ReanimationType aReanimationType = aPlantDef.mReanimationType;

#ifdef _DS_MINIGAMES
    bool isHeatWave = mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE;

    if (isHeatWave)
    {
        aReanimationType = ReanimationType::REANIM_HEATWAVE_SHOOTER;

        if (mState == PlantState::STATE_HEAT_WAVE_POWERED)
            aTrackToPlay = "anim_blink_powered";
        else if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED)
            aTrackToPlay = "anim_blink_exhausted";
        else if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED_2)
            aTrackToPlay = "anim_blink_exhausted2";
    }
#endif
    Reanimation* aBlinkReanim = aApp->mEffectSystem->mReanimationHolder->AllocReanimation(0.0f, 0.0f, 0, aReanimationType);
    aBlinkReanim->SetFramesForLayer(aTrackToPlay);
    aBlinkReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_FULL_LAST_FRAME_AND_HOLD;
    aBlinkReanim->mAnimRate = 15.0f;
    aBlinkReanim->mColorOverride = theReanimBody->mColorOverride;

    if (aTrackToAttach && aAnimToAttach->TrackExists(aTrackToAttach))
    {
        aBlinkReanim->AttachToAnotherReanimation(aAnimToAttach, aTrackToAttach);
    }
    else if (aAnimToAttach->TrackExists("anim_face"))
    {
        aBlinkReanim->AttachToAnotherReanimation(aAnimToAttach, "anim_face");
    }
    else if (aAnimToAttach->TrackExists("anim_idle"))
    {
        aBlinkReanim->AttachToAnotherReanimation(aAnimToAttach, "anim_idle");
    }
    else
    {
        TodTrace("Missing anim_idle for blink");
    }

    aBlinkReanim->mFilterEffect = theReanimBody->mFilterEffect;
    return aBlinkReanim;
}

//0x464230
void Plant::DoBlink()
{
    mBlinkCountdown = 400 + Rand(400);

    if (NotOnGround() || mShootingCounter != 0)
        return;

    if (mSeedType == SeedType::SEED_POTATOMINE && mState != PlantState::STATE_POTATO_ARMED)
        return;

    if (mState == PlantState::STATE_CACTUS_RISING || mState == PlantState::STATE_CACTUS_HIGH || mState == PlantState::STATE_CACTUS_LOWERING ||
        mState == PlantState::STATE_MAGNETSHROOM_SUCKING || mState == PlantState::STATE_MAGNETSHROOM_CHARGING || mState == PlantState::STATE_SCAREDYSHROOM_LOWERING ||
        mState == PlantState::STATE_SCAREDYSHROOM_SCARED)
        return;

    EndBlink();
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim == nullptr)
        return;

    if ((mSeedType == SeedType::SEED_TALLNUT && aBodyReanim->GetImageOverride("anim_idle") == IMAGE_REANIM_TALLNUT_CRACKED2) || 
        (mSeedType == SeedType::SEED_GARLIC && aBodyReanim->GetImageOverride("anim_face") == IMAGE_REANIM_GARLIC_BODY3))
        return;

    if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_TALLNUT || 
        mSeedType == SeedType::SEED_EXPLODE_O_NUT || mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        mBlinkCountdown = 1000 + Rand(1000);
    }

    Reanimation* aBlinkReanim = AttachBlinkAnim(aBodyReanim);
    if (aBlinkReanim)
    {
        mBlinkReanimID = mApp->ReanimationGetID(aBlinkReanim);
    }
    aBodyReanim->AssignRenderGroupToPrefix("anim_eye", RENDER_GROUP_HIDDEN);
}

//0x464390
void Plant::EndBlink()
{
    if (mBlinkReanimID != ReanimationID::REANIMATIONID_NULL)
    {
        mApp->RemoveReanimation(mBlinkReanimID);
        mBlinkReanimID = ReanimationID::REANIMATIONID_NULL;

        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aBodyReanim)
        {
            aBodyReanim->AssignRenderGroupToPrefix("anim_eye", RENDER_GROUP_NORMAL);
        }
    }
}

//0x464410
void Plant::UpdateBlink()
{
    if (mBlinkReanimID != ReanimationID::REANIMATIONID_NULL)
    {
        Reanimation* aBlinkReanim = mApp->ReanimationTryToGet(mBlinkReanimID);
        if (aBlinkReanim == nullptr || aBlinkReanim->mLoopCount > 0)
        {
            EndBlink();
        }
    }

    if (mIsAsleep)
        return;

    if (mBlinkCountdown > 0)
    {
        mBlinkCountdown--;
        if (mBlinkCountdown == 0)
        {
            DoBlink();
        }
    }
}

//0x464480
void Plant::AnimateNuts()
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim == nullptr)
        return;

    Image* aCracked1;
    Image* aCracked2;
    const char* aTrackToOverride;
    if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_EXPLODE_O_NUT || mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        aCracked1 = IMAGE_REANIM_WALLNUT_CRACKED1;
        aCracked2 = IMAGE_REANIM_WALLNUT_CRACKED2;
        aTrackToOverride = "anim_face";
    }
    else if (mSeedType == SeedType::SEED_TALLNUT)
    {
        aCracked1 = IMAGE_REANIM_TALLNUT_CRACKED1;
        aCracked2 = IMAGE_REANIM_TALLNUT_CRACKED2;
        aTrackToOverride = "anim_idle";
    }
    else return;

    int aPosX = mX + 40;
    int aPosY = mY + 10;
    if (mSeedType == SeedType::SEED_TALLNUT)
    {
        aPosY -= 32;
    }

    Image* aImageOverride = aBodyReanim->GetImageOverride(aTrackToOverride);
    if (mPlantHealth < mPlantMaxHealth / 3)
    {
        if (aImageOverride != aCracked2)
        {
            aBodyReanim->SetImageOverride(aTrackToOverride, aCracked2);
            TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, mRenderOrder + 4, ParticleEffect::PARTICLE_WALLNUT_EAT_LARGE);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
            {
                aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
            }
            if (mSeedType == SeedType::SEED_EXPLODE_O_NUT)
                aParticle->OverrideColor(nullptr, Color(255, 64, 64));
        }
    }
    else if (mPlantHealth < mPlantMaxHealth * 2 / 3)
    {
        if (aImageOverride != aCracked1)
        {
            aBodyReanim->SetImageOverride(aTrackToOverride, aCracked1);
            TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, mRenderOrder + 4, ParticleEffect::PARTICLE_WALLNUT_EAT_LARGE);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
            {
                aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
            }
            if (mSeedType == SeedType::SEED_EXPLODE_O_NUT)
                aParticle->OverrideColor(nullptr, Color(255, 64, 64));
        }
    }
    else
    {
        aBodyReanim->SetImageOverride(aTrackToOverride, nullptr);
    }

    if (IsInPlay() && !mApp->IsIZombieLevel())
    {
        if (mRecentlyEatenCountdown > 0)
        {
            aBodyReanim->mAnimRate = 0.0f;
            return;
        }

        if (aBodyReanim->mAnimRate < 1.0f && mOnBungeeState != PlantOnBungeeState::RISING_WITH_BUNGEE)
        {
            aBodyReanim->mAnimRate = RandRangeFloat(10.0f, 15.0f);
        }
    }
}

//0x464680
void Plant::AnimateGarlic()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    Image* aImageOverride = aBodyReanim->GetImageOverride("anim_face");

    if (mPlantHealth < mPlantMaxHealth / 3)
    {
        if (aImageOverride != IMAGE_REANIM_GARLIC_BODY3)
        {
            aBodyReanim->SetImageOverride("anim_face", IMAGE_REANIM_GARLIC_BODY3);
            aBodyReanim->AssignRenderGroupToPrefix("Garlic_stem", RENDER_GROUP_HIDDEN);
        }
    }
    else if (mPlantHealth < mPlantMaxHealth * 2 / 3)
    {
        if (aImageOverride != IMAGE_REANIM_GARLIC_BODY2)
        {
            aBodyReanim->SetImageOverride("anim_face", IMAGE_REANIM_GARLIC_BODY2);
        }
    }
    else
    {
        aBodyReanim->SetImageOverride("anim_face", nullptr);
    }
}

//0x464760
void Plant::AnimatePumpkin()
{
    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    Image* aImageOverride = aBodyReanim->GetImageOverride("Pumpkin_front");

    if (mPlantHealth < mPlantMaxHealth / 3)
    {
        if (aImageOverride != IMAGE_REANIM_PUMPKIN_DAMAGE3)
            aBodyReanim->SetImageOverride("Pumpkin_front", IMAGE_REANIM_PUMPKIN_DAMAGE3);
    }
    else if (mPlantHealth < mPlantMaxHealth * 2 / 3)
    {
        if (aImageOverride != IMAGE_REANIM_PUMPKIN_DAMAGE1)
            aBodyReanim->SetImageOverride("Pumpkin_front", IMAGE_REANIM_PUMPKIN_DAMAGE1);
    }
    else
    {
        aBodyReanim->SetImageOverride("Pumpkin_front", nullptr);
    }
}

//0x464820
void Plant::UpdateShooting()
{
    if (NotOnGround() || mShootingCounter == 0)
        return;

    mShootingCounter--;

    if (mSeedType == SeedType::SEED_FUMESHROOM && mShootingCounter == 15)
    {
        int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PARTICLE, mRow, 0);
        AddAttachedParticle(mX + 85, mY + 31, aRenderPosition, ParticleEffect::PARTICLE_FUMECLOUD);
    }

    if (mSeedType == SeedType::SEED_GLOOMSHROOM)
    {
        if (mShootingCounter == 136 || mShootingCounter == 108 || mShootingCounter == 80 || mShootingCounter == 52)
        {
            int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PARTICLE, mRow, 0);
            AddAttachedParticle(mX + 40, mY + 40, aRenderPosition, ParticleEffect::PARTICLE_GLOOMCLOUD);
        }
        if (mShootingCounter == 126 || mShootingCounter == 98 || mShootingCounter == 70 || mShootingCounter == 42)
        {
            Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
        }
    }
    else if (mSeedType == SeedType::SEED_GATLINGPEA)
    {
        if (mShootingCounter == 18 || mShootingCounter == 35 || mShootingCounter == 51 || mShootingCounter == 68)
        {
            Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
        }
    }
    else if (mSeedType == SeedType::SEED_REPEATER || mSeedType == SeedType::SEED_LEFTPEATER)
    {
        if (mShootingCounter == 25)
        {
            Reanimation* aHeadReanim = mApp->ReanimationTryToGet(mHeadReanimID);
            aHeadReanim->StartBlend(20);
            aHeadReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
            aHeadReanim->mAnimRate = 45.0f;
            aHeadReanim->SetFramesForLayer("anim_shooting");
        }
        else if (mShootingCounter == 1 || mShootingCounter == 26)
        {
            Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
        }
    }
    else if (mSeedType == SeedType::SEED_SPLITPEA)
    {
        Reanimation* aHeadFrontReanim = mApp->ReanimationTryToGet(mHeadReanimID);
        Reanimation* aHeadBackReanim = mApp->ReanimationTryToGet(mHeadReanimID2);

        if (aHeadBackReanim->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
        {
            if (mShootingCounter == 25)
            {
                aHeadBackReanim->StartBlend(20);
                aHeadBackReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
                aHeadBackReanim->mAnimRate = 45.0f;
                aHeadBackReanim->SetFramesForLayer("anim_splitpea_shooting");
            }
            else if (mShootingCounter == 1 || mShootingCounter == 26)
            {
                Fire(nullptr, mRow, PlantWeapon::WEAPON_SECONDARY);
            }
        }

        if (aHeadFrontReanim->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
        {
            if (mShootingCounter == 26)
            {
                Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
            }
        }
    }
    else if (mSeedType == SeedType::SEED_CATTAIL)
    {
        if (mShootingCounter == 19)
        {
            Zombie* aZombie = FindTargetZombie(mRow, PlantWeapon::WEAPON_PRIMARY);
            if (aZombie)
            {
                Fire(aZombie, mRow, PlantWeapon::WEAPON_PRIMARY);
            }
        }
    }
    else if (mShootingCounter == 1)
    {
        if (mSeedType == SeedType::SEED_THREEPEATER)
        {
            int rowAbove = mRow - 1;
            int rowBelow = mRow + 1;
            Reanimation* aHeadReanim2 = mApp->ReanimationGet(mHeadReanimID2);
            Reanimation* aHeadReanim3 = mApp->ReanimationGet(mHeadReanimID3);
            Reanimation* aHeadReanim1 = mApp->ReanimationGet(mHeadReanimID);

            if (aHeadReanim1->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
            {
                Fire(nullptr, rowBelow, PlantWeapon::WEAPON_PRIMARY);
            }
            if (aHeadReanim2->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
            {
                Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
            }
            if (aHeadReanim3->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
            {
                Fire(nullptr, rowAbove, PlantWeapon::WEAPON_PRIMARY);
            }
        }
        else if (mState == PlantState::STATE_CACTUS_LOW)
        {
            Fire(nullptr, mRow, PlantWeapon::WEAPON_SECONDARY);
        }
        else if (mSeedType == SeedType::SEED_CABBAGEPULT || mSeedType == SeedType::SEED_KERNELPULT || mSeedType == SeedType::SEED_MELONPULT || mSeedType == SeedType::SEED_WINTERMELON
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
                || mSeedType == SeedType::SEED_ICEBERGLETTUCE
#endif
            )
        {
            PlantWeapon aPlantWeapon = PlantWeapon::WEAPON_PRIMARY;
            if (mState == PlantState::STATE_KERNELPULT_BUTTER)
            {
                Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_butter", RENDER_GROUP_HIDDEN);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_kernal", RENDER_GROUP_NORMAL);
                mState = PlantState::STATE_NOTREADY;
                aPlantWeapon = PlantWeapon::WEAPON_SECONDARY;
            }
#ifdef _HAS_KERNELPULT_BUTTER_IDLE
            if (Sexy::Rand(4) == 0)
            {
                Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
                aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_butter", RENDER_GROUP_NORMAL);
                aBodyReanim->AssignRenderGroupToPrefix("Cornpult_kernal", RENDER_GROUP_HIDDEN);
                mState = PlantState::STATE_KERNELPULT_BUTTER;
            }
#endif

            Zombie* aZombie = FindTargetZombie(mRow, aPlantWeapon);
            Fire(aZombie, mRow, aPlantWeapon);
        }
        else
        {
            Fire(nullptr, mRow, PlantWeapon::WEAPON_PRIMARY);
        }

        return;
    }

    if (mShootingCounter != 0)
        return;

    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    Reanimation* aHeadReanim = mApp->ReanimationTryToGet(mHeadReanimID);
    if (mSeedType == SeedType::SEED_THREEPEATER)
    {
        Reanimation* aHeadReanim2 = mApp->ReanimationGet(mHeadReanimID2);
        Reanimation* aHeadReanim3 = mApp->ReanimationGet(mHeadReanimID3);

        if (aHeadReanim2->mLoopCount > 0)
        {
            if (aHeadReanim->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
            {
                aHeadReanim->StartBlend(20);
                aHeadReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
                aHeadReanim->SetFramesForLayer("anim_head_idle1");
                aHeadReanim->mAnimRate = aBodyReanim->mAnimRate;
                aHeadReanim->mAnimTime = aBodyReanim->mAnimTime;
            }

            aHeadReanim2->StartBlend(20);
            aHeadReanim2->mLoopType = ReanimLoopType::REANIM_LOOP;
            aHeadReanim2->SetFramesForLayer("anim_head_idle2");
            aHeadReanim2->mAnimRate = aBodyReanim->mAnimRate;
            aHeadReanim2->mAnimTime = aBodyReanim->mAnimTime;

            if (aHeadReanim3->mLoopType == ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD)
            {
                aHeadReanim3->StartBlend(20);
                aHeadReanim3->mLoopType = ReanimLoopType::REANIM_LOOP;
                aHeadReanim3->SetFramesForLayer("anim_head_idle3");
                aHeadReanim3->mAnimRate = aBodyReanim->mAnimRate;
                aHeadReanim3->mAnimTime = aBodyReanim->mAnimTime;
            }
            
            return;
        }
    }
    else if (mSeedType == SeedType::SEED_SPLITPEA)
    {
        Reanimation* aHeadReanim2 = mApp->ReanimationGet(mHeadReanimID2);

        if (aHeadReanim->mLoopCount > 0)
        {
            aHeadReanim->StartBlend(20);
            aHeadReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
            aHeadReanim->SetFramesForLayer("anim_head_idle");
            aHeadReanim->mAnimRate = aBodyReanim->mAnimRate;
            aHeadReanim->mAnimTime = aBodyReanim->mAnimTime;
        }

        if (aHeadReanim2->mLoopCount > 0)
        {
            aHeadReanim2->StartBlend(20);
            aHeadReanim2->mLoopType = ReanimLoopType::REANIM_LOOP;
            aHeadReanim2->SetFramesForLayer("anim_splitpea_idle");
            aHeadReanim2->mAnimRate = aBodyReanim->mAnimRate;
            aHeadReanim2->mAnimTime = aBodyReanim->mAnimTime;
        }

        return;
    }
    else if (mState == PlantState::STATE_CACTUS_HIGH)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            PlayBodyReanim("anim_idlehigh", ReanimLoopType::REANIM_LOOP, 20, 0.0f);

            aBodyReanim->mAnimRate = aBodyReanim->mDefinition->mFPS;
            if (mApp->IsIZombieLevel())
            {
                aBodyReanim->mAnimRate = 0.0f;
            }

            return;
        }
    }
    else if (aHeadReanim)
    {
        if (aHeadReanim->mLoopCount > 0)
        {
            const char* animTrack = "anim_head_idle";

#ifdef _DS_MINIGAMES
            if (mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
            {
                bool isExhausted = mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED || mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED_2;

                if (isExhausted)
                {
                    animTrack = mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED ? "anim_head_tired" : "anim_head_exhausted";
                }

                if (isExhausted && !aBodyReanim->IsAnimPlaying("anim_exhausted")) 
                {
                    aBodyReanim->StartBlend(20);
                    aBodyReanim->SetFramesForLayer("anim_exhausted");
                }
                else if (!isExhausted && !aBodyReanim->IsAnimPlaying("anim_idle")) 
                {
                    aBodyReanim->StartBlend(20);
                    aBodyReanim->SetFramesForLayer("anim_idle");
                }
            }
#endif
            aHeadReanim->StartBlend(20);
            aHeadReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
            aHeadReanim->SetFramesForLayer(animTrack);
            aHeadReanim->mAnimRate = aBodyReanim->mAnimRate;
            aHeadReanim->mAnimTime = aBodyReanim->mAnimTime;
            return;
        }
    }
    else if (mSeedType == SeedType::SEED_COBCANNON)
    {
        if (aBodyReanim->mLoopCount > 0)
        {
            mState = PlantState::STATE_COBCANNON_ARMING;
            mStateCountdown = 3000;
            PlayBodyReanim("anim_unarmed_idle", ReanimLoopType::REANIM_LOOP, 20, aBodyReanim->mDefinition->mFPS);
            return;
        }
    }
    else if (aBodyReanim && aBodyReanim->mLoopCount > 0)
    {
        PlayIdleAnim(aBodyReanim->mDefinition->mFPS);
        return;
    }

    mShootingCounter = 1;
}

//0x464DB0
void Plant::Animate()
{
    if (!mApp->GetDialog(Dialogs::DIALOG_ALMANAC) && (mSeedType == SeedType::SEED_CHERRYBOMB || mSeedType == SeedType::SEED_JALAPENO 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || mSeedType == SeedType::SEED_CHERRYHOVERBOMB || mSeedType == SeedType::SEED_NUKECUMBER
#endif
        ) && mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
    {
        mShakeOffsetX = RandRangeFloat(-1.0f, 1.0f);
        mShakeOffsetY = RandRangeFloat(-1.0f, 1.0f);
    }

    if (mRecentlyEatenCountdown > 0)
    {
        mRecentlyEatenCountdown--;
    }
    if (mEatenFlashCountdown > 0)
    {
        mEatenFlashCountdown--;
    }
    if (mBeghouledFlashCountdown > 0)
    {
        mBeghouledFlashCountdown--;
    }

    if (mSquished || mBurnedCounter != -1)
    {
        mFrame = 0;
        return;
    }

    if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_TALLNUT || mSeedType == SeedType::SEED_EXPLODE_O_NUT || mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        AnimateNuts();
    }
    else if (mSeedType == SeedType::SEED_GARLIC)
    {
        AnimateGarlic();
    }
    else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
    {
        AnimatePumpkin();
    }

    UpdateBlink();

    if (mAnimPing)
    {
        if (mAnimCounter < mFrameLength * mNumFrames - 1)
        {
            mAnimCounter++;
        }
        else
        {
            mAnimPing = false;
            mAnimCounter -= mFrameLength;
        }
    }
    else if (mAnimCounter > 0)
    {
        mAnimCounter--;
    }
    else
    {
        mAnimPing = true;
        mAnimCounter += mFrameLength;
    }
    mFrame = mAnimCounter / mFrameLength;
}

//0x464EF0
float PlantFlowerPotHeightOffset(SeedType theSeedType, float theFlowerPotScale)
{
    float aHeightOffset = -5.0f * theFlowerPotScale;
    float aScaleOffsetFix = 0.0f;

    switch (theSeedType)
    {
    case SeedType::SEED_CHOMPER:
    case SeedType::SEED_PLANTERN:
        aHeightOffset -= 5.0f;
        break;
    case SeedType::SEED_SCAREDYSHROOM:
        aHeightOffset += 5.0f;
        aScaleOffsetFix -= 8.0f;
        break;
    case SeedType::SEED_SUNSHROOM:
    case SeedType::SEED_PUFFSHROOM:
        aScaleOffsetFix -= 4.0f;
        break;
    case SeedType::SEED_HYPNOSHROOM:
    case SeedType::SEED_MAGNETSHROOM:
    case SeedType::SEED_PEASHOOTER:
    case SeedType::SEED_REPEATER:
    case SeedType::SEED_LEFTPEATER:
    case SeedType::SEED_SNOWPEA:
    case SeedType::SEED_THREEPEATER:
    case SeedType::SEED_SUNFLOWER:
    case SeedType::SEED_MARIGOLD:
    case SeedType::SEED_CABBAGEPULT:
    case SeedType::SEED_MELONPULT:
    case SeedType::SEED_TANGLEKELP:
    case SeedType::SEED_BLOVER:
    case SeedType::SEED_SPIKEWEED:
        aScaleOffsetFix -= 8.0f;
        break;
    case SeedType::SEED_SEASHROOM:
    case SeedType::SEED_POTATOMINE:
        aScaleOffsetFix -= 4.0f;
        break;
    case SeedType::SEED_LILYPAD:
        aScaleOffsetFix -= 16.0f;
        break;
    case SeedType::SEED_INSTANT_COFFEE:
        aScaleOffsetFix -= 20.0f;
        break;
    }

    return aHeightOffset + (theFlowerPotScale * aScaleOffsetFix - aScaleOffsetFix);
}

//0x465040
float PlantDrawHeightOffset(Board* theBoard, Plant* thePlant, SeedType theSeedType, int theCol, int theRow, bool neverFloat)
{
    float aHeightOffset = 0.0f;

    bool doFloating = false;
    if (Plant::IsFlying(theSeedType))
    {
        doFloating = false;
    }
    else if (theBoard == nullptr)
    {
        if (Plant::IsAquatic(theSeedType))
        {
            doFloating = true;
        }
    }
    else if (theBoard->IsPoolSquare(theCol, theRow) || theBoard->mBackground == BackgroundType::BACKGROUND_ZOMBIQUARIUM && gLawnApp->mDialogMap.size() == 0)
    {
        doFloating = true;
    }

    if (doFloating && !neverFloat)
    {
        int aCounter;
        if (theBoard)
        {
            aCounter = theBoard->mMainCounter;
        }
        else
        {
            aCounter = gLawnApp->mAppCounter;
        }

        float aPos = theRow * PI + theCol * 0.25f * PI;
        float aTime = aCounter * 2.0f * PI / 200.0f;
        float aFloatingHeight = sin(aPos + aTime) * 2.0f;
        aHeightOffset += aFloatingHeight;
    }

    if (theBoard && (thePlant == nullptr || !thePlant->mSquished))
    {
        Plant* aPot = theBoard->GetFlowerPotAt(theCol, theRow);
        if (aPot && !aPot->mSquished && theSeedType != SeedType::SEED_FLOWERPOT)
        {
            aHeightOffset += PlantFlowerPotHeightOffset(theSeedType, 1.0f);
        }
    }

    if (theSeedType == SeedType::SEED_FLOWERPOT)
    {
        aHeightOffset += 26.0f;
    }
    else if (theSeedType == SeedType::SEED_LILYPAD)
    {
        aHeightOffset += 25.0f;
    }
    else if (theSeedType == SeedType::SEED_STARFRUIT)
    {
        aHeightOffset += 10.0f;
    }
    else if (theSeedType == SeedType::SEED_TANGLEKELP)
    {
        aHeightOffset += 24.0f;
    }
    else if (theSeedType == SeedType::SEED_SEASHROOM)
    {
        aHeightOffset += 28.0f;
    }
    else if (theSeedType == SeedType::SEED_INSTANT_COFFEE)
    {
        aHeightOffset -= 20.0f;
    }
    else if (Plant::IsFlying(theSeedType))
    {
        aHeightOffset -= 30.0f;
    }
    else if (theSeedType == SeedType::SEED_CACTUS)
    {
        return aHeightOffset;
    }
    else if (theSeedType == SeedType::SEED_PUMPKINSHELL)
    {
        aHeightOffset += 15.0f;
    }
    else if (theSeedType == SeedType::SEED_PUFFSHROOM)
    {
        aHeightOffset += 5.0f;
    }
    else if (theSeedType == SeedType::SEED_SCAREDYSHROOM)
    {
        aHeightOffset -= 14.0f;
    }
    else if (theSeedType == SeedType::SEED_GRAVEBUSTER)
    {
        aHeightOffset -= 40.0f;
    }
    else if (theSeedType == SeedType::SEED_SPIKEWEED || theSeedType == SeedType::SEED_SPIKEROCK 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || theSeedType == SeedType::SEED_YAMPOLINE
#endif
        )
    {
        int aBottomRow = 4;
        if (theBoard && theBoard->StageHas6Rows())
        {
            aBottomRow = 5;
        }

        if (theSeedType == SeedType::SEED_SPIKEROCK)
        {
            aHeightOffset += 6.0f;
        }

        if (theBoard && theBoard->GetFlowerPotAt(theCol, theRow) && gLawnApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
        {
            aHeightOffset += 5.0f;
        }
        else if (theBoard && theBoard->StageHasRoof())
        {
            aHeightOffset += 15.0f;
        }
        else if (theBoard && theBoard->IsPoolSquare(theCol, theRow))
        {
            aHeightOffset += 0.0f;
        }
        else if (theRow == aBottomRow && theCol >= 7 && theBoard->StageHas6Rows())
        {
            aHeightOffset += 1.0f;
        }
        else if (theRow == aBottomRow && theCol < 7)
        {
            aHeightOffset += 12.0f;
        }
        else
        {
            aHeightOffset += 15.0f;
        }
    }

    return aHeightOffset;
}

//0x465380
void Plant::GetPeaHeadOffset(int& theOffsetX, int& theOffsetY)
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);

    int aTrackIndex = 0;
    if (aBodyReanim->TrackExists("anim_stem"))
    {
        aTrackIndex = aBodyReanim->FindTrackIndex("anim_stem");
    }
    else if(aBodyReanim->TrackExists("anim_idle"))
    {
        aTrackIndex = aBodyReanim->FindTrackIndex("anim_idle");
    }

    ReanimatorTransform aTransform;
    aBodyReanim->GetCurrentTransform(aTrackIndex, &aTransform);
    theOffsetX = aTransform.mTransX * aBodyReanim->mOverlayMatrix.m00;
    theOffsetY = aTransform.mTransY * aBodyReanim->mOverlayMatrix.m11;
}

//0x465460
void Plant::DrawMagnetItems(Graphics* g)
{
    float aOffsetX = 0.0f;
    float aOffsetY = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);

    for (int i = 0; i < MAX_MAGNET_ITEMS; i++)
    {
        MagnetItem* aMagnetItem = &mMagnetItems[i];
        if (aMagnetItem->mItemType != MagnetItemType::MAGNET_ITEM_NONE)
        {
            int aCelRow = 0, aCelCol = 0;
            Image* aImage = nullptr;
            float aScale = 0.8f;

            if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_BUCKET_1)
            {
                aImage = IMAGE_REANIM_ZOMBIE_BUCKET1;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_BUCKET_2)
            {
                aImage = IMAGE_REANIM_ZOMBIE_BUCKET2;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_BUCKET_3)
            {
                aImage = IMAGE_REANIM_ZOMBIE_BUCKET3;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_FOOTBALL_HELMET_1)
            {
                aImage = IMAGE_REANIM_ZOMBIE_FOOTBALL_HELMET;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_FOOTBALL_HELMET_2)
            {
                aImage = IMAGE_REANIM_ZOMBIE_FOOTBALL_HELMET2;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_FOOTBALL_HELMET_3)
            {
                aImage = IMAGE_REANIM_ZOMBIE_FOOTBALL_HELMET3;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_DOOR_1)
            {
                aImage = IMAGE_REANIM_ZOMBIE_SCREENDOOR1;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_DOOR_2)
            {
                aImage = IMAGE_REANIM_ZOMBIE_SCREENDOOR2;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_DOOR_3)
            {
                aImage = IMAGE_REANIM_ZOMBIE_SCREENDOOR3;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_TRASHCAN_1)
            {
                aImage = IMAGE_REANIM_ZOMBIE_TRASHCAN1;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_TRASHCAN_2)
            {
                aImage = IMAGE_REANIM_ZOMBIE_TRASHCAN2;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_TRASHCAN_3)
            {
                aImage = IMAGE_REANIM_ZOMBIE_TRASHCAN3;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_PROPELLER)
            {
                aScale = 1.0f;
                aImage = IMAGE_PROPELLER;
            }
            else if (aMagnetItem->mItemType >= MagnetItemType::MAGNET_ITEM_POGO_1 && aMagnetItem->mItemType <= MagnetItemType::MAGNET_ITEM_POGO_3)
            {
                aCelCol = (int)aMagnetItem->mItemType - (int)MagnetItemType::MAGNET_ITEM_POGO_1;
                aImage = IMAGE_ZOMBIEPOGO;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_LADDER_1)
            {
                aImage = IMAGE_REANIM_ZOMBIE_LADDER_1;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_LADDER_2)
            {
                aImage = IMAGE_REANIM_ZOMBIE_LADDER_1_DAMAGE1;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_LADDER_3)
            {
                aImage = IMAGE_REANIM_ZOMBIE_LADDER_1_DAMAGE2;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_LADDER_PLACED)
            {
                aImage = IMAGE_REANIM_ZOMBIE_LADDER_5;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_JACK_IN_THE_BOX)
            {
                aImage = IMAGE_REANIM_ZOMBIE_JACKBOX_BOX;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_PICK_AXE)
            {
                aImage = IMAGE_REANIM_ZOMBIE_DIGGER_PICKAXE;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_SILVER_COIN)
            {
                aScale = 1.0f;
                aImage = IMAGE_REANIM_COIN_SILVER_DOLLAR;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_GOLD_COIN)
            {
                aScale = 1.0f;
                aImage = IMAGE_REANIM_COIN_GOLD_DOLLAR;
            }
            else if (aMagnetItem->mItemType == MagnetItemType::MAGNET_ITEM_DIAMOND)
            {
                aScale = 1.0f;
                aImage = IMAGE_REANIM_DIAMOND;
            }
            else
            {
                TOD_ASSERT();
            }

            g->PushState();
            
            if (mBurnedCounter != -1)
            {
                g->SetColorizeImages(true);
                g->SetColor(Color::Black);
            }

            if (aScale == 1.0f)
            {
                g->DrawImageCel(aImage, aMagnetItem->mPosX - mX + aOffsetX, aMagnetItem->mPosY - mY + aOffsetY, aCelCol, aCelRow);
            }
            else
            {
                TodDrawImageCelScaledF(g, aImage, aMagnetItem->mPosX - mX + aOffsetX, aMagnetItem->mPosY - mY + aOffsetY, aCelCol, aCelRow, aScale, aScale);
            }

            g->PopState();
        }
    }
}

Image* Plant::GetImage(SeedType theSeedType)
{
    Image** aImages = GetPlantDefinition(theSeedType).mPlantImage;
    return aImages ? aImages[0] : nullptr;
}

//0x465680
void Plant::DrawShadow(Sexy::Graphics* g, float theOffsetX, float theOffsetY)
{
    if (mSeedType == SeedType::SEED_LILYPAD || mSeedType == SeedType::SEED_STARFRUIT || mSeedType == SeedType::SEED_TANGLEKELP || 
        mSeedType == SeedType::SEED_SEASHROOM || mSeedType == SeedType::SEED_COBCANNON || mSeedType == SeedType::SEED_SPIKEWEED || 
        mSeedType == SeedType::SEED_SPIKEROCK || mSeedType == SeedType::SEED_GRAVEBUSTER && IsOnBoard()|| mSeedType == SeedType::SEED_CATTAIL ||
        mSeedType == SeedType::SEED_INSTANT_COFFEE 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || mSeedType == SeedType::SEED_YAMPOLINE 
#endif
        || mOnBungeeState == PlantOnBungeeState::RISING_WITH_BUNGEE || mBurnedCounter != -1)
        return;

    if (IsOnBoard() && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN && mApp->mZenGarden->mGardenType == GardenType::GARDEN_MAIN)
        return;

    //if (IsOnBoard() && mBoard->GetFlowerPotAt(mPlantCol, mRow) && mSeedType != SEED_FLOWERPOT)
    //    return;

    int aShadowType = 0;
    float aShadowOffsetX = -3.0f;
    float aShadowOffsetY = 51.0f;
    float aScale = 1.0f;
    if (mBoard && mBoard->StageIsNight())
    {
        aShadowType = 1;
    }

    if (mSeedType == SeedType::SEED_POTATOMINE) 
    {
        if (mState == STATE_NOTREADY)
        {
            aScale = 0.65f;
        }
        else if (mState == STATE_POTATO_RISING)
        {
            Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
            aScale = 0.65f + 0.35f * aBodyReanim->mAnimTime;
        }
    }

    if (mSeedType == SeedType::SEED_SQUASH)
    {
        if (mBoard)
        {
            aShadowOffsetY += mBoard->GridToPixelY(mPlantCol, mRow) - mY;
        }
        aShadowOffsetY += 5.0f;
    }
    else if (mSeedType == SeedType::SEED_PUFFSHROOM)
    {
        aScale = 0.5f;
        aShadowOffsetY = 42.0f;
    }
    else if (mSeedType == SeedType::SEED_SUNSHROOM)
    {
        aShadowOffsetY = 42.0f;
        if (mState == PlantState::STATE_SUNSHROOM_SMALL)
        {
            aScale = 0.5f;
        }
        else if (mState == PlantState::STATE_SUNSHROOM_GROWING)
        {
            Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
            aScale = 0.5f + 0.5f * aBodyReanim->mAnimTime;
        }
    }
    else if (mSeedType == SeedType::SEED_UMBRELLA)
    {
        aScale = 0.5f;
        aShadowOffsetX = -5.0f;
        aShadowOffsetY = 52.0f;
    }
    else if (mSeedType == SeedType::SEED_FUMESHROOM || mSeedType == SeedType::SEED_GLOOMSHROOM)
    {
        aScale = 1.3f;
        aShadowOffsetY = 47.0f;
    }
    else if (mSeedType == SeedType::SEED_CABBAGEPULT || mSeedType == SeedType::SEED_MELONPULT || mSeedType == SeedType::SEED_WINTERMELON)
    {
        aShadowOffsetY = 47.0f;
    }
    else if (mSeedType == SeedType::SEED_KERNELPULT)
    {
        aShadowOffsetX = 0.0f;
        aShadowOffsetY = 47.0f;
    }
    else if (mSeedType == SeedType::SEED_SCAREDYSHROOM)
    {
        aShadowOffsetX = -9.0f;
        aShadowOffsetY = 55.0f;
    }
    else if (mSeedType == SeedType::SEED_CHOMPER)
    {
        aShadowOffsetX = -15.0f;
        aShadowOffsetY = 57.0f;
    }
    else if (mSeedType == SeedType::SEED_FLOWERPOT)
    {
        aShadowOffsetX = -4.0f;
        aShadowOffsetY = 46.0f;
    }
    else if (mSeedType == SeedType::SEED_TALLNUT)
    {
        aShadowOffsetY = 54.0f;
        aScale = 1.3f;
    }
    else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
    {
        aShadowOffsetY = 46.0f;
        aScale = 1.4f;
    }
    else if (mSeedType == SeedType::SEED_CACTUS)
    {
        aShadowOffsetX = -8.0f;
        aShadowOffsetY = 50.0f;
    }
    else if (mSeedType == SeedType::SEED_PLANTERN)
    {
        aShadowOffsetY = 57.0f;
    }
    else if (mSeedType == SeedType::SEED_INSTANT_COFFEE)
    {
        aShadowOffsetY = 71.0f;
    }
    else if (mSeedType == SeedType::SEED_GIANT_WALLNUT)
    {
        aShadowOffsetX = mApp->IsWallnutBowlingLevel() ? -33.0f : 0.0f;
        aShadowOffsetY = 56.0f;
        aScale = 1.7f;
    }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    if (mSeedType == SeedType::SEED_STINGER)
    {
        aShadowOffsetY += 90.0f;
        if (mBoard && (mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_NORMAL_POSITION) || mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_PUMPKIN)))
            return;
    }
    else if (mSeedType == SeedType::SEED_CHERRYHOVERBOMB)
    {
        aShadowOffsetY += 30.0f;
        if (mBoard && (mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_NORMAL_POSITION) || mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_PUMPKIN)))
            return;
    }
    else 
#endif
    if (Plant::IsFlying(mSeedType))
    {
        aShadowOffsetY += 10.0f;
        if (mBoard && (mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_NORMAL_POSITION) || mBoard->GetTopPlantAt(mPlantCol, mRow, TOPPLANT_ONLY_PUMPKIN)))
            return;
    }

    if (FloatApproxEqual(mRad, 0.0f))
    {
        float heightOffset = 0;
        if (mSeedType != SeedType::SEED_FLOWERPOT)
            heightOffset = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);
        float rotatedHeightX = sin(mRad) * heightOffset;
        aShadowOffsetX += rotatedHeightX;
    }

    if (aShadowType == 0)
    {
        TodDrawImageCelCenterScaledF(g, IMAGE_PLANTSHADOW, theOffsetX + aShadowOffsetX, theOffsetY + aShadowOffsetY, 0, aScale, aScale);
    }
    else
    {
        TodDrawImageCelCenterScaledF(g, IMAGE_PLANTSHADOW2, theOffsetX + aShadowOffsetX, theOffsetY + aShadowOffsetY, 0, aScale, aScale);
    }
}

//0x465A90
void Plant::Draw(Graphics* g)
{
    float aOffsetX = 0.0f;
    float aOffsetY = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);
    if (Plant::IsFlying(mSeedType) && mSquished)
    {
        aOffsetY += 30.0f;
    }

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    if (!((LawnApp*)gSexyAppBase)->GetDialog(Dialogs::DIALOG_ALMANAC) && mSeedType == SeedType::SEED_STINGER)
    {
        aOffsetY -= 60.0f;
    }
#endif

    int aImageIndex = mFrame;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    if (mSeedType == SeedType::SEED_DUPLICATORCYCLER) {
        aImageIndex = 9;
    }
#endif
    Image* aPlantImage = Plant::GetImage(mSeedType);

    if (mSquished)
    {
        DrawVariation drawVar = DrawVariation::VARIATION_NORMAL;
        DrawFilterVariation filterVar = DrawFilterVariation::FILTERVARIATION_NONE;
        unsigned int aBitVar = 0U;

        if (mApp->IsIZombieLevel()) SetBit(aBitVar, (int)DrawBitVariation::BITVARIATION_IZOMBIE, true);

        if (mImitaterType == SeedType::SEED_IMITATER)
        {
            if (mSeedType == SeedType::SEED_HYPNOSHROOM || mSeedType == SeedType::SEED_SQUASH || mSeedType == SeedType::SEED_POTATOMINE ||
                mSeedType == SeedType::SEED_GARLIC || mSeedType == SeedType::SEED_LILYPAD)
            {
                filterVar = FILTERVARIATION_LESS_WASHED_OUT;
            }
            else
            {
                filterVar = FILTERVARIATION_WASHED_OUT;
            }
        }

        if (mSeedType == SeedType::SEED_FLOWERPOT)
        {
            aOffsetY -= 15.0f;
        }
        else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
        {
            aOffsetY -= 9.0f;
        }
        else if (mSeedType == SeedType::SEED_INSTANT_COFFEE)
        {
            aOffsetY -= 20.0f;
        }
        /*else if (mSeedType == SeedType::SEED_KERNELPULT && mState == PlantState::STATE_KERNELPULT_BUTTER)
        {
            drawVar = DrawVariation::VARIATION_KERNELPULT_WITH_BUTTER;
        }*/
        else  if (mSeedType == SeedType::SEED_SUNSHROOM)
        {
            if (mState == STATE_SUNSHROOM_BIG) 
                drawVar = DrawVariation::VARIATION_SUNSHROOM_BIG;
            
        }

        bool aDrawPumpkinBack = false;

        Plant* aPumpkin = nullptr;
        {
            Plant* aPlant = nullptr;
            while (mBoard->IteratePlants(aPlant))
            {
                if (aPlant->mSeedType == SeedType::SEED_SQUASH &&
                    (aPlant->mState == PlantState::STATE_SQUASH_RISING || aPlant->mState == PlantState::STATE_SQUASH_FALLING || aPlant->mState == PlantState::STATE_SQUASH_DONE_FALLING))
                {
                    continue;
                }
                else if (Plant::IsFlying(aPlant->mSeedType))
                {
                    continue;
                }
                else if (aPlant->mOnBungeeState == PlantOnBungeeState::RISING_WITH_BUNGEE || aPlant->mDead)
                {
                    continue;
                }


                if (aPlant->mPlantCol == mPlantCol && aPlant->mRow == mRow && aPlant->mSquished == mSquished && aPlant->mSeedType == SeedType::SEED_PUMPKINSHELL)
                {
                    aPumpkin = aPlant;
                    break;
                }
            }
        }

        Plant* aPlantInPumpkin = nullptr;

        if (aPumpkin)
        {
            if (aPumpkin != this && mSeedType != SeedType::SEED_PUMPKINSHELL)
            {
                aPlantInPumpkin = this;
                aDrawPumpkinBack = true;
            }
            else
            {
                Plant* aPlant = nullptr;
                while (mBoard->IteratePlants(aPlant))
                {
                    if (Plant::IsFlying(aPlant->mSeedType)) continue;
                    if (aPlant->mSeedType == SeedType::SEED_SQUASH &&
                        (aPlant->mState == PlantState::STATE_SQUASH_RISING || aPlant->mState == PlantState::STATE_SQUASH_FALLING || aPlant->mState == PlantState::STATE_SQUASH_DONE_FALLING))
                    {
                        continue;
                    }

                    if (aPlant->mPlantCol == mPlantCol && aPlant->mRow == mRow && aPlant->mSquished == aPumpkin->mSquished && aPlant->mSeedType != SeedType::SEED_PUMPKINSHELL)
                    {
                        aPlantInPumpkin = aPlant;
                        break;
                    }
                }
            }
        }

        g->PushState();
        g->SetLinearBlend(true);
        g->SetScale(1.0f, 0.25f, 0.0f, 0.0f);

        int anAlpha = 255;
        if (mDisappearCountdown >= 0)
        {
            anAlpha = ClampInt(255 * mDisappearCountdown / 10, 0, 255);
            g->SetColorizeImages(true);
            g->SetColor(Color(255, 255, 255, anAlpha));
        }

        if (aDrawPumpkinBack)
        {
            aPumpkin->DrawPumkin(g, DrawVariation::VARIATION_PUMPKINSHELL_BACK, aBitVar);
        }

        if (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_TALLNUT || mSeedType == SeedType::SEED_PUMPKINSHELL && !aPlantInPumpkin ||
            mSeedType == SeedType::SEED_GARLIC || mSeedType == SeedType::SEED_SPIKEROCK)
        {
            if (mPlantHealth < mPlantMaxHealth / 3) drawVar = DrawVariation::VARIATION_NORMAL_DEGRADE2;
            else if (mPlantHealth < mPlantMaxHealth * 2 / 3) drawVar = DrawVariation::VARIATION_NORMAL_DEGRADE1;
        }
        
        if (mSeedType == SeedType::SEED_PUMPKINSHELL && aPlantInPumpkin)
        {
            if (mPlantHealth < mPlantMaxHealth / 3) drawVar = DrawVariation::VARIATION_PUMPKINSHELL_FRONT_DEGRADE2;
            else if (mPlantHealth < mPlantMaxHealth * 2 / 3) drawVar = DrawVariation::VARIATION_PUMPKINSHELL_FRONT_DEGRADE1;
            else  drawVar = DrawVariation::VARIATION_PUMPKINSHELL_FRONT;
        }

        SeedType jSeedType = mSeedType;
        SeedType jImitaterType = mImitaterType;

        if (mImitaterType == SeedType::SEED_IMITATER)
        {
            jSeedType = mImitaterType;
            jImitaterType = mSeedType;
        }

        DrawSeedType(g, jSeedType, jImitaterType, drawVar, aOffsetX, 60.0f + aOffsetY, aBitVar);
        g->PopState();
    }
    else
    {
        bool aDrawPumpkinBack = false;
        Plant* aPumpkin = nullptr;

        if (IsOnBoard())
        {
            aPumpkin = mBoard->GetPumpkinAt(mPlantCol, mRow);
            if (aPumpkin)
            {
                Plant* aPlantInPumpkin = mBoard->GetTopPlantAt(mPlantCol, mRow, PlantPriority::TOPPLANT_ONLY_NORMAL_POSITION);
                if (aPlantInPumpkin)
                {
                    if (aPlantInPumpkin->mRenderOrder > aPumpkin->mRenderOrder || aPlantInPumpkin->mOnBungeeState == GETTING_GRABBED_BY_BUNGEE)
                    {
                        aPlantInPumpkin = nullptr;
                    }
                }

                if (aPlantInPumpkin == this)
                {
                    aDrawPumpkinBack = true;
                }
                if (aPlantInPumpkin == nullptr && mSeedType == SeedType::SEED_PUMPKINSHELL)
                {
                    aDrawPumpkinBack = true;
                }
            }
            else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
            {
                aDrawPumpkinBack = true;
                aPumpkin = this;
            }
        }
        else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
        {
            aDrawPumpkinBack = true;
            aPumpkin = this;
        }

        DrawShadow(g, aOffsetX, aOffsetY);

        if (Plant::IsFlying(mSeedType))
        {
            int aCounter;
            if (IsOnBoard())
            {
                aCounter = mBoard->mMainCounter;
            }
            else
            {
                aCounter = mApp->mAppCounter;
            }

            float aTime = (mRow * 97 + mPlantCol * 61 + aCounter) * 0.03f;
            float aWave = sin(aTime) * 2.0f;
            aOffsetY += aWave;
        }

        if (aDrawPumpkinBack)
        {
            Reanimation* aPumpkinReanim = mApp->ReanimationGet(aPumpkin->mBodyReanimID);

            g->PushState();
            g->mTransX += aPumpkin->mX - mX;
            g->mTransY += aPumpkin->mY - mY;
            aPumpkinReanim->DrawRenderGroup(g, 1);
            g->PopState();
        }

        aOffsetX += mShakeOffsetX;
        aOffsetY += mShakeOffsetY;
        if (IsInPlay() && mApp->IsIZombieLevel())
        {
            mBoard->mChallenge->IZombieDrawPlant(g, this);
        }
        else if (mBodyReanimID != ReanimationID::REANIMATIONID_NULL)
        {
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aBodyReanim)
            {
                if (!mApp->Is3DAccelerated() && mSeedType == SeedType::SEED_FLOWERPOT && IsOnBoard() && 
                    aBodyReanim->mAnimRate == 0.0f && aBodyReanim->IsAnimPlaying("anim_idle"))
                {
                    unsigned int drawbitVar = 0U;
                    if (mApp->IsIZombieLevel())
                        SetBit(drawbitVar, DrawBitVariation::BITVARIATION_IZOMBIE, true);
                    mApp->mReanimatorCache->DrawCachedPlant(g, aOffsetX, aOffsetY, mSeedType, DrawVariation::VARIATION_NORMAL, (DrawFilterVariation)aBodyReanim->mFilterEffect, drawbitVar);
                }
                else
                {
                    aBodyReanim->Draw(g);
                }
            }
        }
        else
        {

            if (IsInPlay())
            {
                SeedType aSeedType = SeedType::SEED_NONE;
                if (mBoard)
                {
                    aSeedType = mBoard->GetSeedTypeInCursor();
                }

                if (IsPartOfUpgradableTo(aSeedType) && mBoard->CanPlantAt(mPlantCol, mRow, aSeedType) == PlantingReason::PLANTING_OK)
                {
                    g->SetColorizeImages(true);
                    g->SetColor(GetFlashingColor(mBoard->mMainCounter, 90));
                }
                else if (aSeedType == SeedType::SEED_COBCANNON && mBoard->CanPlantAt(mPlantCol - 1, mRow, aSeedType) == PlantingReason::PLANTING_OK)
                {
                    g->SetColorizeImages(true);
                    g->SetColor(GetFlashingColor(mBoard->mMainCounter, 90));
                }
                else if (mBoard && mBoard->mTutorialState == TutorialState::TUTORIAL_SHOVEL_DIG)
                {
                    g->SetColorizeImages(true);
                    g->SetColor(GetFlashingColor(mBoard->mMainCounter, 90));
                }
            }
            

            if (mBurnedCounter != -1)
            {
                g->SetColorizeImages(true);
                g->SetColor(Color::Black);
                TodDrawImageCelF(g, aPlantImage, aOffsetX, aOffsetY, aImageIndex, 0);
                g->SetColorizeImages(false);
            }
            else
            {
                g->SetColorizeImages(false);
                TodDrawImageCelF(g, aPlantImage, aOffsetX, aOffsetY, aImageIndex, 0);
            }

            if (IsInPlay())
            {
                if (mBurnedCounter != -1)
                {
                }
                else if (mHighlighted)
                {
                    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
                    g->SetColorizeImages(true);
                    g->SetColor(Color(255, 255, 255, 196));
                    TodDrawImageCelF(g, aPlantImage, aOffsetX, aOffsetY, aImageIndex, 0);
                    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
                    g->SetColorizeImages(false);
                }
                else if (mEatenFlashCountdown > 0)
                {
                    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
                    g->SetColorizeImages(true);
                    g->SetColor(Color(255, 255, 255, ClampInt(mEatenFlashCountdown * 3, 0, 255)));
                    TodDrawImageCelF(g, aPlantImage, aOffsetX, aOffsetY, aImageIndex, 0);
                    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
                    g->SetColorizeImages(false);
                }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
                if (mSeedType == SeedType::SEED_DUPLICATORCYCLER && IsInPlay() && mApp->mGameScene == GameScenes::SCENE_PLAYING && mDuplicatorSeedType != SeedType::SEED_NONE) {
                    int aCost = mBoard->GetCurrentPlantCost(mDuplicatorSeedType, mDuplicatorImitaterType);
                    float aPercentDark = 0.0f;
                    if (mBoard->mCursorObject->mDuplicatorPlantID == (PlantID)mBoard->mPlants.DataArrayGetID(this) || mStateCountdown == 5000)
                    {
                        aPercentDark = 1.0f;
                    }
                    else
                    {
                        aPercentDark = mStateCountdown / 5000.0f;
                    }
                    int aGrayness = 255;
                    if (mApp->mGameScene != GameScenes::SCENE_PLAYING)
                    {
                        aGrayness = mBoard->mSeedBank->mCutSceneDarken;
                        aPercentDark = 0.0f;
                    }
                    else if (mApp->mEasyPlantingCheat)
                    {
                        aPercentDark = 0.0f;
                    }
                    else if (!mBoard->CanTakeSunMoney(aCost) || aPercentDark > 1.0f || !mBoard->PlantingRequirementsMet(mDuplicatorSeedType))
                    {
                        aGrayness = 128;
                    }

                    DrawSeedPacket(g, 15.0f, PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow), mDuplicatorSeedType, mDuplicatorImitaterType, aPercentDark, aGrayness, true, true);
                }
#endif
            }
        }

        if (mSeedType == SeedType::SEED_MAGNETSHROOM && !DrawMagnetItemsOnTop())
        {
            DrawMagnetItems(g);
        }
    }

    if (IsInPlay() && !mSquished && mApp->mShowHealthBar && !mApp->IsWallnutBowlingLevel() && mPlantHealth != mPlantMaxHealth /* &&
        (mSeedType == SeedType::SEED_WALLNUT || mSeedType == SeedType::SEED_TALLNUT || mSeedType == SeedType::SEED_PUMPKINSHELL || 
            mSeedType == SeedType::SEED_GARLIC)*/) {
        const int HEALTH_Y = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow) + 10;
        g->SetColor(Color(75, 75, 255));
        g->FillRect(12, HEALTH_Y, 50, 5);

        g->SetColor(Color(75, 255, 255));
        float HPpercent = (mPlantHealth) / (float)(mPlantMaxHealth);
        g->FillRect(12, HEALTH_Y, (int)(50 * HPpercent), 5);

        g->SetColor(Color::Black);
        g->DrawRect(12, HEALTH_Y, 50, 5);
    }
}

//0x4660B0
void Plant::DrawSeedType(Graphics* g, SeedType theSeedType, SeedType theImitaterType, DrawVariation theDrawVariation, float thePosX, float thePosY, unsigned int theBitVariation)
{
    g->PushState();
    int aCelRow = 0;
    int aCelCol = 2;
    float aOffsetX = 0.0f;
    float aOffsetY = 0.0f;
    SeedType aSeedType = theSeedType;
    DrawVariation aDrawVariation = theDrawVariation;
    DrawFilterVariation aFilterVaration = DrawFilterVariation::FILTERVARIATION_NONE;

    if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
    {
        aSeedType = theImitaterType;
        //aDrawVariation = DrawVariation::VARIATION_IMITATER;
        aFilterVaration = FILTERVARIATION_WASHED_OUT;
        if (theImitaterType == SeedType::SEED_HYPNOSHROOM || theImitaterType == SeedType::SEED_SQUASH || theImitaterType == SeedType::SEED_POTATOMINE ||
            theImitaterType == SeedType::SEED_GARLIC || theImitaterType == SeedType::SEED_LILYPAD)
        {
            aFilterVaration = FILTERVARIATION_LESS_WASHED_OUT;
        }
    }
    

    if (!((LawnApp*)gSexyAppBase)->GetDialog(Dialogs::DIALOG_ALMANAC) &&
        ((LawnApp*)gSexyAppBase)->mGameMode == GameMode::GAMEMODE_CHALLENGE_BIG_TIME &&
        (aSeedType == SeedType::SEED_WALLNUT || aSeedType == SeedType::SEED_SUNFLOWER || aSeedType == SeedType::SEED_MARIGOLD))
    {
        g->mScaleX *= 1.5f;
        g->mScaleY *= 1.5f;
        aOffsetX = -20.0f;
        aOffsetY = -40.0f;
        g->SetLinearBlend(true);

        {
            Reanimation aReanim;
            aReanim.ReanimationInitializeType(thePosX + aOffsetX, thePosY + aOffsetY, 
                theSeedType == SeedType::SEED_SUNFLOWER || theImitaterType == SeedType::SEED_SUNFLOWER ? ReanimationType::REANIM_SUNFLOWER :
                theSeedType == SeedType::SEED_WALLNUT || theImitaterType == SeedType::SEED_WALLNUT ? ReanimationType::REANIM_WALLNUT :
                 ReanimationType::REANIM_MARIGOLD);
            aReanim.SetFramesForLayer("anim_idle");
           
            if (theSeedType == SeedType::SEED_SUNFLOWER || theImitaterType == SeedType::SEED_SUNFLOWER)
            {
                aReanim.mAnimTime = 0.15f;
            }

            /*if (g->GetColorizeImages())
            {
                aReanim.mColorOverride = g->GetColor();
            }*/
            aReanim.OverrideScale(g->mScaleX, g->mScaleY);

            if (aFilterVaration != DrawFilterVariation::FILTERVARIATION_NONE)
            {
                switch (aFilterVaration)
                {
                case DrawFilterVariation::FILTERVARIATION_WASHED_OUT:
                    aReanim.mFilterEffect = FilterEffect::FILTER_EFFECT_WASHED_OUT;
                    break;
                case DrawFilterVariation::FILTERVARIATION_LESS_WASHED_OUT:
                    aReanim.mFilterEffect = FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT;
                    break;
                case DrawFilterVariation::FILTERVARIATION_WHITE:
                    aReanim.mFilterEffect = FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT;
                    break;
                }
            }

            aReanim.Draw(g);           
        }
        g->PopState();
        return;
    }

    if (aSeedType == SeedType::SEED_LEFTPEATER)
    {
        aOffsetX += g->mScaleX * 80.0f;
        g->mScaleX *= -1.0f;
    }
    /*if (aSeedType == SeedType::SEED_KERNELPULT)
    {
        aDrawVariation = DrawVariation::VARIATION_KERNELPULT_WITH_BUTTER;
    }*/
    if (aSeedType == SeedType::SEED_SEASHROOM)
    {
        aOffsetY += 3.535f;
    }
    if (aSeedType == SeedType::SEED_SUNSHROOM)
    {
        aDrawVariation = DrawVariation::VARIATION_SUNSHROOM_BIG;
    }
    if (aSeedType == SeedType::SEED_TANGLEKELP || aSeedType == SeedType::SEED_SEASHROOM)
    {
        aDrawVariation = DrawVariation::VARIATION_AQUARIUM;
    }

    g->PushState();
    //g->SetLinearBlend(true);

    if (Challenge::IsZombieSeedType(aSeedType))
    {
        ZombieType aZombieType = Challenge::IZombieSeedTypeToZombieType(aSeedType);
        if (aZombieType == ZombieType::ZOMBIE_POLEVAULTER)
        {
            aZombieType = ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE;
        }

        SDL_DisplayID display = SDL_GetPrimaryDisplay();
        float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;

        g->mScaleX /= scale;
        g->mScaleY /= scale;

        gLawnApp->mReanimatorCache->DrawCachedZombie(g, thePosX + aOffsetX, thePosY + aOffsetY, aZombieType);
    }
    else
    {
        const PlantDefinition& aPlantDef = GetPlantDefinition(aSeedType);

        if (aSeedType == SeedType::SEED_GIANT_WALLNUT)
        {
            SDL_DisplayID display = SDL_GetPrimaryDisplay();
            float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;
            g->mScaleX *= 1.4f / scale;
            g->mScaleY *= 1.4f / scale;
            TodDrawImageScaledF(g, IMAGE_REANIM_WALLNUT_BODY, thePosX - 53.0f, thePosY - 56.0f, g->mScaleX, g->mScaleY);
        }
        else if (aPlantDef.mReanimationType != ReanimationType::REANIM_NONE)
        {
            SDL_DisplayID display = SDL_GetPrimaryDisplay();
            float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;
            g->mScaleX /= scale;
            g->mScaleY /= scale;

            int offsetX, offsetY, aWidth, aHeight;
            gLawnApp->mReanimatorCache->GetPlantImageSize(theSeedType, offsetX, offsetY, aWidth, aHeight);

            aOffsetX += ((offsetX * (scale - 1)) / 2.0f) * g->mScaleX * scale;
            aOffsetY += ((offsetY * (scale - 1)) / 2.0f) * g->mScaleY * scale;

            gLawnApp->mReanimatorCache->DrawCachedPlant(g, thePosX + aOffsetX, thePosY + aOffsetY, aSeedType, aDrawVariation, aFilterVaration, theBitVariation);
        }
        else
        {
            if (aSeedType == SeedType::SEED_KERNELPULT)
            {
                aCelRow = 2;
            }
            else if (aSeedType == SeedType::SEED_TWINSUNFLOWER)
            {
                aCelRow = 1;
            }

            Image* aPlantImage = Plant::GetImage(aSeedType);
            if (aPlantImage->mNumCols <= 2)
            {
                aCelCol = aPlantImage->mNumCols - 1;
            }

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
            if (aSeedType == SeedType::SEED_DUPLICATORCYCLER) {
                aCelCol = 9;
            }
#endif
            TodDrawImageCelScaledF(g, aPlantImage, thePosX + aOffsetX, thePosY + aOffsetY, aCelCol, aCelRow, g->mScaleX, g->mScaleY);
        }
    }
    g->PopState();
    g->PopState();
}

//0x466390
void Plant::MouseDown(int x, int y, int theClickCount)
{
    if (theClickCount < 0 || mBurnedCounter != -1)
        return;

    if (mState == PlantState::STATE_COBCANNON_READY)
    {
        mBoard->ClearCursor();
        mBoard->mCursorObject->mType = SeedType::SEED_NONE;
        mBoard->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_COBCANNON_TARGET;
        mBoard->mCursorObject->mSeedBankIndex = -1;
        mBoard->mCursorObject->mCoinID = CoinID::COINID_NULL;
        mBoard->mCursorObject->mCobCannonPlantID = (PlantID)mBoard->mPlants.DataArrayGetID(this);
        mBoard->mCobCannonCursorDelayCounter = 30;
        mBoard->mCobCannonMouseX = x;
        mBoard->mCobCannonMouseY = y;
    }

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    if (mSeedType == SeedType::SEED_DUPLICATORCYCLER && mDuplicatorSeedType != SeedType::SEED_NONE) {
        int aCost = mBoard->GetCurrentPlantCost(mDuplicatorSeedType, mDuplicatorImitaterType);
        int isReady = mStateCountdown == 0;
        int hasEnoughSuns = mBoard->CanTakeSunMoney(aCost);
        int requirementsMet = mBoard->PlantingRequirementsMet(mDuplicatorSeedType);
    
        if (isReady && hasEnoughSuns && requirementsMet)
        {
            mBoard->ClearCursor();
            mBoard->mCursorObject->mType = mDuplicatorSeedType;
            mBoard->mCursorObject->mImitaterType = mDuplicatorImitaterType;
            mBoard->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_PLANT_FROM_DUPLICATOR;
            mBoard->mCursorObject->mSeedBankIndex = -1;
            mBoard->mCursorObject->mCoinID = CoinID::COINID_NULL;
            mBoard->mCursorObject->mDuplicatorPlantID = (PlantID)mBoard->mPlants.DataArrayGetID(this);
        }
        else if (!isReady) {
            mBoard->mToolTip->SetWarningText(_S("[WAITING_FOR_SEED]"));
        }
        else if (!hasEnoughSuns && !mBoard->HasConveyorBeltSeedBank() && !mApp->IsSlotMachineLevel()) {
            mBoard->mToolTip->SetWarningText(_S("[NOT_ENOUGH_SUN]"));
            mApp->PlaySample(Sexy::SOUND_BUZZER);
            mBoard->mOutOfMoneyCounter = 70;
        }
        else if (!requirementsMet) {
            if (mDuplicatorSeedType == SeedType::SEED_GATLINGPEA)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_REPEATER]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_WINTERMELON)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_MELONPULT]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_TWINSUNFLOWER)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_SUNFLOWER]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_SPIKEROCK)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_SPIKEWEED]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_COBCANNON)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_KERNELPULTS]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_GOLD_MAGNET)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_MAGNETSHROOM]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_GLOOMSHROOM)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_FUMESHROOM]"));
            }
            else if (mDuplicatorSeedType == SeedType::SEED_CATTAIL)
            {
                mBoard->mToolTip->SetWarningText(_S("[REQUIRES_LILY_PAD]"));
            }
        }
    }
#endif
}

//0x466420
void Plant::IceZombies()
{
    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        aZombie->HitIceTrap();
    }

    mBoard->mIceTrapCounter = 300;
    TodParticleSystem* aPoolSparklyParticle = mApp->ParticleTryToGet(mBoard->mPoolSparklyParticleID);
    if (aPoolSparklyParticle)
    {
        aPoolSparklyParticle->mDontUpdate = false;
    }

    Zombie* aBossZombie = nullptr;
    while (mBoard->IterateZombies(aBossZombie))
    {
        if (aBossZombie->mZombieType == ZOMBIE_BOSS)
        {
            aBossZombie->BossDestroyFireball();
        }
    }
}

//0x4664B0
void Plant::BurnRow(int theRow)
{
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        if ((aZombie->mZombieType == ZombieType::ZOMBIE_BOSS || aZombie->mRow == theRow) && aZombie->EffectedByDamage(aDamageRangeFlags))
        {
            aZombie->RemoveColdEffects();
            aZombie->ApplyBurn();
        }
    }

    GridItem* aGridItem = nullptr;
    while (mBoard->IterateGridItems(aGridItem))
    {
        if (aGridItem->mGridY == theRow && aGridItem->mGridItemType == GridItemType::GRIDITEM_LADDER)
        {
            aGridItem->GridItemDie();
        }
    }

    Zombie* aBossZombie = nullptr;
    while (mBoard->IterateZombies(aBossZombie))
    {
        if (aBossZombie->mZombieType == ZOMBIE_BOSS && aBossZombie->mFireballRow == theRow)
        {
            // 注：原版中将 Zombie::BossDestroyIceballInRow(int) 函数改为了 Zombie::BossDestroyIceball()，冰球是否位于目标行的判断则移动至此处进行
            aBossZombie->BossDestroyIceballInRow(theRow);
        }
    }
}

//0x4665B0
void Plant::BlowAwayFliers(int theX, int theRow)
{
    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        if (!aZombie->IsDeadOrDying())
        {
            Rect aZombieRect = aZombie->GetZombieRect();
            if (aZombie->IsFlying())
            {
                aZombie->mBlowingAway = true;
            }
        }
    }

    mApp->PlaySample(SOUND_BLOVER);
    mBoard->mFogBlownCountDown = 4000;
}

//0x466650
void Plant::KillAllPlantsNearDoom()
{
    Plant* aPlant = nullptr;
    while (mBoard->IteratePlants(aPlant))
    {
        if (aPlant->mRow == mRow && aPlant->mPlantCol == mPlantCol)
        {
            aPlant->Die();
        }
    }
}

//0x4666A0
void Plant::DoSpecial()
{
    int aPosX = mX + mWidth / 2;
    int aPosY = mY + mHeight / 2;
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);

    switch (mSeedType)
    {
    case SeedType::SEED_BLOVER:
    {
        if (mState != PlantState::STATE_DOINGSPECIAL)
        {
            mState = PlantState::STATE_DOINGSPECIAL;
            BlowAwayFliers(mX, mRow);
        }
        break;
    }
    case SeedType::SEED_CHERRYBOMB:
    {
        mApp->PlayFoley(FoleyType::FOLEY_CHERRYBOMB);
        mApp->PlayFoley(FoleyType::FOLEY_JUICY);

        if (mBoard->KillAllZombiesInRadius(mRow, aPosX, aPosY, 115, 1, true, aDamageRangeFlags) >= 10)
            if (!mApp->IsIZombieLevel())
                ReportAchievement::GiveAchievement(mApp, AchievementId::Explodonator, true); // @Patoke: add achievement

        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_POWIE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mBoard->ShakeBoard(3, -4);

        Die();
        break;
    }
    case SeedType::SEED_DOOMSHROOM:
    {
        mApp->PlaySample(SOUND_DOOMSHROOM);

        mBoard->KillAllZombiesInRadius(mRow, aPosX, aPosY, 250, 3, true, aDamageRangeFlags);
        KillAllPlantsNearDoom();

        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_DOOM);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mBoard->AddACrater(mPlantCol, mRow)->mGridItemCounter = 18000;
        mBoard->ShakeBoard(3, -4);

        Die();
        break;
    }
    case SeedType::SEED_JALAPENO:
    {
        mApp->PlayFoley(FoleyType::FOLEY_JALAPENO_IGNITE);
        mApp->PlayFoley(FoleyType::FOLEY_JUICY);

        mBoard->DoFwoosh(mRow);
        for (int i = 0; i < 12; i++) {
            Reanimation* aOriReanim = mApp->ReanimationTryToGet(mBoard->mFwooshID[mRow][i]);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aOriReanim && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
            {
                aOriReanim->mFilterEffect = aBodyReanim->mFilterEffect;
            }
        }
        mBoard->ShakeBoard(3, -4);

        BurnRow(mRow);
        mBoard->mIceTimer[mRow] = min(mBoard->mIceTimer[mRow], 20);

        Die();
        break;
    }
    case SeedType::SEED_UMBRELLA:
    {
        if (mState != PlantState::STATE_UMBRELLA_TRIGGERED && mState != PlantState::STATE_UMBRELLA_REFLECTING)
        {
            mState = PlantState::STATE_UMBRELLA_TRIGGERED;
            mStateCountdown = 5;

            const PlantDefinition& aPlantDef = GetPlantDefinition(mSeedType);

            Reanimation* aSpecialReanim = mApp->ReanimationTryToGet(mSpecialReanimID);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);

            if (!aSpecialReanim)
            {
                float aOffsetY = PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);

                aSpecialReanim = mApp->AddReanimation(mX, mY + aOffsetY, Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PROJECTILE, mRow + 1, 0), aPlantDef.mReanimationType);
                aSpecialReanim->AssignRenderGroupToTrack("anim_face", RENDER_GROUP_HIDDEN);
                aSpecialReanim->PlayReanim("anim_block", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 22.0f);
                mSpecialReanimID = mApp->ReanimationGetID(aSpecialReanim);

                if (!FloatApproxEqual(mRad, 0.0f))
                {
                    float aOffsetX = -sin(mRad) * aOffsetY;
                    TodScaleRotateTransformMatrix(aSpecialReanim->mOverlayMatrix, mX + aBodyReanim->mOverlayMatrix.m02 + aOffsetX, mY + aOffsetX + aBodyReanim->mOverlayMatrix.m12, mRad, 1.0f, 1.0f);
                }

                if (aBodyReanim)
                {
                    aSpecialReanim->mFilterEffect = aBodyReanim->mFilterEffect;
                    aBodyReanim->ShowOnlyTrack("anim_face");
                }
            }
            else
            {
                aSpecialReanim->StartBlend(20);
                aSpecialReanim->SetFramesForLayer("anim_block");
                aSpecialReanim->mAnimTime = 0.0f;
                aSpecialReanim->mLoopCount = 0;
                aSpecialReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD;
                aSpecialReanim->mAnimRate = 22.0f;
                aSpecialReanim->mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PROJECTILE, mRow + 1, 0);
            }  
            
            //PlayBodyReanim("anim_block", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 22.0f);
        }

        break;
    }
    case SeedType::SEED_ICESHROOM:
    {
        mApp->PlayFoley(FoleyType::FOLEY_FROZEN);
        IceZombies();
        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_ICE_TRAP);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }

        Die();
        break;
    }
    case SeedType::SEED_POTATOMINE:
    {
        aPosX = mX + mWidth / 2 - 20;
        aPosY = mY + mHeight / 2;

        mApp->PlaySample(SOUND_POTATO_MINE);
        if (mBoard->GetKilledlZombiesInRadius(mRow, aPosX, aPosY, 60, 0, false, aDamageRangeFlags) >= 1) 
            if (!mApp->IsIZombieLevel())
                ReportAchievement::GiveAchievement(mApp, AchievementId::Spudow, true); // @Patoke: add achievement

        int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PARTICLE, mRow, 0);
        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX + 20.0f, aPosY, aRenderPosition, ParticleEffect::PARTICLE_POTATO_MINE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mBoard->ShakeBoard(3, -4);

        Die();
        break;
    }
    case SeedType::SEED_INSTANT_COFFEE:
    {
        Plant* aPlant = mBoard->GetTopPlantAt(mPlantCol, mRow, PlantPriority::TOPPLANT_ONLY_NORMAL_POSITION);
        if (aPlant && aPlant->mIsAsleep)
        {
            aPlant->mWakeUpCounter = 100;
        }

        mState = PlantState::STATE_DOINGSPECIAL;
        PlayBodyReanim("anim_crumble", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 22.0f);
        mApp->PlayFoley(FoleyType::FOLEY_COFFEE);

        break;
    }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_TIMESTOPPER:
    {
        mDoSpecialCountdown = -1;
        mBoard->mTimeStopCounter = 300;
        mApp->PlayFoley(FoleyType::FOLEY_EXPLOSION);
        break;
    }
    case SeedType::SEED_CHERRYHOVERBOMB:
    {
        mApp->PlayFoley(FoleyType::FOLEY_CHERRYBOMB);
        mApp->PlayFoley(FoleyType::FOLEY_JUICY);

        if (mBoard->KillAllZombiesInRadius(mRow, aPosX, aPosY, 115, 1, true, aDamageRangeFlags) >= 10)
            if (!mApp->IsIZombieLevel())
                ReportAchievement::GiveAchievement(mApp, Explodonator, true); // @Patoke: add achievement

        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_POWIE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mBoard->ShakeBoard(3, -4);

        Die();
        break;
    }
    case SeedType::SEED_DUPLICATORCYCLER:
    {
        mDoSpecialCountdown = 300;
        if (IsInPlay())
        {
            if (mBoard->mSeedBank->mNumPackets > 0)
            {
                int targetIndex = Rand(mBoard->mSeedBank->mNumPackets);
                mDuplicatorSeedType = mBoard->mSeedBank->mSeedPackets[targetIndex].mPacketType;
                mDuplicatorImitaterType = mBoard->mSeedBank->mSeedPackets[targetIndex].mImitaterType;
            }
        }
        break;
    }
    case SeedType::SEED_NUKECUMBER:
    {
        mApp->PlaySample(SOUND_DOOMSHROOM);

        mBoard->NukeBoard();
        KillAllPlantsNearDoom();

        mBoard->AddACrater(mPlantCol, mRow)->mGridItemCounter = 18000;
        mBoard->ShakeBoard(3, -4);

        Die();
        break;
    }
#endif
    case SeedType::SEED_EXPLODE_O_NUT:
    {
        mApp->PlayFoley(FoleyType::FOLEY_CHERRYBOMB);

        int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY) | 32U;
        mBoard->KillAllZombiesInRadius(mRow, aPosX, aPosY, 90, 1, true, aDamageRangeFlags);
        TodParticleSystem* aParticle = mApp->AddTodParticle(aPosX, aPosY, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_POWIE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
        mBoard->ShakeBoard(3, -4);
        Die();
        break;
    }
    }
}

//0x466B80
void Plant::ImitaterMorph()
{
    Die();
    Plant* aPlant = mBoard->AddPlant(mPlantCol, mRow, mImitaterType, SeedType::SEED_IMITATER);
    aPlant->mRenderOrder = aPlant->CalcRenderOrder();

    if (mImitaterType == SeedType::SEED_FLOWERPOT)
    {
        Plant* aTopPlant = mBoard->GetTopPlantAt(mPlantCol, mRow, PlantPriority::TOPPLANT_BUNGEE_ORDER);
        if (aTopPlant->mSeedType != SeedType::SEED_FLOWERPOT)
        {
            Plant* aFlowerPot = mBoard->GetFlowerPotAt(mPlantCol, mRow);
            if (aTopPlant)
                mApp->ReanimationGet(aFlowerPot->mBodyReanimID)->mAnimRate = 0.0f;
        }
    }

    FilterEffect aFilter = FilterEffect::FILTER_EFFECT_WASHED_OUT;
    if (mImitaterType == SeedType::SEED_HYPNOSHROOM || mImitaterType == SeedType::SEED_SQUASH || mImitaterType == SeedType::SEED_POTATOMINE ||
        mImitaterType == SeedType::SEED_GARLIC || mImitaterType == SeedType::SEED_LILYPAD)
        aFilter = FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT;

    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(aPlant->mBodyReanimID);
    if (aBodyReanim)
    {
        aBodyReanim->mFilterEffect = aFilter;
    }
    Reanimation* aHeadReanim = mApp->ReanimationTryToGet(aPlant->mHeadReanimID);
    if (aHeadReanim)
    {
        aHeadReanim->mFilterEffect = aFilter;
    }Reanimation* aHeadReanim2 = mApp->ReanimationTryToGet(aPlant->mHeadReanimID2);
    if (aHeadReanim2)
    {
        aHeadReanim2->mFilterEffect = aFilter;
    }Reanimation* aHeadReanim3 = mApp->ReanimationTryToGet(aPlant->mHeadReanimID3);
    if (aHeadReanim3)
    {
        aHeadReanim3->mFilterEffect = aFilter;
    }

    if (mImitaterType == SeedType::SEED_PLANTERN)
    {
        if (!IsOnBoard() || mApp->mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
        {
            aPlant->AddAttachedParticle(mX + 40, mY + 40, (int)RenderLayer::RENDER_LAYER_FOG + 1, ParticleEffect::PARTICLE_LANTERN_SHINE);
        }
    }

}

//0x466CA0
void Plant::UpdateImitater()
{
    if (mState != PlantState::STATE_IMITATER_MORPHING)
    {
        if (mStateCountdown == 0)
        {
            mState = PlantState::STATE_IMITATER_MORPHING;
            PlayBodyReanim("anim_explode", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 26.0f);
        }
    }
    else
    {
        Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
        if (aBodyReanim->ShouldTriggerTimedEvent(0.8f))
        {
            // Why? Incase someone like Exter do the Imitated Imitater Exploit lol - Inliothixie
            TodParticleSystem* aParticle = mApp->AddTodParticle(mX + 40, mY + 40, (int)RenderLayer::RENDER_LAYER_TOP, ParticleEffect::PARTICLE_IMITATER_MORPH);
            Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
            if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
            {
                aParticle->OverrideFilterEffect(nullptr, FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT);
            }
        }
        if (aBodyReanim->mLoopCount > 0)
        {
            ImitaterMorph();
        }
    }
}

//0x466D50
void Plant::CobCannonFire(int theTargetX, int theTargetY)
{
    TOD_ASSERT(mState == PlantState::STATE_COBCANNON_READY);

    mState = PlantState::STATE_COBCANNON_FIRING;
    mShootingCounter = 206;
    PlayBodyReanim("anim_shooting", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 12.0f);

    mTargetX = theTargetX - 47.0f;
    mTargetY = theTargetY;

    Reanimation* aBodyReanim = mApp->ReanimationGet(mBodyReanimID);
    ReanimatorTrackInstance* aTrackInstance = aBodyReanim->GetTrackInstanceByName("CobCannon_Cob");
    aTrackInstance->mTrackColor = Color::White;
}

//0x466E00
void Plant::Fire(Zombie* theTargetZombie, int theRow, PlantWeapon thePlantWeapon)
{
    if (mSeedType == SeedType::SEED_FUMESHROOM)
    {
        DoRowAreaDamage(20, 2U);
        mApp->PlayFoley(FoleyType::FOLEY_FUME);
        return;
    }
    if (mSeedType == SeedType::SEED_GLOOMSHROOM)
    {
        DoRowAreaDamage(20, 2U);
        return;
    }
    if (mSeedType == SeedType::SEED_STARFRUIT)
    {
        StarFruitFire();
        return;
    }

    ProjectileType aProjectileType;
    switch (mSeedType)
    {
    case SeedType::SEED_PEASHOOTER:
    case SeedType::SEED_REPEATER:
    case SeedType::SEED_THREEPEATER:
    case SeedType::SEED_SPLITPEA:
    case SeedType::SEED_GATLINGPEA:
    case SeedType::SEED_LEFTPEATER:
        aProjectileType = ProjectileType::PROJECTILE_PEA;
        break;
    case SeedType::SEED_SNOWPEA:
        aProjectileType = ProjectileType::PROJECTILE_SNOWPEA;
        break;
    case SeedType::SEED_PUFFSHROOM:
    case SeedType::SEED_SCAREDYSHROOM:
    case SeedType::SEED_SEASHROOM:
        aProjectileType = ProjectileType::PROJECTILE_PUFF;
        break;
    case SeedType::SEED_CACTUS:
#ifdef _PIERCING_CACTUS
        aProjectileType = ProjectileType::PROJECTILE_PIERCE_SPIKE;
        break;
#endif
    case SeedType::SEED_CATTAIL:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_STINGER:
#endif
        aProjectileType = ProjectileType::PROJECTILE_SPIKE;
        break;
    case SeedType::SEED_CABBAGEPULT:
        aProjectileType = ProjectileType::PROJECTILE_CABBAGE;
        break;
    case SeedType::SEED_KERNELPULT:
        aProjectileType = ProjectileType::PROJECTILE_KERNEL;
        break;
    case SeedType::SEED_MELONPULT:
        aProjectileType = ProjectileType::PROJECTILE_MELON;
        break;
    case SeedType::SEED_WINTERMELON:
        aProjectileType = ProjectileType::PROJECTILE_WINTERMELON;
        break;
    case SeedType::SEED_COBCANNON:
        aProjectileType = ProjectileType::PROJECTILE_COBBIG;
        break;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_ICEBERGLETTUCE:
        aProjectileType = ProjectileType::PROJECTILE_LETTUCE;
        break;
    case SeedType::SEED_BEEHIVE:
        aProjectileType = ProjectileType::PROJECTILE_BEE;
        break;
#endif
    default:
        TOD_ASSERT();
        break;
    }
    if (mSeedType == SeedType::SEED_KERNELPULT && thePlantWeapon == PlantWeapon::WEAPON_SECONDARY)
    {
        aProjectileType = ProjectileType::PROJECTILE_BUTTER;
    }

    mApp->PlayFoley(FoleyType::FOLEY_THROW);
    if (mSeedType == SeedType::SEED_SNOWPEA || mSeedType == SeedType::SEED_WINTERMELON)
    {
        mApp->PlayFoley(FoleyType::FOLEY_SNOW_PEA_SPARKLES);
    }
    else if (mSeedType == SeedType::SEED_PUFFSHROOM || mSeedType == SeedType::SEED_SCAREDYSHROOM || mSeedType == SeedType::SEED_SEASHROOM)
    {
        mApp->PlayFoley(FoleyType::FOLEY_PUFF);
    }

    int aOriginX, aOriginY;
    if (mSeedType == SeedType::SEED_PUFFSHROOM)
    {
        aOriginX = mX + 40;
        aOriginY = mY + 40;
    }
    else if (mSeedType == SeedType::SEED_SEASHROOM)
    {
        aOriginX = mX + 45;
        aOriginY = mY + 63;
    }
    else if (mSeedType == SeedType::SEED_CABBAGEPULT)
    {
        aOriginX = mX + 5;
        aOriginY = mY - 12;
    }
    else if (mSeedType == SeedType::SEED_MELONPULT || mSeedType == SeedType::SEED_WINTERMELON)
    {
        aOriginX = mX + 25;
        aOriginY = mY - 46;
    }
    else if (mSeedType == SeedType::SEED_CATTAIL)
    {
        aOriginX = mX + 20;
        aOriginY = mY - 3;
    }
    else if (mSeedType == SeedType::SEED_KERNELPULT && thePlantWeapon == PlantWeapon::WEAPON_PRIMARY)
    {
        aOriginX = mX + 19;
        aOriginY = mY - 37;
    }
    else if (mSeedType == SeedType::SEED_KERNELPULT && thePlantWeapon == PlantWeapon::WEAPON_SECONDARY)
    {
        aOriginX = mX + 12;
        aOriginY = mY - 56;
    }
    else if (mSeedType == SeedType::SEED_PEASHOOTER || mSeedType == SeedType::SEED_SNOWPEA || mSeedType == SeedType::SEED_REPEATER)
    {
        int aOffsetX, aOffsetY;
        GetPeaHeadOffset(aOffsetX, aOffsetY);
        aOriginX = mX + aOffsetX + 24;
        aOriginY = mY + aOffsetY - 33;
    }
    else if (mSeedType == SeedType::SEED_LEFTPEATER)
    {
        int aOffsetX, aOffsetY;
        GetPeaHeadOffset(aOffsetX, aOffsetY);
        aOriginX = mX + aOffsetX + 27;        
        aOriginY = mY + aOffsetY - 33;
    }
    else if (mSeedType == SeedType::SEED_GATLINGPEA)
    {
        int aOffsetX, aOffsetY;
        GetPeaHeadOffset(aOffsetX, aOffsetY);
        aOriginX = mX + aOffsetX + 34;
        aOriginY = mY + aOffsetY - 33;
    }
    else if (mSeedType == SeedType::SEED_SPLITPEA)
    {
        int aOffsetX, aOffsetY;
        GetPeaHeadOffset(aOffsetX, aOffsetY);
        aOriginY = mY + aOffsetY - 33;

        if (thePlantWeapon == PlantWeapon::WEAPON_SECONDARY)
        {
            aOriginX = mX + aOffsetX - 67; // -40
        }
        else
        {
            aOriginX = mX + aOffsetX + 24;
        }
    }
    else if (mSeedType == SeedType::SEED_THREEPEATER)
    {
        aOriginX = mX + 45;
        aOriginY = mY + 10;
    }
    else if (mSeedType == SeedType::SEED_SCAREDYSHROOM)
    {
        aOriginX = mX + 29;
        aOriginY = mY + 21;
    }
    else if (mSeedType == SeedType::SEED_CACTUS)
    {
        if (thePlantWeapon == PlantWeapon::WEAPON_PRIMARY)
        {
            aOriginX = mX + 93;
            aOriginY = mY - 50;
        }
        else
        {
            aOriginX = mX + 70;
            aOriginY = mY + 23;
        }
    }
    else if (mSeedType == SeedType::SEED_COBCANNON)
    {
        float baseOffsetX = -44;
        float baseOffsetY = -184;

        float offsetX = baseOffsetX;
        float offsetY = baseOffsetY;

        if (!FloatApproxEqual(mRad, 0.0f))
        {
            float cosA = cos(mRad);
            float sinA = sin(mRad);

            offsetX = baseOffsetX * cosA + baseOffsetY * sinA;
            offsetY = baseOffsetX * sinA + baseOffsetY * cosA;

            float rotatedHeightX = sin(mRad) * PlantDrawHeightOffset(mBoard, this, mSeedType, mPlantCol, mRow);
            offsetX += -cos(mRad) * 7.5f + rotatedHeightX;
            offsetY += sin(mRad) * 20;
        }

        aOriginX = mX + offsetX;
        aOriginY = mY + offsetY;
    }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    else if (mSeedType == SeedType::SEED_STINGER)
    {
        aOriginX = mX + 10;
        aOriginY = mY + 5 - 60;
    }
#endif
    else
    {
        aOriginX = mX + 10;
        aOriginY = mY + 5;
    }
    if (mBoard->GetFlowerPotAt(mPlantCol, mRow))
    {
        aOriginY -= 5;
    }
    
    if (mSeedType == SeedType::SEED_SNOWPEA)
    {
        int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_LAWN_MOWER, mRow, 1);
        TodParticleSystem* aParticle = mApp->AddTodParticle(aOriginX + 8, aOriginY + 13, aRenderPosition, ParticleEffect::PARTICLE_SNOWPEA_PUFF);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
    }
    else if (mSeedType == SeedType::SEED_PUFFSHROOM)
    {
        int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_LAWN_MOWER, mRow, 1);
        TodParticleSystem* aParticle = mApp->AddTodParticle(aOriginX + 18, aOriginY + 13, aRenderPosition, ParticleEffect::PARTICLE_PUFFSHROOM_MUZZLE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
    }
    else if (mSeedType == SeedType::SEED_SCAREDYSHROOM)
    {
        int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_LAWN_MOWER, mRow, 1);
        TodParticleSystem* aParticle = mApp->AddTodParticle(aOriginX + 27, aOriginY + 13, aRenderPosition, ParticleEffect::PARTICLE_PUFFSHROOM_MUZZLE);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aParticle && aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aParticle->OverrideFilterEffect(nullptr, aBodyReanim->mFilterEffect);
        }
    }

    Projectile* aProjectile = mBoard->AddProjectile(aOriginX, aOriginY, mRenderOrder - 1, theRow, aProjectileType);
    aProjectile->mDamageRangeFlags = GetDamageRangeFlags(thePlantWeapon);

    if (aProjectileType == ProjectileType::PROJECTILE_COBBIG && !FloatApproxEqual(mRad, 0.0f))
    {
        aProjectile->OverrideAngle(mRad);
    }

    {
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aBodyReanim && aBodyReanim->mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
        {
            aProjectile->mFilterEffect = aBodyReanim->mFilterEffect;
        }
    }

    if (mSeedType == SeedType::SEED_CABBAGEPULT || mSeedType == SeedType::SEED_KERNELPULT ||
        mSeedType == SeedType::SEED_MELONPULT || mSeedType == SeedType::SEED_WINTERMELON
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || mSeedType == SeedType::SEED_ICEBERGLETTUCE
#endif
        )
    {
        float aRangeX, aRangeY;
        if (theTargetZombie)
        {
            Rect aZombieRect = theTargetZombie->GetZombieRect();
            aRangeX = theTargetZombie->ZombieTargetLeadX(50.0f) - aOriginX - 30.0f;
            aRangeY = aZombieRect.mY - aOriginY;

            if (theTargetZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_RIDING)
            {
                aRangeX -= 60.0f;
            }
            if (theTargetZombie->mZombieType == ZombieType::ZOMBIE_POGO && theTargetZombie->mHasObject)
            {
                aRangeX -= 60.0f;
            }
            if (theTargetZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_WALKING_IN_POOL)
            {
                aRangeX -= 40.0f;
            }
            if (theTargetZombie->mZombieType == ZombieType::ZOMBIE_BOSS)
            {
                aRangeY = mBoard->GridToPixelY(8, mRow) - aOriginY;
            }
        }
        else
        {
            aRangeX = 700.0f - aOriginX;
            aRangeY = 0.0f;
        }
        if (aRangeX < 40.0f)
        {
            aRangeX = 40.0f;
        }

        aProjectile->mMotionType = ProjectileMotion::MOTION_LOBBED;
        aProjectile->mVelX = aRangeX / 120.0f;
        aProjectile->mVelY = 0.0f;
        aProjectile->mVelZ = aRangeY / 120.0f - 7.0f;
        aProjectile->mAccZ = 0.115f;
    }
    else if (mSeedType == SeedType::SEED_THREEPEATER)
    {
        if (theRow < mRow)
        {
            aProjectile->mMotionType = ProjectileMotion::MOTION_THREEPEATER;
            aProjectile->mVelY = -3.0f;
            aProjectile->mShadowY += 80.0f;
        }
        else if (theRow > mRow)
        {
            aProjectile->mMotionType = ProjectileMotion::MOTION_THREEPEATER;
            aProjectile->mVelY = 3.0f;
            aProjectile->mShadowY -= 80.0f;
        }
    }
    else if (mSeedType == SeedType::SEED_PUFFSHROOM || mSeedType == SeedType::SEED_SEASHROOM)
    {
        aProjectile->mMotionType = ProjectileMotion::MOTION_PUFF;
    }
    else if (mSeedType == SeedType::SEED_SPLITPEA && thePlantWeapon == PlantWeapon::WEAPON_SECONDARY)
    {
        aProjectile->mMotionType = ProjectileMotion::MOTION_BACKWARDS;
    }
    else if (mSeedType == SeedType::SEED_LEFTPEATER)
    {
        aProjectile->mMotionType = ProjectileMotion::MOTION_BACKWARDS;
    }
    else if (mSeedType == SeedType::SEED_CATTAIL)
    {
        aProjectile->mVelX = 2.0f;
        aProjectile->mMotionType = ProjectileMotion::MOTION_HOMING;
        aProjectile->mTargetZombieID = mBoard->ZombieGetID(theTargetZombie);
    }
    else if (mSeedType == SeedType::SEED_COBCANNON)
    {
        aProjectile->mVelX = 0.001f;
        aProjectile->mDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
        aProjectile->mMotionType = ProjectileMotion::MOTION_LOBBED;
        aProjectile->mVelY = 0.0f;
        aProjectile->mAccZ = 0.0f;
        aProjectile->mVelZ = -8.0f;
        aProjectile->mCobTargetX = mTargetX - 40;
        aProjectile->mCobTargetRow = mBoard->PixelToGridYKeepOnBoard(mTargetX, mTargetY);

        if (!FloatApproxEqual(mRad, 0.0f))
        {
            float adjustedAngle = mRad - (PI / 2.0f);
            float speed = aProjectile->mVelZ;

            aProjectile->mVelX = speed * cos(adjustedAngle);
            aProjectile->mVelZ = -speed * sin(adjustedAngle);
        }
    }
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    else if (mSeedType == SeedType::SEED_BEEHIVE)
    {
        aProjectile->mMotionType = ProjectileMotion::MOTION_BEE;
        aProjectile->mVelX = 2.0f;
        aProjectile->mTargetZombieID = mBoard->ZombieGetID(theTargetZombie);
    }
#endif
}

//0x4675C0
Zombie* Plant::FindTargetZombie(int theRow, PlantWeapon thePlantWeapon)
{
    int aDamageRangeFlags = GetDamageRangeFlags(thePlantWeapon);
    Rect aAttackRect = GetPlantAttackRect(thePlantWeapon);
    int aHighestWeight = 0;
    Zombie* aBestZombie = nullptr;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        int aRowDeviation = aZombie->mRow - theRow;
        if (aZombie->mZombieType == ZombieType::ZOMBIE_BOSS)
        {
            aRowDeviation = 0;
        }

        if (!aZombie->mHasHead || aZombie->IsTangleKelpTarget())
        {
            if (mSeedType == SeedType::SEED_POTATOMINE || mSeedType == SeedType::SEED_CHOMPER || mSeedType == SeedType::SEED_TANGLEKELP)
            {
                continue;
            }
        }

        bool needPortalCheck = false;
        if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_PORTAL_COMBAT)
        {
            if (mSeedType == SeedType::SEED_PEASHOOTER || mSeedType == SeedType::SEED_CACTUS || mSeedType == SeedType::SEED_REPEATER)
            {
                needPortalCheck = true;
            }
        }

        if (mSeedType != SeedType::SEED_CATTAIL 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
            && mSeedType != SeedType::SEED_BEEHIVE
#endif
            )
        {
            if (mSeedType == SeedType::SEED_GLOOMSHROOM)
            {
                if (aRowDeviation < -1 || aRowDeviation > 1)
                {
                    continue;
                }
            }
            else if (needPortalCheck)
            {
                if (!mBoard->mChallenge->CanTargetZombieWithPortals(this, aZombie))
                {
                    continue;
                }
            }
            else if (aRowDeviation)
            {
                continue;
            }
        }

        if (aZombie->mZombiePhase == ZombiePhase::PHASE_ZAMBONI_FLAT && IsSpiky())
        {
            continue;
        }

        if (aZombie->EffectedByDamage(aDamageRangeFlags) 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
            || aZombie->mZombieType == ZombieType::ZOMBIE_DOG  && (mSeedType == SeedType::SEED_YAMPOLINE || IsSpiky()) && aZombie->EffectedByDamage(13) ||
            aZombie->mZombiePhase == ZombiePhase::PHASE_PROPELLER_FLYING && mSeedType == SeedType::SEED_CACTUS && thePlantWeapon == PlantWeapon::WEAPON_PRIMARY
#endif 
           )
        {
            int aExtraRange = 0;

            if (mSeedType == SeedType::SEED_CHOMPER)
            {
                if (aZombie->mZombiePhase == ZombiePhase::PHASE_DIGGER_WALKING)
                {
                    aAttackRect.mX += 20;
                    aAttackRect.mWidth -= 20;
                }

                if (aZombie->mZombiePhase == ZombiePhase::PHASE_POGO_BOUNCING || (aZombie->mZombieType == ZombieType::ZOMBIE_BUNGEE && aZombie->mTargetCol == mPlantCol))
                {
                    continue;
                }

                if (aZombie->mIsEating || mState == PlantState::STATE_CHOMPER_BITING)
                {
                    aExtraRange = 60;
                }
            }

            if (mSeedType == SeedType::SEED_POTATOMINE)
            {
                if (mBoard->GetFlowerPotAt(mPlantCol, mRow) && aZombie->mZombiePhase == PHASE_DIGGER_TUNNELING)
                    continue;

                if ((aZombie->mZombieType == ZombieType::ZOMBIE_POGO && aZombie->mHasObject) ||
                    aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_IN_VAULT || aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_PRE_VAULT)
                {
                    continue;
                }

                if (aZombie->mZombieType != ZombieType::ZOMBIE_POLEVAULTER && !aZombie->IsWalkingBackwards())
                {
                    aAttackRect.mX -= 40;
                    aAttackRect.mWidth += 40;  // 原版经典土豆地雷 Bug 及“四撑杆引雷”的原理
                }
                else {
                    aAttackRect.mX += 40;
                    aAttackRect.mWidth -= 40;
                }

                if (aZombie->mZombieType == ZombieType::ZOMBIE_BUNGEE && aZombie->mTargetCol != mPlantCol)
                {
                    continue;
                }

                if (aZombie->mIsEating)
                {
                    aExtraRange = 30;
                }
            }

            if ((mSeedType == SeedType::SEED_EXPLODE_O_NUT && aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_IN_VAULT) ||
                (mSeedType == SeedType::SEED_TANGLEKELP && !aZombie->mInPool))
            {
                continue;
            }

#ifdef _HAS_BLOOMANDDOOM_CONTENTS
            if (mSeedType == SeedType::SEED_YAMPOLINE && (aZombie->mYampolineCounter > 0 || aZombie->IsDeadOrDying() || !aZombie->mHasHead || 
                aZombie->mZombieType == ZombieType::ZOMBIE_BOSS || aZombie->mZombieType == ZombieType::ZOMBIE_CATAPULT || 
                aZombie->mZombieType == ZombieType::ZOMBIE_ZAMBONI))
                continue;
#endif

            Rect aZombieRect = aZombie->GetZombieRect();
            if ((aZombie->mZombiePhase == ZombiePhase::PHASE_POLEVAULTER_POST_VAULT || aZombie->mZombieType == ZombieType::ZOMBIE_POGO) && aZombie->mZombiePhase == ZombiePhase::PHASE_ZOMBIE_NORMAL &&
                (mSeedType == SeedType::SEED_SPIKEWEED || mSeedType == SeedType::SEED_SPIKEROCK))
                aZombieRect.mWidth += 50;
            if (!needPortalCheck && GetRectOverlap(aAttackRect, aZombieRect) < -aExtraRange)
            {
                continue;
            }

            ////////////////////

            int aWeight = -aZombieRect.mX;
            if (mSeedType == SeedType::SEED_CATTAIL 
#ifdef _HAS_BLOOMANDDOOM_CONTENTS
                || mSeedType == SeedType::SEED_BEEHIVE
#endif      
            )
            {
                aWeight = -Distance2D(mX + 40.0f, mY + 40.0f, aZombieRect.mX + aZombieRect.mWidth / 2, aZombieRect.mY + aZombieRect.mHeight / 2);
                if (aZombie->IsFlying())
                {
                    aWeight += 10000;  // 优先攻击飞行单位
                }
            }

            if (aBestZombie == nullptr || aWeight > aHighestWeight)
            {
                aHighestWeight = aWeight;
                aBestZombie = aZombie;
            }
        }
    }

    return aBestZombie;
}

//0x4678B0
int Plant::DistanceToClosestZombie()
{
    int aDamageRangeFlags = GetDamageRangeFlags(PlantWeapon::WEAPON_PRIMARY);
    Rect aAttackRect = GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);
    int aClosestDistance = 1000;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie))
    {
        if (aZombie->mRow == mRow && aZombie->EffectedByDamage(aDamageRangeFlags))
        {
            Rect aZombieRect = aZombie->GetZombieRect();
            int aDistance = -GetRectOverlap(aAttackRect, aZombieRect);
            if (aDistance < aClosestDistance)
            {
                aClosestDistance = max(aDistance, 0);
            }
        }
    }

    return aClosestDistance;
}

//0x4679B0
void Plant::Die()
{
    if (IsOnBoard() && !mApp->GetDialog(DIALOG_ALMANAC) && mApp->ChallengeHasScores(mApp->mGameMode) && mApp->IsIZombieLevel()) {
        mBoard->mChallenge->mChallengePoints += GetCost(mSeedType, mImitaterType);
    }

    if (IsOnBoard() && mSeedType == SeedType::SEED_TANGLEKELP)
    {
        Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
        if (aZombie)
        {
            aZombie->DieWithLoot();
        }
    }

    mDead = true;
    RemoveEffects();

    if (!Plant::IsFlying(mSeedType) && IsOnBoard())
    {
        GridItem* aLadder = mBoard->GetLadderAt(mPlantCol, mRow);
        if (aLadder)
        {
            aLadder->GridItemDie();
        }
    }

    if (IsOnBoard())
    {
        {
            Plant* aTopPlant = mBoard->GetTopPlantAt(mPlantCol, mRow, PlantPriority::TOPPLANT_BUNGEE_ORDER);
            Plant* aFlowerPot = mBoard->GetFlowerPotAt(mPlantCol, mRow);
            if (aFlowerPot && aTopPlant == aFlowerPot)
            {
                Reanimation* aPotReanim = mApp->ReanimationGet(aFlowerPot->mBodyReanimID);
                aPotReanim->mAnimRate = RandRangeFloat(10.0f, 15.0f);
            }
        }

        if (mSeedType == SEED_COBCANNON)
        {
            Plant* aTopPlant = mBoard->GetTopPlantAt(mPlantCol + 1, mRow, PlantPriority::TOPPLANT_BUNGEE_ORDER);
            Plant* aFlowerPot = mBoard->GetFlowerPotAt(mPlantCol + 1, mRow);
            if (aFlowerPot && aTopPlant == aFlowerPot)
            {
                Reanimation* aPotReanim = mApp->ReanimationGet(aFlowerPot->mBodyReanimID);
                aPotReanim->mAnimRate = RandRangeFloat(10.0f, 15.0f);
            }

            if (mBoard->mCursorObject->mCobCannonPlantID == (PlantID)mBoard->mPlants.DataArrayGetID(this)) {
                mBoard->ClearCursor();
            }
        }
    }
}

PlantDefinition& GetPlantDefinition(SeedType theSeedType)
{
    TOD_ASSERT(gPlantDefs[theSeedType].mSeedType == theSeedType);
    TOD_ASSERT(theSeedType >= 0 && theSeedType < (int)SeedType::NUM_SEED_TYPES);
    
    return gPlantDefs[theSeedType];
}

//0x467B00
int Plant::GetCost(SeedType theSeedType, SeedType theImitaterType)
{
    if (!gLawnApp->GetDialog(Dialogs::DIALOG_ALMANAC) && (gLawnApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED || gLawnApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST))
    {
        if (theSeedType == SeedType::SEED_REPEATER)
        {
            return 1000;
        }
        else if (theSeedType == SeedType::SEED_FUMESHROOM)
        {
            return 500;
        }
        else if (theSeedType == SeedType::SEED_TALLNUT)
        {
            return 250;
        }
        else if (theSeedType == SeedType::SEED_BEGHOULED_BUTTON_SHUFFLE)
        {
            return 100;
        }
        else if (theSeedType == SeedType::SEED_BEGHOULED_BUTTON_CRATER)
        {
            return 200;
        }
    }

    switch (theSeedType)
    {
    case SeedType::SEED_SLOT_MACHINE_SUN:           return 0;
    case SeedType::SEED_SLOT_MACHINE_DIAMOND:       return 0;
    case SeedType::SEED_ZOMBIQUARIUM_SNORKLE:       return 100;
    case SeedType::SEED_ZOMBIQUARIUM_TROPHY:        return 1000;
    case SeedType::SEED_ZOMBIE_NORMAL:              return 50;
    case SeedType::SEED_ZOMBIE_TRAFFIC_CONE:        return 75;
    case SeedType::SEED_ZOMBIE_POLEVAULTER:         return 75;
    case SeedType::SEED_ZOMBIE_PAIL:                return 125;
    case SeedType::SEED_ZOMBIE_LADDER:              return 150;
    case SeedType::SEED_ZOMBIE_DIGGER:              return 125;
    case SeedType::SEED_ZOMBIE_BUNGEE:              return 125;
    case SeedType::SEED_ZOMBIE_FOOTBALL:            return 175;
    case SeedType::SEED_ZOMBIE_BALLOON:             return 150;
    case SeedType::SEED_ZOMBIE_SCREEN_DOOR:         return 100;
    case SeedType::SEED_ZOMBONI:                    return 175;
    case SeedType::SEED_ZOMBIE_POGO:                return 200;
    case SeedType::SEED_ZOMBIE_DANCER:              return 350;
    case SeedType::SEED_ZOMBIE_GARGANTUAR:          return 300;
    case SeedType::SEED_ZOMBIE_IMP:                 return 50;
    default:
    {
        if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
        {
            const PlantDefinition& aPlantDef = GetPlantDefinition(theImitaterType);
            return aPlantDef.mSeedCost;
        }
        else
        {
            const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
            return aPlantDef.mSeedCost;
        }
    }
    }
}
//0x467C00
// GOTY @Patoke: 0x46B6C0
SexyString Plant::GetNameString(SeedType theSeedType, SeedType theImitaterType)
{
    const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
    SexyString aName = StrFormat(_S("[%s]"), aPlantDef.mPlantName);
    SexyString aTranslatedName = TodStringTranslate(aName);

    if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
    {
        const PlantDefinition& aImitaterDef = GetPlantDefinition(theImitaterType);
        SexyString aImitaterName = StrFormat(_S("[%s]"), aImitaterDef.mPlantName);
        SexyString aTranslatedImitaterName = TodStringTranslate(aImitaterName);
        return StrFormat(_S("%s %s"), aTranslatedName.c_str(), aTranslatedImitaterName.c_str());
    }

    return aTranslatedName;
}

//0x467DB0
SexyString Plant::GetToolTip(SeedType theSeedType)
{
    const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
    SexyString aToolTip = StrFormat(_S("[%s_TOOLTIP]"), aPlantDef.mPlantName);
    return TodStringTranslate(aToolTip);
}

//0x467E30
int Plant::GetRefreshTime(SeedType theSeedType, SeedType theImitaterType)
{
    if (Challenge::IsZombieSeedType(theSeedType))
    {
        return 0;
    }

    if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
    {
        const PlantDefinition& aPlantDef = GetPlantDefinition(theImitaterType);
        return aPlantDef.mRefreshTime;
    }
    else
    {
        const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
        return aPlantDef.mRefreshTime;
    }
}

//0x467E60
bool Plant::IsNocturnal(SeedType theSeedtype)
{
    return
        theSeedtype == SeedType::SEED_PUFFSHROOM ||
        theSeedtype == SeedType::SEED_SEASHROOM ||
        theSeedtype == SeedType::SEED_SUNSHROOM ||
        theSeedtype == SeedType::SEED_FUMESHROOM ||
        theSeedtype == SeedType::SEED_HYPNOSHROOM ||
        theSeedtype == SeedType::SEED_DOOMSHROOM ||
        theSeedtype == SeedType::SEED_ICESHROOM ||
        theSeedtype == SeedType::SEED_MAGNETSHROOM ||
        theSeedtype == SeedType::SEED_SCAREDYSHROOM ||
        theSeedtype == SeedType::SEED_GLOOMSHROOM;
}

// GOTY @Patoke: inlined 0x40FB2B
bool Plant::IsFungus(SeedType theSeedtype)
{
    return
        theSeedtype == SeedType::SEED_PUFFSHROOM ||
        theSeedtype == SeedType::SEED_SUNSHROOM ||
        theSeedtype == SeedType::SEED_FUMESHROOM ||
        theSeedtype == SeedType::SEED_HYPNOSHROOM ||
        theSeedtype == SeedType::SEED_SCAREDYSHROOM ||
        theSeedtype == SeedType::SEED_ICESHROOM ||
        theSeedtype == SeedType::SEED_DOOMSHROOM ||
        theSeedtype == SeedType::SEED_SEASHROOM ||
        theSeedtype == SeedType::SEED_MAGNETSHROOM ||
        theSeedtype == SeedType::SEED_GLOOMSHROOM;
}


//0x467EA0
bool Plant::IsAquatic(SeedType theSeedType)
{
    return
        theSeedType == SeedType::SEED_LILYPAD ||
        theSeedType == SeedType::SEED_TANGLEKELP ||
        theSeedType == SeedType::SEED_SEASHROOM ||
        theSeedType == SeedType::SEED_CATTAIL;
}

bool Plant::IsFlying(SeedType theSeedtype)
{
    return theSeedtype == SeedType::SEED_INSTANT_COFFEE 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || theSeedtype == SeedType::SEED_CHERRYHOVERBOMB || theSeedtype == SeedType::SEED_STINGER
#endif
        ;
}

//0x467EC0
bool Plant::IsUpgrade(SeedType theSeedtype)
{
    return
        theSeedtype == SeedType::SEED_GATLINGPEA ||
        theSeedtype == SeedType::SEED_WINTERMELON ||
        theSeedtype == SeedType::SEED_TWINSUNFLOWER ||
        theSeedtype == SeedType::SEED_SPIKEROCK ||
        theSeedtype == SeedType::SEED_COBCANNON ||
        theSeedtype == SeedType::SEED_GOLD_MAGNET ||
        theSeedtype == SeedType::SEED_GLOOMSHROOM ||
        theSeedtype == SeedType::SEED_CATTAIL;
}

//0x467EF0
Rect Plant::GetPlantRect()
{
    Rect aRect;
    if (mSeedType == SeedType::SEED_TALLNUT)
    {
        aRect = Rect(mX + 10, mY, mWidth, mHeight);
    }
    else if (mSeedType == SeedType::SEED_PUMPKINSHELL)
    {
        aRect = Rect(mX, mY, mWidth - 20, mHeight);
    }
    else if (mSeedType == SeedType::SEED_COBCANNON)
    {
        aRect = Rect(mX, mY, 140, 80);
    }
    else
    {
        aRect = Rect(mX + 10, mY, mWidth - 20, mHeight);
    }

    return aRect;
}

//0x467F90
Rect Plant::GetPlantAttackRect(PlantWeapon thePlantWeapon)
{
    Rect aRect;
    if (mApp->IsWallnutBowlingLevel())
    {
        aRect = Rect(mX, mY, mWidth - 20, mHeight);
    }
    else if (thePlantWeapon == PlantWeapon::WEAPON_SECONDARY && mSeedType == SeedType::SEED_SPLITPEA)
    {
        aRect = Rect(0, mY, mX + 16, mHeight);
    }
    else switch (mSeedType)
    {
    case SeedType::SEED_LEFTPEATER:     aRect = Rect(0,             mY,             mX,                 mHeight);               break;
    case SeedType::SEED_SQUASH:         aRect = Rect(mX + 20,       mY,             mWidth - 35,        mHeight);               break;
    case SeedType::SEED_CHOMPER:        aRect = Rect(mX + 80,       mY,             40,                 mHeight);               break;
    case SeedType::SEED_SPIKEWEED:
    case SeedType::SEED_SPIKEROCK:      aRect = Rect(mX + 20,       mY,             mWidth - 50,        mHeight);               break;
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_YAMPOLINE:      aRect = Rect(mX + 40,       mY,             mWidth - 79,        mHeight);               break;
#endif
    case SeedType::SEED_POTATOMINE:     aRect = Rect(mX,            mY,             mWidth - 25,        mHeight);               break;
    case SeedType::SEED_TORCHWOOD:      aRect = Rect(mX + 50,       mY,             30,                 mHeight);               break;
    case SeedType::SEED_PUFFSHROOM:
    case SeedType::SEED_SEASHROOM:      aRect = Rect(mX + 60,       mY,             230,                mHeight);               break;
    case SeedType::SEED_FUMESHROOM:     aRect = Rect(mX + 60,       mY,             340,                mHeight);               break;
    case SeedType::SEED_GLOOMSHROOM:    aRect = Rect(mX - 80,       mY - 80,        240,                240);                   break;
    case SeedType::SEED_TANGLEKELP:     aRect = Rect(mX,            mY,             mWidth,             mHeight);               break;
    case SeedType::SEED_CATTAIL:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SeedType::SEED_BEEHIVE:
#endif
                                        aRect = Rect(-BOARD_WIDTH,  -BOARD_HEIGHT,  BOARD_WIDTH * 2,    BOARD_HEIGHT * 2);      break;
    default:                            aRect = Rect(mX + 60,       mY,             BOARD_WIDTH,        mHeight);               break;
    }

    return aRect;
}

//0x4681E0
bool Plant::PreloadPlantResources(SeedType theSeedType)
{
    bool needPreload = false;
    const PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
    if (&gReanimatorDefArray[(int)aPlantDef.mReanimationType].mTracks.tracks == nullptr)
        needPreload = true;
    if (aPlantDef.mReanimationType != ReanimationType::REANIM_NONE)
    {
        ReanimatorEnsureDefinitionLoaded(aPlantDef.mReanimationType, true);
    }

#ifdef _DS_MINIGAMES
    if (theSeedType == SeedType::SEED_PEASHOOTER && gLawnApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE)
    {
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_HEATWAVE_SHOOTER, true);
    }
#endif

    if (theSeedType == SeedType::SEED_CHERRYBOMB || theSeedType == SeedType::SEED_EXPLODE_O_NUT 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
        || theSeedType == SeedType::SEED_CHERRYHOVERBOMB 
#endif
    )
    {
        if (&gReanimatorDefArray[(int)ReanimationType::REANIM_ZOMBIE_CHARRED].mTracks.tracks == nullptr)
            needPreload = true;
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE_CHARRED, true);
        
    }
    else if (theSeedType == SeedType::SEED_JALAPENO)
    {
        if (&gReanimatorDefArray[(int)ReanimationType::REANIM_JALAPENO_FIRE].mTracks.tracks == nullptr)
            needPreload = true;
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_JALAPENO_FIRE, true);
    }
    else if (theSeedType == SeedType::SEED_TORCHWOOD)
    {
        if (&gReanimatorDefArray[(int)ReanimationType::REANIM_FIRE_PEA].mTracks.tracks == nullptr  &&
            &gReanimatorDefArray[(int)ReanimationType::REANIM_JALAPENO_FIRE].mTracks.tracks == nullptr)
            needPreload = true;
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_FIRE_PEA, true);
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_JALAPENO_FIRE, true);
        
    }
    else if (Plant::IsNocturnal(theSeedType))
    {
        if (&gReanimatorDefArray[(int)ReanimationType::REANIM_SLEEPING].mTracks.tracks == nullptr)
            needPreload = true;
        ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SLEEPING, true);
        
    }

    return needPreload;
}

//0x468280
void Plant::PlayIdleAnim(float theRate)
{
    Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
    if (aBodyReanim)
    {
        const char* aTrackAnim = "anim_idle";

#ifdef _DS_MINIGAMES
        if (mSeedType == SeedType::SEED_PEASHOOTER && mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE) {
            if (mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED || mState == PlantState::STATE_HEAT_WAVE_EXHAUSTED_2)
                aTrackAnim = "anim_exhausted";
        }
#endif
        
        PlayBodyReanim(aTrackAnim, ReanimLoopType::REANIM_LOOP, 20, theRate);
        if (mApp->IsIZombieLevel())
        {
            aBodyReanim->mAnimRate = 0.0f;
        }
    }
}

void Plant::UpdateBurn()
{
    mBurnedCounter--;
    if (mBurnedCounter == 0)
    {
        Die();
    }
}

void Plant::UpdatePowered()
{
    mPoweredCounter--;
    if (mPoweredCounter == 0)
    {
        mPoweredCounter = -1;
        mState = PlantState::STATE_NOTREADY;
        Reanimation* aHeadReanim = mApp->ReanimationTryToGet(mHeadReanimID);
        Reanimation* aBodyReanim = mApp->ReanimationTryToGet(mBodyReanimID);
        if (aHeadReanim && aBodyReanim)
        {
            EndBlink();
            aHeadReanim->SetImageOverride("idle_mouth", nullptr);
            aHeadReanim->SetImageOverride("anim_face", nullptr);
            aHeadReanim->SetImageOverride("anim_sprout", nullptr);
            aBodyReanim->SetImageOverride("frontleaf_tip_left", nullptr);
            aBodyReanim->SetImageOverride("frontleaf_right_tip", nullptr);
            aBodyReanim->SetImageOverride("frontleaf", nullptr);
            aBodyReanim->SetImageOverride("stalk_top", nullptr);
            aBodyReanim->SetImageOverride("stalk_bottom", nullptr);
            aBodyReanim->SetImageOverride("backleaf_right_tip", nullptr);
            aBodyReanim->SetImageOverride("backleaf_left_tip", nullptr);
            aBodyReanim->SetImageOverride("backleaf", nullptr);
        }
    }
}

void Plant::DrawPumkin(Graphics* g, DrawVariation theDrawVariation, unsigned int theDrawBitVariation) 
{
    SeedType aSeedType = SeedType::SEED_PUMPKINSHELL;
    SeedType aImitaterType = mImitaterType;

    if (aImitaterType == SeedType::SEED_IMITATER)
    {
        aSeedType = mImitaterType;
        aImitaterType = SeedType::SEED_PUMPKINSHELL;
    }


    DrawSeedType(g, aSeedType, aImitaterType, theDrawVariation, 0, 65.0f, theDrawBitVariation);
}

void Plant::ApplyBurn()
{
    if (mBurnedCounter != -1) return;

    if (IsOnBoard() && mBoard->mGridSquareType[mPlantCol][mRow] == GridSquareType::GRIDSQUARE_POOL)
    {
        Die();
        return;
    }

    mBurnedCounter = 300;
    mApp->RemoveReanimation(mBlinkReanimID);
    mApp->RemoveReanimation(mLightReanimID);
    mApp->RemoveReanimation(mSleepingReanimID);

    UpdateReanimColor();
}