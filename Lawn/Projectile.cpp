#include "Board.h"
#include "Plant.h"
#include "Zombie.h"
#include "Cutscene.h"
#include "Projectile.h"
#include "../LawnApp.h"
#include "../Resources.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/TodFoley.h"
#include "../Sexy.TodLib/TodDebug.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/Attachment.h"
#include "Widget/AchievementsScreen.h"
#include "../Sexy.TodLib/Trail.h"
#include "../Sexy.TodLib/EffectSystem.h"
#include "../Sexy.TodLib/FilterEffect.h"

ProjectileDefinition gProjectileDefinition[] = {  //0x69F1C0
	{ ProjectileType::PROJECTILE_PEA,           0,  20  },
	{ ProjectileType::PROJECTILE_SNOWPEA,       0,  20  },
	{ ProjectileType::PROJECTILE_CABBAGE,       0,  40  },
	{ ProjectileType::PROJECTILE_MELON,         0,  80  },
	{ ProjectileType::PROJECTILE_PUFF,          0,  20  },
	{ ProjectileType::PROJECTILE_WINTERMELON,   0,  80  },
	{ ProjectileType::PROJECTILE_FIREBALL,      0,  40  },
	{ ProjectileType::PROJECTILE_STAR,          0,  20  },
	{ ProjectileType::PROJECTILE_SPIKE,         0,  20  },
	{ ProjectileType::PROJECTILE_BASKETBALL,    0,  75  },
	{ ProjectileType::PROJECTILE_KERNEL,        0,  20  },
	{ ProjectileType::PROJECTILE_COBBIG,        0,  300 },
	{ ProjectileType::PROJECTILE_BUTTER,        0,  40  },
	{ ProjectileType::PROJECTILE_ZOMBIE_PEA,    0,  20  },
#ifdef _PIERCING_CACTUS
	{ ProjectileType::PROJECTILE_PIERCE_SPIKE,  0,  20  },
#endif
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	{ ProjectileType::PROJECTILE_LETTUCE,       10, 40  },
	{ ProjectileType::PROJECTILE_BEE,			3,  20  }
#endif
};

Projectile::Projectile()
{
}

Projectile::~Projectile()
{
	AttachmentDie(mAttachmentID);
}

//0x46C730
void Projectile::ProjectileInitialize(int theX, int theY, int theRenderOrder, int theRow, ProjectileType theProjectileType)
{
	int aGridX = mBoard->PixelToGridXKeepOnBoard(theX, theY);
	mProjectileType = theProjectileType;
	mPosX = theX;
	mPosY = theY;
	mPosZ = 0.0f;
	mVelX = 0.0f;
	mVelY = 0.0f;
	mVelZ = 0.0f;
	mAccZ = 0.0f;
	mShadowY = mBoard->GridToPixelY(aGridX, theRow) + 67.0f;
	mHitTorchwoodGridX = -1;
	mMotionType = ProjectileMotion::MOTION_STRAIGHT;
	mFrame = 0;
	mNumFrames = 1;
	mRow = theRow;
	mCobTargetX = 0.0f;
	mDamageRangeFlags = 0;
	mDead = false;
	mAttachmentID = AttachmentID::ATTACHMENTID_NULL;
	mCobTargetRow = 0;
	mTargetZombieID = ZombieID::ZOMBIEID_NULL;
	mOnHighGround = mBoard->mGridSquareType[aGridX][theRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND;
	mRad = 0.0f;
	if (mBoard->StageHasRoof())
	{
		mShadowY -= 12.0f;
	}
	mRenderOrder = theRenderOrder;
	mRotation = 0.0f;
	mRotationSpeed = 0.0f;
	mWidth = 40;
	mHeight = 40;
	mProjectileAge = 0;
	mClickBackoffCounter = 0;
	mAnimTicksPerFrame = 0;
	mHasImpactAnimation = false;
	mIsPlayingImpactAnimation = false;
	mAnimOffset = 0;
	mOverrideColor = Color::White;
	mExtraAdditiveColor = Color::Black;
	mEnableExtraAdditiveDraw = false;
	mFilterEffect = FilterEffect::FILTER_EFFECT_NONE;
	memset(mPiercedZombies, 0, sizeof(mPiercedZombies));
	mNumPierced = 0;

	if (mProjectileType == ProjectileType::PROJECTILE_CABBAGE || mProjectileType == ProjectileType::PROJECTILE_BUTTER)
	{
		mRotation = -7 * PI / 25;  // DEG_TO_RAD(-50.4f);
		mRotationSpeed = RandRangeFloat(-0.08f, -0.02f);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON)
	{
		mRotation = -2 * PI / 5;  // DEG_TO_RAD(-72.0f);
		mRotationSpeed = RandRangeFloat(-0.08f, -0.02f);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
	{
		mRotation = -2 * PI / 5;  // DEG_TO_RAD(-72.0f);
		mRotationSpeed = RandRangeFloat(-0.08f, -0.02f);
		Trail* aTrail = mApp->mEffectSystem->mTrailHolder->AllocTrail(305000, TrailType::TRAIL_ICE);
		if (aTrail) 
		{
			aTrail->mFilterEffect = mFilterEffect;
			AttachTrail(mAttachmentID, aTrail, 20.0f, 20.0f);
		}
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 8.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_SNOWPEA_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 18.0f, 23.0f);
		if (aParticle)
			aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_KERNEL)
	{
		mRotation = 0.0f;
		mRotationSpeed = RandRangeFloat(-0.2f, -0.08f);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
	{
		Trail* aTrail = mApp->mEffectSystem->mTrailHolder->AllocTrail(305000, TrailType::TRAIL_ICE);
		if (aTrail) 
		{
			aTrail->mFilterEffect = mFilterEffect;
			AttachTrail(mAttachmentID, aTrail, 8.0f, 13.0f);
		}
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 8.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_SNOWPEA_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 8.0f, 13.0f);
		if (aParticle)
			aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		//TOD_ASSERT();
		bool needsReanim = true;
		Attachment* aAttachment = gEffectSystem->mAttachmentHolder->mAttachments.DataArrayTryToGet(mAttachmentID);
		if (aAttachment)
		{
			for (auto effects : aAttachment->mEffectArray)
			{
				if (effects.mEffectType == EffectType::EFFECT_REANIM)
				{
					Reanimation* aReanim = mApp->ReanimationTryToGet((ReanimationID)effects.mEffectID);
					if (aReanim && aReanim->mReanimationType == ReanimationType::REANIM_FIRE_PEA)
					{
						needsReanim = false;
						break;
					}
				}
			}
		}

		if (needsReanim)
		{
			float aOffsetX = -25.0f;
			float aOffsetY = -25.0f;
			Reanimation* aFirePeaReanim = mApp->AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_FIRE_PEA);
			if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
			{
				aFirePeaReanim->OverrideScale(-1.0f, 1.0f);
				aOffsetX += 80.0f;
			}
			aFirePeaReanim->SetPosition(mPosX + aOffsetX, mPosY + aOffsetY);
			aFirePeaReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
			aFirePeaReanim->mAnimRate = RandRangeFloat(50.0f, 80.0f);
			if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
				aFirePeaReanim->mFilterEffect = mFilterEffect;
			}
			AttachReanim(mAttachmentID, aFirePeaReanim, aOffsetX, aOffsetY);
		}
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		mWidth = IMAGE_REANIM_COBCANNON_COB->GetWidth();
		mHeight = IMAGE_REANIM_COBCANNON_COB->GetHeight();
		mRotation = PI / 2;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_PUFF)
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 13.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_PUFFSHROOM_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 13.0f, 13.0f);
		if (aParticle)
			aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL)
	{
		mRotation = RandRangeFloat(0.0f, 2 * PI);
		mRotationSpeed = RandRangeFloat(0.05f, 0.1f);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_STAR)
	{
		mShadowY += 15.0f;
		mRotationSpeed = RandRangeFloat(0.05f, 0.1f);
		if (Rand(2) == 0)
		{
			mRotationSpeed = -mRotationSpeed;
		}
	}
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	else if (mProjectileType == ProjectileType::PROJECTILE_LETTUCE)
	{
		mHasImpactAnimation = true;
		mNumFrames = 8;
		mAnimTicksPerFrame = 12;
		Trail* aTrail = mApp->mEffectSystem->mTrailHolder->AllocTrail(305000, TrailType::TRAIL_ICE);
		if (aTrail) 
		{
			aTrail->mFilterEffect = mFilterEffect;
			AttachTrail(mAttachmentID, aTrail, 10.0f, 10.0f);
		}
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 8.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_SNOWPEA_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 8.0f, 13.0f);
		if (aParticle)
			aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BEE)
	{
		mNumFrames = 8;
		mAnimTicksPerFrame = 12;
	}
#endif

	mAnimCounter = 0;
	mX = (int)mPosX;
	mY = (int)mPosY;
}

//0x46CAA0
Plant* Projectile::FindCollisionTargetPlant()
{
	Rect aProjectileRect = GetProjectileRect();

	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		if (aPlant->mRow != mRow || aPlant->mBurnedCounter != -1)
			continue;

		if (mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA)
		{
			if (aPlant->mSeedType == SeedType::SEED_PUFFSHROOM ||
				aPlant->mSeedType == SeedType::SEED_SEASHROOM ||
				aPlant->mSeedType == SeedType::SEED_SUNSHROOM && aPlant->mState != PlantState::STATE_SUNSHROOM_BIG||
				aPlant->mSeedType == SeedType::SEED_POTATOMINE ||
				aPlant->mSeedType == SeedType::SEED_SPIKEWEED ||
				aPlant->mSeedType == SeedType::SEED_SPIKEROCK ||
				aPlant->mSeedType == SeedType::SEED_LILYPAD ||
				aPlant->mState == PlantState::STATE_SCAREDYSHROOM_SCARED
				|| aPlant->mSeedType == SeedType::SEED_TANGLEKELP
				|| aPlant->mSeedType == SeedType::SEED_FLOWERPOT
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
				|| aPlant->mSeedType == SeedType::SEED_YAMPOLINE
#endif
				)  // 僵尸豌豆不能击中低矮植物
				continue;
		}

		Rect aPlantRect = aPlant->GetPlantRect();
		if (GetRectOverlap(aProjectileRect, aPlantRect) > 8)
		{
			if (mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA)
			{
				Plant* aPlant2 = mBoard->GetTopPlantAt(aPlant->mPlantCol, aPlant->mRow, PlantPriority::TOPPLANT_EATING_ORDER);
				if (aPlant2 && (aPlant2->mSeedType == SeedType::SEED_PUFFSHROOM ||
					aPlant2->mSeedType == SeedType::SEED_SUNSHROOM && aPlant->mState != PlantState::STATE_SUNSHROOM_BIG ||
					aPlant2->mSeedType == SeedType::SEED_POTATOMINE ||
					aPlant2->mSeedType == SeedType::SEED_SPIKEWEED ||
					aPlant2->mSeedType == SeedType::SEED_SPIKEROCK ||
					aPlant2->mSeedType == SeedType::SEED_LILYPAD ||
					aPlant2->mState == PlantState::STATE_SCAREDYSHROOM_SCARED
					|| aPlant2->mSeedType == SeedType::SEED_TANGLEKELP
					|| aPlant2->mSeedType == SeedType::SEED_FLOWERPOT
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
					|| aPlant2->mSeedType == SeedType::SEED_YAMPOLINE
#endif
					))  // 僵尸豌豆不能击中低矮植物
					continue;

				return aPlant2;
			}
			else
			{
				return mBoard->GetTopPlantAt(aPlant->mPlantCol, aPlant->mRow, PlantPriority::TOPPLANT_CATAPULT_ORDER);
			}
		}
	}

	return nullptr;
}

//0x46CC30
bool Projectile::PeaAboutToHitTorchwood()
{
	if (mMotionType != ProjectileMotion::MOTION_STRAIGHT)
		return false;

	if (mProjectileType != ProjectileType::PROJECTILE_PEA && mProjectileType != ProjectileType::PROJECTILE_SNOWPEA)
		return false;

	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		if (aPlant->mSeedType == SeedType::SEED_TORCHWOOD && aPlant->mRow == mRow && !aPlant->NotOnGround() && mHitTorchwoodGridX != aPlant->mPlantCol)
		{
			Rect aPlantAttackRect = aPlant->GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);
			Rect aProjectileRect = GetProjectileRect();
			aProjectileRect.mX += 40;

			if (GetRectOverlap(aPlantAttackRect, aProjectileRect) > 10)
			{
				bool result = true;

				Zombie* aZombie = nullptr;
				while (mBoard->IterateZombies(aZombie)) {
					if (aZombie->mRow == mRow && aZombie->mX < aPlantAttackRect.mX - 80) {
						result = false;
						break;
					}
				}
				return result;
			}
		}
	}

	return false;
}

//0x46CD40
Zombie* Projectile::FindCollisionTarget()
{
	if (PeaAboutToHitTorchwood())  // “卡火炬”的原理，这段代码在两版内测版中均不存在
		return nullptr;

	Rect aProjectileRect = GetProjectileRect();
	Zombie* aBestZombie = nullptr;
	int aMinX = 0;

	Zombie* aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		bool isEffected = aZombie->EffectedByDamage((unsigned int)mDamageRangeFlags);

		if (mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA && aZombie->mMindControlled) {
			isEffected = aZombie->EffectedByDamage(129);
		}

#ifdef _PIERCING_CACTUS
		if (mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE && mNumPierced > 0)
		{
			unsigned int theZombieID = mBoard->mZombies.DataArrayGetID(aZombie);
			bool isAlreadyPierced = false;

			for (int i = 0; i < mNumPierced; ++i) // Todo: Make a fallback incase this goes beyond the size
			{
				if (mPiercedZombies[i] == theZombieID)
				{
					isAlreadyPierced = true;
					break;
				}
			}

			if (isAlreadyPierced)	continue;
		}
#endif

		if ((aZombie->mZombieType == ZombieType::ZOMBIE_BOSS || aZombie->mRow == mRow) && isEffected)
		{
			if (mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA && !aZombie->mMindControlled)
			{
				continue;
			}

			if (aZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_WALKING_IN_POOL && mPosZ >= 45.0f)
			{
				continue;
			}

			if (mProjectileType == ProjectileType::PROJECTILE_STAR && mProjectileAge < 25 && mVelX >= 0.0f && aZombie->mZombieType == ZombieType::ZOMBIE_DIGGER)
			{
				continue;
			}

			Rect aZombieRect = aZombie->GetZombieRect();
			if (GetRectOverlap(aProjectileRect, aZombieRect) > 0)
			{
				if (aBestZombie == nullptr || aZombie->mX < aMinX)
				{
					aBestZombie = aZombie;
					aMinX = aZombie->mX;
				}
			}
		}
	}

	return aBestZombie;
}

//0x46CE80
void Projectile::CheckForCollision()
{
	if (mMotionType == ProjectileMotion::MOTION_PUFF && mProjectileAge >= 75)
	{
		Die();
		return;
	}

	Zombie* aTargetZombie = mMotionType == ProjectileMotion::MOTION_HOMING ?  mBoard->ZombieTryToGet(mTargetZombieID) : nullptr;

	if ((mPosX > WIDE_BOARD_WIDTH && mProjectileType != ProjectileType::PROJECTILE_ZOMBIE_PEA || mPosX + mWidth < 0) && !aTargetZombie)
	{
		Die();
		return;
	}

	if (mMotionType == ProjectileMotion::MOTION_HOMING 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		|| mProjectileType == ProjectileType::PROJECTILE_BEE
#endif
		)
	{
		Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
		if (aZombie && aZombie->EffectedByDamage((unsigned int)mDamageRangeFlags))
		{
			Rect aProjectileRect = GetProjectileRect();
			Rect aZombieRect = aZombie->GetZombieRect();
			if (GetRectOverlap(aProjectileRect, aZombieRect) >= 0 && mPosY > aZombieRect.mY&& mPosY < aZombieRect.mY + aZombieRect.mHeight)
			{
				DoImpact(aZombie);
			}
		}
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_STAR && (mPosY > 600.0f || mPosY < 0.0f))
	{
		Die();
		return;
	}

	if ((mProjectileType == ProjectileType::PROJECTILE_PEA || mProjectileType == ProjectileType::PROJECTILE_STAR) && mShadowY - mPosY > 90.0f)
	{
		return;
	}

	if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA)
	{
		Plant* aPlant = FindCollisionTargetPlant();
		if (aPlant)
		{
			const ProjectileDefinition& aProjectileDef = GetProjectileDef();
			aPlant->mPlantHealth -= aProjectileDef.mDamage;
			aPlant->mEatenFlashCountdown = max(aPlant->mEatenFlashCountdown, 25);

			mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
			TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX - 3.0f, mPosY + 17.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_PEA_SPLAT);
			if (aParticle)
			{
				aParticle->OverrideColor(nullptr, mOverrideColor);
				if (mEnableExtraAdditiveDraw)
				{
					aParticle->OverrideExtraAdditiveColor(nullptr, mExtraAdditiveColor);
					aParticle->OverrideExtraAdditiveDraw(nullptr, true);
				}
			}
			Die();
		}
	}

	Zombie* aZombie = FindCollisionTarget();
	if (aZombie)
	{
		if (aZombie->mOnHighGround && CantHitHighGround())
		{
			return;
		}

		DoImpact(aZombie);
	}
}

//0x46D090
bool Projectile::CantHitHighGround()
{
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS || mMotionType == ProjectileMotion::MOTION_HOMING
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		|| mProjectileType == ProjectileType::PROJECTILE_BEE
#endif
		)
		return false;

	return (
		mProjectileType == ProjectileType::PROJECTILE_PEA ||
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_STAR ||
		mProjectileType == ProjectileType::PROJECTILE_PUFF ||
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL
		) && !mOnHighGround;
}

//0x46D0D0
void Projectile::CheckForHighGround()
{
	float aShadowDelta = mShadowY - mPosY;

	if (mProjectileType == ProjectileType::PROJECTILE_PEA ||
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL ||
		mProjectileType == ProjectileType::PROJECTILE_SPIKE ||
#ifdef _PIERCING_CACTUS
		mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE ||
#endif
		mProjectileType == ProjectileType::PROJECTILE_COBBIG )
	{
		if (aShadowDelta < 28.0f)
		{
			DoImpact(nullptr);
			return;
		}
	}

	if (mProjectileType == ProjectileType::PROJECTILE_PUFF && aShadowDelta < 0.0f)
	{
		DoImpact(nullptr);
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_STAR && aShadowDelta < 23.0f)
	{
		DoImpact(nullptr);
		return;
	}

	if (CantHitHighGround())
	{
		int aGridX = mBoard->PixelToGridXKeepOnBoard(mPosX + 30, mPosY);
		if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
		{
			DoImpact(nullptr);
		}
	}
}

//0x46D1F0
bool Projectile::IsSplashDamage(Zombie* theZombie)
{
	if (mProjectileType && theZombie && theZombie->IsFireResistant() && (mMotionType == ProjectileMotion::MOTION_STRAIGHT || mMotionType == ProjectileMotion::MOTION_THREEPEATER))
		return false;

	return 
		mProjectileType == ProjectileType::PROJECTILE_MELON || 
		mProjectileType == ProjectileType::PROJECTILE_WINTERMELON || 
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL
#ifdef _SPLASH_SNOWPEA
		|| mProjectileType == ProjectileType::PROJECTILE_SNOWPEA
#endif
		;
}

//0x46D230
unsigned int Projectile::GetDamageFlags(Zombie* theZombie)
{
	unsigned int aDamageFlags = 0U;

	bool considerateHoming = false;
	if (mMotionType == ProjectileMotion::MOTION_HOMING || mMotionType == ProjectileMotion::MOTION_BEE)
	{
		considerateHoming = mVelX < 0.0f;
	}

	if (IsSplashDamage(theZombie) && mProjectileType != ProjectileType::PROJECTILE_SNOWPEA)
	{
		SetBit(aDamageFlags, (int)DamageFlags::DAMAGE_HITS_SHIELD_AND_BODY, true);
	}
	else if (mMotionType == ProjectileMotion::MOTION_LOBBED || (mMotionType == ProjectileMotion::MOTION_BACKWARDS || considerateHoming) != theZombie->IsWalkingBackwards())
	{
		SetBit(aDamageFlags, (int)DamageFlags::DAMAGE_BYPASSES_SHIELD, true);
	}
	else if ((mMotionType == ProjectileMotion::MOTION_STAR && mVelX < 0.0f) != theZombie->IsWalkingBackwards())
	{
		SetBit(aDamageFlags, (int)DamageFlags::DAMAGE_BYPASSES_SHIELD, true);
	}

	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		SetBit(aDamageFlags, (int)DamageFlags::DAMAGE_LOBBED, true);
	}


	if (mProjectileType == ProjectileType::PROJECTILE_SNOWPEA || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		|| mProjectileType == ProjectileType::PROJECTILE_LETTUCE
#endif
		)
	{
		SetBit(aDamageFlags, (int)DamageFlags::DAMAGE_FREEZE, true);
	}

	return aDamageFlags;
}

//0x46D2B0
bool Projectile::IsZombieHitBySplash(Zombie* theZombie)
{
	int aDamageFlags = mDamageRangeFlags;

	Rect aProjectileRect = GetProjectileRect();
	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL
#ifdef _SPLASH_SNOWPEA 
		|| mProjectileType == ProjectileType::PROJECTILE_SNOWPEA
#endif
		)
	{
		if (mMotionType == MOTION_BACKWARDS) 
		{
			aProjectileRect = Rect(mX - mWidth + 10, mY, mWidth - 10, mHeight);
		}
		else 
		{
			aProjectileRect.mWidth = 100;
		}
	}

	int aRowDeviation = theZombie->mRow - mRow;
	Rect aZombieRect = theZombie->GetZombieRect();
	if (theZombie->IsFireResistant() && mMotionType == MOTION_STRAIGHT && mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		return false;
	}

#ifdef _SPLASH_SNOWPEA
	if (!theZombie->CanBeChilled() && mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
	{
		return false;
	}
#endif

	if (theZombie->mZombieType == ZombieType::ZOMBIE_BOSS)
	{
		aRowDeviation = 0;
	}
	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL 
#ifdef _SPLASH_SNOWPEA
		|| mProjectileType == PROJECTILE_SNOWPEA
#endif

		)
	{
		if (aRowDeviation != 0)
		{
			return false;
		}
	}
	else if (aRowDeviation > 1 || aRowDeviation < -1)
	{
		return false;
	}

	return theZombie->EffectedByDamage((unsigned int)aDamageFlags) && GetRectOverlap(aProjectileRect, aZombieRect) >= 0;
}

//0x46D390
void Projectile::DoSplashDamage(Zombie* theZombie)
{
	const ProjectileDefinition& aProjectileDef = GetProjectileDef();

	int aZombiesGetSplashed = 0;
	Zombie* aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		if (aZombie != theZombie && IsZombieHitBySplash(aZombie))
		{
			aZombiesGetSplashed++;
		}
	}

	int aOriginalDamage = aProjectileDef.mDamage;
	int aSplashDamage = aProjectileDef.mDamage / 3;
	int aMaxSplashDamageAmount = aSplashDamage * 7;
	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		aMaxSplashDamageAmount = aOriginalDamage;
	}
	int aSplashDamageAmount = aSplashDamage * aZombiesGetSplashed;
	if (aSplashDamageAmount > aMaxSplashDamageAmount)
	{
		//aSplashDamage *= aMaxSplashDamageAmount / aSplashDamage;
		aSplashDamage = aOriginalDamage * aMaxSplashDamageAmount / (aSplashDamageAmount * 3);
		aSplashDamage = max(aSplashDamage, 1);
	}

	aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		if (IsZombieHitBySplash(aZombie))
		{
			unsigned int aDamageFlags = GetDamageFlags(aZombie);
			if (aZombie == theZombie)
			{
				aZombie->TakeDamage(aOriginalDamage, aDamageFlags);
			}
			else
			{
				aZombie->TakeDamage(aSplashDamage, aDamageFlags);

				if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
				{
					aZombie->RemoveColdEffects();
				}
			}

			if (TestBit(aDamageFlags, (int)DamageFlags::DAMAGE_HITS_SHIELD_AND_BODY) && mMotionType == ProjectileMotion::MOTION_LOBBED)
			{
				aZombie->mHelmetRecoilCounter = 12;
			}
		}
	}
}

//0x46D490
void Projectile::UpdateLobMotion()
{
	float displaceX = -700.0f;

	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG && !FloatApproxEqual(mRad, 0.0f))
	{
		displaceX /= sin(mRad);
	}

	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG && mPosZ < displaceX)
	{
		mVelX = 0.001f;
		mVelZ = 8.0f;
		mRow = mCobTargetRow;
		mPosX = mCobTargetX;
		int aCobTargetCol = mBoard->PixelToGridXKeepOnBoard(mCobTargetX, 0);
		mPosY = mBoard->GridToPixelY(aCobTargetCol, mCobTargetRow);
		mShadowY = mPosY + 67.0f;
		mRotation = -PI / 2;
	}

	float aAccZ = mAccZ;
	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY) {
		aAccZ *= 2;
	}
	mPosZ += mVelZ + 0.5f * aAccZ;
	mVelZ += aAccZ;
	mPosX += mVelX;
	mPosY += mVelY;

	bool isRising = mVelZ < 0.0f;
	if (isRising && (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL || mProjectileType == ProjectileType::PROJECTILE_COBBIG))
	{
		return;
	}
	if (mProjectileAge > 20)
	{
		if (isRising)
		{
			return;
		}

		float aMinCollisionZ = 0.0f;
		if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
		{
			aMinCollisionZ = -32.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL)
		{
			aMinCollisionZ = 60.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
		{
			aMinCollisionZ = -35.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_CABBAGE || mProjectileType == ProjectileType::PROJECTILE_KERNEL)
		{
			aMinCollisionZ = -30.0f;
		}
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		else if (mProjectileType == ProjectileType::PROJECTILE_LETTUCE)
		{
			aMinCollisionZ = -40.0f;
		}
#endif
		else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
		{
			aMinCollisionZ = -60.0f;
		}
		if (mBoard->mPlantRow[mRow] == PlantRowType::PLANTROW_POOL && mX >= 10 && mX <= 700)
		{
			aMinCollisionZ += 40.0f;
		}

		/*int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
		if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
		{
			aMinCollisionZ -= HIGH_GROUND_HEIGHT;
		}*/

		if (mPosZ <= aMinCollisionZ)
		{
			return;
		}
	}

	Plant* aPlant = nullptr;
	Zombie* aZombie = nullptr;

	if (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL)
	{
		aPlant = FindCollisionTargetPlant();
	}
	else
	{
		aZombie = FindCollisionTarget();
	}

	float aGroundZ = 80.0f;
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		aGroundZ = -40.0f;
	}
	int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
	bool isHighGround = false;
	if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
	{
		aGroundZ -= HIGH_GROUND_HEIGHT;
	}
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	if (mProjectileType == PROJECTILE_LETTUCE)
	{
		aGroundZ -= 20;
	}
#endif
	bool hitGround = mPosZ > aGroundZ;
	if (aZombie == nullptr && aPlant == nullptr && !hitGround)
	{
		return;
	}

	if (aPlant)
	{
		Plant* aUmbrellaPlant = mBoard->FindUmbrellaPlant(aPlant->mPlantCol, aPlant->mRow);
		if (aUmbrellaPlant)
		{
			if (aUmbrellaPlant->mState == PlantState::STATE_UMBRELLA_REFLECTING)
			{
				mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
				int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_TOP, 0, 1);
				mApp->AddTodParticle(mPosX + 20.0f, mPosY + 20.0f, aRenderPosition, ParticleEffect::PARTICLE_UMBRELLA_REFLECT);
				Die();
			}
			else if (aUmbrellaPlant->mState != PlantState::STATE_UMBRELLA_TRIGGERED)
			{
				mApp->PlayFoley(FoleyType::FOLEY_UMBRELLA);
				aUmbrellaPlant->DoSpecial();
			}
		}
		else
		{
			aPlant->mPlantHealth -= GetProjectileDef().mDamage;
			aPlant->mEatenFlashCountdown = max(aPlant->mEatenFlashCountdown, 25);
			mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
			Die();
		}
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		int aBeforeGargantuarCount = mBoard->GetLiveGargantuarCount();
		mBoard->KillAllZombiesInRadius(mRow, mPosX + 80, mPosY + 40, 115, 1, true, mDamageRangeFlags);
		int aAfterGargantuarCount = mBoard->GetLiveGargantuarCount();
		int killedGargantuars = aBeforeGargantuarCount - aAfterGargantuarCount;
		if (killedGargantuars > 0) {
			mBoard->mGargantuarsKillsByCornCob += killedGargantuars;
		}

		if (mBoard->mGargantuarsKillsByCornCob >= 2) {
			ReportAchievement::GiveAchievement(mApp, AchievementId::PopcornParty, true);
		}

		DoImpact(nullptr);
	}
	else
	{
		DoImpact(aZombie);
	}
}

//0x46D890
void Projectile::UpdateNormalMotion()
{
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
	{
		mPosX -= 3.33f;
	}
	else if (mMotionType == ProjectileMotion::MOTION_HOMING)
	{
		Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
		if (aZombie && aZombie->EffectedByDamage((unsigned int)mDamageRangeFlags))
		{
			Rect aZombieRect = aZombie->GetZombieRect();
			SexyVector2 aTargetCenter(aZombie->ZombieTargetLeadX(0.0f), aZombieRect.mY + aZombieRect.mHeight / 2);
			SexyVector2 aProjectileCenter(mPosX + mWidth / 2, mPosY + mHeight / 2);
			SexyVector2 aToTarget = (aTargetCenter - aProjectileCenter).Normalize();
			SexyVector2 aMotion(mVelX, mVelY);

			aMotion += aToTarget * (0.001f * mProjectileAge);
			aMotion = aMotion.Normalize();
			aMotion *= 2.0f;

			mVelX = aMotion.x;
			mVelY = aMotion.y;
			mRotation = -atan2(mVelY, mVelX);
		}

		mPosY += mVelY;
		mPosX += mVelX;
		mShadowY += mVelY;
		mRow = mBoard->PixelToGridYKeepOnBoard(mPosX, mPosY);
	}
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	else if (mProjectileType == ProjectileType::PROJECTILE_BEE)
	{
		Zombie* aZombie = mBoard->ZombieTryToGet(mTargetZombieID);
		if (aZombie && aZombie->EffectedByDamage((unsigned int)mDamageRangeFlags))
		{
			Rect aZombieRect = aZombie->GetZombieRect();
			SexyVector2 aTargetCenter(aZombie->ZombieTargetLeadX(0.0f), aZombieRect.mY + aZombieRect.mHeight / 2);
			SexyVector2 aProjectileCenter(mPosX + mWidth / 2, mPosY + mHeight / 2);
			SexyVector2 aToTarget = (aTargetCenter - aProjectileCenter).Normalize();
			SexyVector2 aMotion(mVelX, mVelY);

			aMotion += aToTarget * (0.001f * mProjectileAge);
			aMotion = aMotion.Normalize();
			aMotion *= 2.0f;

			mVelX = aMotion.x;
			mVelY = aMotion.y;
			mRotation = -atan2(mVelY, mVelX);

			if (mVelX > 0) {
				mMotionType = ProjectileMotion::MOTION_BEE; 
			}
			else if (mVelX < 0) {
				mMotionType = ProjectileMotion::MOTION_BEE_BACKWARDS; 
				mRotation -= PI;
			}
		}

		mPosY += mVelY;
		mPosX += mVelX;
		mShadowY += mVelY;
		mRow = mBoard->PixelToGridYKeepOnBoard(mPosX, mPosY);
	}
#endif
	else if (mMotionType == ProjectileMotion::MOTION_STAR)
	{
		mPosY += mVelY;
		mPosX += mVelX;
		mShadowY += mVelY;

		if (mVelY != 0.0f)
		{
			mRow = mBoard->PixelToGridYKeepOnBoard(mPosX, mPosY);
		}
	}
	else if (mMotionType == ProjectileMotion::MOTION_BEE)
	{
		//if (mProjectileAge < 60)
		//{
		//	mPosY -= 0.5f;
		//}
		//mPosX += 3.33f;
	}
	else if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		if (mVelZ < 0.0f)
		{
			mVelZ += 0.002f;
			mVelZ = min(mVelZ, 0.0f);
			mPosY += mVelZ;
			mRotation = 0.3f - 0.7f * mVelZ * PI * 0.25f;
		}
		mPosX += 0.4f;
	}
	else if (mMotionType == ProjectileMotion::MOTION_BEE_BACKWARDS)
	{
		//if (mProjectileAge < 60)
		//{
		//	mPosY -= 0.5f;
		//}
		//mPosX -= 3.33f;
	}
	else if (mMotionType == ProjectileMotion::MOTION_THREEPEATER)
	{
		mPosX += 3.33f;
		mPosY += mVelY;
		mVelY *= 0.97f;
		mShadowY += mVelY;
	}
	else
	{
		mPosX += 3.33f;
	}

	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
	{
		if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
		{
			mVelZ += 0.004f;
		}
		else
		{
			mVelZ += 0.2f;
		}

		mPosY += mVelZ;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL) {
		Reanimation* aFirePeaReanim = FindReanimAttachment(mAttachmentID);
		if (aFirePeaReanim) aFirePeaReanim->mOverlayMatrix.m00 = mMotionType == ProjectileMotion::MOTION_BACKWARDS ? -1 : 1;
	}

	CheckForCollision();
	CheckForHighGround();
}

//0x46DC70
void Projectile::UpdateMotion()
{
	if (mAnimTicksPerFrame > 0)
	{
		mAnimCounter = (mAnimCounter + 1) % (mNumFrames * mAnimTicksPerFrame);
		mFrame = mAnimCounter / mAnimTicksPerFrame;
	}

	int aOldRow = mRow;
	float aOldY = mBoard->GetPosYBasedOnRow(mPosX, mRow);
	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		UpdateLobMotion();
	}
	else
	{
		UpdateNormalMotion();
	}

	float aSlopeHeightChange = mBoard->GetPosYBasedOnRow(mPosX, aOldRow) - aOldY;
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		aSlopeHeightChange = 0.0f;  // 修复“上界之风”
	}
	if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		mPosY += aSlopeHeightChange;
	}
	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		mPosY += aSlopeHeightChange;
		mPosZ -= aSlopeHeightChange;
	}
	mShadowY += aSlopeHeightChange;
	mX = (int)mPosX;
	mY = (int)(mPosY + mPosZ);
}

//0x46DD30
void Projectile::PlayImpactSound(Zombie* theZombie)
{
	bool aPlayHelmSound = true;
	bool aPlaySplatSound = true;
	if (mProjectileType == ProjectileType::PROJECTILE_KERNEL)
	{
		mApp->PlayFoley(FoleyType::FOLEY_KERNEL_SPLAT);
		aPlayHelmSound = false;
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
	{
		mApp->PlayFoley(FoleyType::FOLEY_BUTTER);
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL && IsSplashDamage(theZombie))
	{
		mApp->PlayFoley(FoleyType::FOLEY_IGNITE);
		aPlayHelmSound = false;
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
	{
		mApp->PlayFoley(FoleyType::FOLEY_MELONIMPACT);
		aPlaySplatSound = false;
	}

	if (aPlayHelmSound && theZombie)
	{
		if (theZombie->mHelmType == HELMTYPE_BUCKET)
		{
			mApp->PlayFoley(FoleyType::FOLEY_SHIELD_HIT);
			aPlaySplatSound = false;
		}
		else if (theZombie->mHelmType == HELMTYPE_TRAFFIC_CONE || theZombie->mHelmType == HELMTYPE_DIGGER || theZombie->mHelmType == HELMTYPE_FOOTBALL)
		{
			mApp->PlayFoley(FoleyType::FOLEY_PLASTIC_HIT);
		}
	}

	if (aPlaySplatSound)
	{
		mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
	}
}

//0x46E000
void Projectile::DoImpact(Zombie* theZombie)
{
	ProjectileDefinition aProjDef = GetProjectileDef();
#ifdef _PIERCING_CACTUS
	if (theZombie && mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE)
	{
		unsigned int theZombieID = mBoard->mZombies.DataArrayGetID(theZombie);
		bool isAlreadyPierced = false;

		if (mNumPierced > 0)
		{
			aProjDef.mDamage = 10;
			
			for (int i = 0; i < mNumPierced; ++i) // Todo: Make a fallback incase this goes beyond the size
			{
				if (mPiercedZombies[i] == theZombieID)
				{
					isAlreadyPierced = true;
					break;
				}
			}
		}

		if (isAlreadyPierced)
		{
			return;
		}
		else
		{
			mPiercedZombies[mNumPierced++] = theZombieID;

			if (theZombie->mShieldType != ShieldType::SHIELDTYPE_NONE && mNumPierced == 1)
			{
				mNumPierced++;
				theZombie->TakeBodyDamage(8, GetDamageFlags(theZombie));
			}

			if (theZombie->IsFlying())
			{
				mNumPierced++;
			}
		}
	}
#endif
	PlayImpactSound(theZombie);

	/*if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL) {
		const int FIREBALL_RANGE = 50;
		const Rect screenRect{ 0, 0, BOARD_WIDTH, BOARD_HEIGHT };

		Rect lightRect = Rect(mPosX + 40 - FIREBALL_RANGE, mPosY + 20 - 10 - FIREBALL_RANGE, FIREBALL_RANGE * 2, FIREBALL_RANGE * 2);
		lightRect = lightRect.Intersection(screenRect);
		mBoard->mLightSourceV.push_back(std::make_pair(50, lightRect));
	}*/

	if (IsSplashDamage(theZombie))
	{
		if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL && theZombie)
		{
			theZombie->RemoveColdEffects();
		}

		DoSplashDamage(theZombie);
	}
	else if (theZombie)
	{
		unsigned int aDamageFlags = GetDamageFlags(theZombie);
		theZombie->TakeDamage(aProjDef.mDamage, aDamageFlags);
	}

	float aLastPosX = mPosX - mVelX;
	float aLastPosY = mPosY + mPosZ - mVelY - mVelZ;
	ParticleEffect aEffect = ParticleEffect::PARTICLE_NONE;
	float aSplatPosX = mPosX + 12.0f;
	float aSplatPosY = mPosY + 12.0f;
	if (mProjectileType == ProjectileType::PROJECTILE_MELON)
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(aLastPosX + 30.0f, aLastPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_MELONSPLASH);
		aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(aLastPosX + 30.0f, aLastPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_WINTERMELON);
		aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		int aRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_GROUND, mCobTargetRow, 2);
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 80.0f, mPosY + 40.0f, aRenderOrder, ParticleEffect::PARTICLE_BLASTMARK);
		TodParticleSystem* aParticle2 = mApp->AddTodParticle(mPosX + 80.0f, mPosY + 40.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_POPCORNSPLASH);
		aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
		aParticle2->OverrideFilterEffect(nullptr, mFilterEffect);
		mApp->PlaySample(SOUND_DOOMSHROOM);
		mBoard->ShakeBoard(3, -4);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_PEA || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA)
	{
		aSplatPosX -= 15.0f;
		aEffect = ParticleEffect::PARTICLE_PEA_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
	{
		aSplatPosX -= 15.0f;
		aEffect = ParticleEffect::PARTICLE_SNOWPEA_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		if (IsSplashDamage(theZombie))
		{
			if (mPosX + 38.0f > 710 - mWidth)
			{
				mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_ZOMBIE, mRow, 7);
			}

			Reanimation* aFireReanim = mApp->AddReanimation(mPosX + 38.0f, mPosY - 20.0f, mRenderOrder + 1, ReanimationType::REANIM_JALAPENO_FIRE);
			aFireReanim->mAnimTime = 0.25f;
			aFireReanim->mAnimRate = 24.0f;
			aFireReanim->OverrideScale(0.7f, 0.4f);
			if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
				aFireReanim->mFilterEffect = mFilterEffect;
			}
		}
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_STAR)
	{
		aEffect = ParticleEffect::PARTICLE_STAR_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_PUFF)
	{
		aSplatPosX -= 20.0f;
		aEffect = ParticleEffect::PARTICLE_PUFF_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_CABBAGE)
	{
		aSplatPosX = aLastPosX - 38.0f;
		aSplatPosY = aLastPosY + 23.0f;
		aEffect = ParticleEffect::PARTICLE_CABBAGE_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_KERNEL)
	{
		aSplatPosX = aLastPosX - 38.0f;
		aSplatPosY = aLastPosY + 23.0f;
		aEffect = ParticleEffect::PARTICLE_KERNEL_SPLAT;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
	{
		aSplatPosX = aLastPosX - 20.0f;
		aSplatPosY = aLastPosY + 63.0f;
		aEffect = ParticleEffect::PARTICLE_BUTTER_SPLAT;

		if (theZombie)
		{
			theZombie->ApplyButter();
			if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
				theZombie->mButterFilterEffect = mFilterEffect;
		}
	}

	else if (mProjectileType == ProjectileType::PROJECTILE_SPIKE 
	#ifdef _PIERCING_CACTUS || mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE
	#endif
	)
	{
		aSplatPosX -= 15.0f;
		aEffect = ParticleEffect::PARTICLE_SPIKE_SPLAT;
	}


	if (aEffect != ParticleEffect::PARTICLE_NONE)
	{
		if (theZombie && theZombie->mZombieType != ZOMBIE_BOSS)
		{
			float aPosX = aSplatPosX + 52.0f - theZombie->mX;
			float aPosY = aSplatPosY - theZombie->mY 
				+ theZombie->mAltitude
			;
			if (theZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_WALKING_IN_POOL || theZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_WALKING_IN_POOL)
			{
				aPosY += 60.0f;
			}
			if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
			{
				aPosX -= 80.0f;
			}
			else if (mPosX > theZombie->mX + 40 && mMotionType != ProjectileMotion::MOTION_LOBBED)
			{
				aPosX -= 60.0f;
			}

			aPosY = ClampFloat(aPosY, 20.0f, 100.0f);
			TodParticleSystem* aParticle = theZombie->AddAttachedParticle(aPosX, aPosY, aEffect);
			if (aParticle)
			{
				aParticle->OverrideColor(nullptr, mOverrideColor);
				if (mEnableExtraAdditiveDraw)
				{
					aParticle->OverrideExtraAdditiveColor(nullptr, mExtraAdditiveColor);
					aParticle->OverrideExtraAdditiveDraw(nullptr, true);
				}
				if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
					aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
			}
		}
		else
		{
			if (mMotionType == MOTION_LOBBED) {
				aSplatPosX = aLastPosX;
			}

			TodParticleSystem* aParticle = mApp->AddTodParticle(aSplatPosX, aSplatPosY, mRenderOrder + 1, aEffect);
			if (aParticle)
			{
				aParticle->OverrideColor(nullptr, mOverrideColor);
				if (mEnableExtraAdditiveDraw)
				{
					aParticle->OverrideExtraAdditiveColor(nullptr, mExtraAdditiveColor);
					aParticle->OverrideExtraAdditiveDraw(nullptr, true);
				}
				if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE)
					aParticle->OverrideFilterEffect(nullptr, mFilterEffect);
			}

		}
	}

	if (mHasImpactAnimation)	{
		mIsPlayingImpactAnimation = true;
		AttachmentDie(mAttachmentID);

		mAnimCounter = 0;
		mNumFrames = 2;
		mAnimTicksPerFrame = 12;
		mAnimOffset = 8;

#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		if (mProjectileType == ProjectileType::PROJECTILE_LETTUCE)
		{
			mX -= mVelX * 2 + 48;
			mY -= mVelY * 2 + 53;

			int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
			bool isHighGround = false;
			if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
			{
				mY -= HIGH_GROUND_HEIGHT;
			}
		}
#endif
	}
	else 
	{
#ifdef _PIERCING_CACTUS
		if (mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE && mNumPierced < 2)
			return;
#endif

		Die();
	}
}

//0x46E460
void Projectile::Update()
{
	mProjectileAge++;
	if (mApp->mGameScene != GameScenes::SCENE_PLAYING && !mBoard->mCutScene->ShouldRunUpsellBoard())
		return;

	int aTime = 20;
	if (mProjectileType == ProjectileType::PROJECTILE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA || 
		mProjectileType == ProjectileType::PROJECTILE_CABBAGE || 
		mProjectileType == ProjectileType::PROJECTILE_MELON || 
		mProjectileType == ProjectileType::PROJECTILE_WINTERMELON || 
		mProjectileType == ProjectileType::PROJECTILE_KERNEL || 
		mProjectileType == ProjectileType::PROJECTILE_BUTTER || 
		mProjectileType == ProjectileType::PROJECTILE_COBBIG || 
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SPIKE
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		|| mProjectileType == ProjectileType::PROJECTILE_BEE
#endif
		)
	{
		aTime = 0;
	}
	if (mProjectileAge > aTime)
	{
		mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PROJECTILE, mRow, 0);
	}

	if (mPosX > 710 - mWidth)
	{
		mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_ZOMBIE, mRow, 8);
	}

	if (mClickBackoffCounter > 0)
	{
		mClickBackoffCounter--;
	}
	mRotation += mRotationSpeed;

	if (mIsPlayingImpactAnimation) 
	{
		if (mAnimTicksPerFrame > 0)
		{
			mAnimCounter = (mAnimCounter + 1) % (mNumFrames * mAnimTicksPerFrame);
			mFrame = mAnimOffset + mAnimCounter / mAnimTicksPerFrame;
			
			if (mAnimCounter == 0) 
			{
				Die();
			}
		}
	}
	else 
	{
		UpdateMotion();
		AttachmentUpdateAndMove(mAttachmentID, mPosX, mPosY + mPosZ);
	}
}

//0x46E540
void Projectile::Draw(Graphics* g)
{
	Graphics gProj(*g);
	gProj.SetColorizeImages(true);
	gProj.SetColor(mOverrideColor);

	const ProjectileDefinition& aProjectileDef = GetProjectileDef();

	Image* aImage;
	float aScaleX = 1.0f;
	float aScaleY = 1.0f;
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		aImage = IMAGE_REANIM_COBCANNON_COB;
		aScaleX = aScaleY = 0.9f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_PEA || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA)
	{
		aImage = IMAGE_PROJECTILEPEA;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
	{
		aImage = IMAGE_PROJECTILESNOWPEA;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		aImage = nullptr;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SPIKE
#ifdef _PIERCING_CACTUS
		|| mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE
#endif
		)
	{
		aImage = IMAGE_PROJECTILECACTUS;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_STAR)
	{
		aImage = IMAGE_PROJECTILE_STAR;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_PUFF)
	{
		aImage = IMAGE_PUFFSHROOM_PUFF1;
		aScaleX = aScaleY = TodAnimateCurveFloat(0, 30, mProjectileAge, 0.3f, 1.0f, TodCurves::CURVE_LINEAR);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL)
	{
		aImage = IMAGE_REANIM_ZOMBIE_CATAPULT_BASKETBALL;
		aScaleX = aScaleY = 1.1f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_CABBAGE)
	{
		aImage = IMAGE_REANIM_CABBAGEPULT_CABBAGE;
		aScaleX = aScaleY = 1.0f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_KERNEL)
	{
		aImage = IMAGE_REANIM_CORNPULT_KERNAL;
		aScaleX = aScaleY = 0.95f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
	{
		aImage = IMAGE_REANIM_CORNPULT_BUTTER;
		aScaleX = aScaleY = 0.8f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON)
	{
		aImage = IMAGE_REANIM_MELONPULT_MELON;
		aScaleX = aScaleY = 1.0f;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
	{
		aImage = IMAGE_REANIM_WINTERMELON_PROJECTILE;
		aScaleX = aScaleY = 1.0f;
	}
	else
	{
		//TOD_ASSERT();
		aImage = IMAGE_PROJECTILE;
		aScaleX = aScaleY = 1.0f;
	}

	if (aImage && mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
		aImage = FilterEffectGetImage(aImage, mFilterEffect);
	}

	bool aMirror = false;
	if (mMotionType == ProjectileMotion::MOTION_BEE_BACKWARDS)
	{
		aMirror = true;
	}

	if (aImage)
	{
		TOD_ASSERT(aProjectileDef.mImageRow < aImage->mNumRows);
		TOD_ASSERT(mFrame < aImage->mNumCols);

		int aCelWidth = aImage->GetCelWidth();
		int aCelHeight = aImage->GetCelHeight();
		Rect aSrcRect(aCelWidth * mFrame, aCelHeight * aProjectileDef.mImageRow, aCelWidth, aCelHeight);
		if (FloatApproxEqual(mRotation, 0.0f) && FloatApproxEqual(aScaleX, 1.0f) && FloatApproxEqual(aScaleY, 1.0f))
		{
			Rect aDestRect(0, 0, aCelWidth, aCelHeight);
			gProj.DrawImageMirror(aImage, aDestRect, aSrcRect, aMirror);
		}
		else
		{
			float aOffsetX = mPosX + aCelWidth * 0.5f;
			float aOffsetY = mPosZ + mPosY + aCelHeight * 0.5f;
			if (aMirror) 
				aScaleX *= -1;

			SexyTransform2D aTransform;
			TodScaleRotateTransformMatrix(aTransform, aOffsetX + mBoard->mX + (g->mTransX - mX), aOffsetY + mBoard->mY + (g->mTransY - mY), mRotation, aScaleX, aScaleY);
			TodBltMatrix(&gProj, aImage, aTransform, gProj.mClipRect, gProj.mColor, gProj.mDrawMode, aSrcRect);
		}
	}

	if (mAttachmentID != AttachmentID::ATTACHMENTID_NULL)
	{
		Graphics theParticleGraphics(gProj);
		MakeParentGraphicsFrame(&theParticleGraphics);
		AttachmentDraw(mAttachmentID, &theParticleGraphics, false);
	}

	if (mEnableExtraAdditiveDraw) {
		gProj.SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		gProj.SetColor(mExtraAdditiveColor);

		if (aImage)
		{
			TOD_ASSERT(aProjectileDef.mImageRow < aImage->mNumRows);
			TOD_ASSERT(mFrame < aImage->mNumCols);

			int aCelWidth = aImage->GetCelWidth();
			int aCelHeight = aImage->GetCelHeight();
			Rect aSrcRect(aCelWidth * mFrame, aCelHeight * aProjectileDef.mImageRow, aCelWidth, aCelHeight);
			if (FloatApproxEqual(mRotation, 0.0f) && FloatApproxEqual(aScaleX, 1.0f) && FloatApproxEqual(aScaleY, 1.0f))
			{
				Rect aDestRect(0, 0, aCelWidth, aCelHeight);
				gProj.DrawImageMirror(aImage, aDestRect, aSrcRect, aMirror);
			}
			else
			{
				float aOffsetX = mPosX + aCelWidth * 0.5f;
				float aOffsetY = mPosZ + mPosY + aCelHeight * 0.5f;

				if (aMirror)
					aScaleX *= -1;

				SexyTransform2D aTransform;
				TodScaleRotateTransformMatrix(aTransform, aOffsetX + mBoard->mX, aOffsetY + mBoard->mY, mRotation, aScaleX, aScaleY);
				TodBltMatrix(&gProj, aImage, aTransform, gProj.mClipRect, gProj.mColor, gProj.mDrawMode, aSrcRect);
			}
		}

		if (mAttachmentID != AttachmentID::ATTACHMENTID_NULL)
		{
			Graphics theParticleGraphics(gProj);
			MakeParentGraphicsFrame(&theParticleGraphics);
			AttachmentDraw(mAttachmentID, &theParticleGraphics, false);
		}
	}
}

//0x46E8C0
void Projectile::DrawShadow(Graphics* g)
{
	if (mIsPlayingImpactAnimation)	return;

	int aCelCol = 0;
	float aScaleX = 1.0f;
	float aScaleY = 1.0f;
	float aStretch = 1.0f;
	float aOffsetX = mPosX - mX;
	float aOffsetY = mPosY - mY;

	int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
	bool isHighGround = false;
	if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
	{
		isHighGround = true;
	}
	if (mOnHighGround && !isHighGround)
	{
		aOffsetY += HIGH_GROUND_HEIGHT;
	}
	else if (!mOnHighGround && isHighGround)
	{
		aOffsetY -= HIGH_GROUND_HEIGHT;
	}

	if (mBoard->StageIsNight())
	{
		aCelCol = 1;
	}

	switch (mProjectileType)
	{
	case ProjectileType::PROJECTILE_PEA:
	case ProjectileType::PROJECTILE_ZOMBIE_PEA:
		aOffsetX += 3.0f;
		break;

	case ProjectileType::PROJECTILE_SNOWPEA:
		aOffsetX += -1.0f;
		aScaleX = aScaleY = 1.3f;
		break;

	case ProjectileType::PROJECTILE_STAR:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	case ProjectileType::PROJECTILE_BEE:
#endif
		aOffsetX += 7.0f;
		break;

	case ProjectileType::PROJECTILE_CABBAGE:
	case ProjectileType::PROJECTILE_KERNEL:
	case ProjectileType::PROJECTILE_BUTTER:
	case ProjectileType::PROJECTILE_MELON:
	case ProjectileType::PROJECTILE_WINTERMELON:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	case ProjectileType::PROJECTILE_LETTUCE:
#endif
		aOffsetX += 3.0f;
		aOffsetY += 10.0f;
		aScaleX = aScaleY = 1.6f;
		break;

	case ProjectileType::PROJECTILE_PUFF:
		return;
		
	case ProjectileType::PROJECTILE_COBBIG:
		aScaleX = aScaleY = 1.0f;
		aStretch = 3.0f;
		aOffsetX += 57.0f;
		break;

	case ProjectileType::PROJECTILE_FIREBALL:
		aScaleX = aScaleY = 1.4f;
		break;
	}

	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		float aHeight = ClampFloat(-mPosZ, 0.0f, 200.0f);
		aScaleX = aScaleY *= 200.0f / (aHeight + 200.0f);
	}

	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG && mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		float aShadowX = mCobTargetX;
		float aShadowY = mBoard->GridToPixelY(mBoard->PixelToGridXKeepOnBoard(mCobTargetX, 0), mCobTargetRow);

		float aGroundZ = 80.0f;
		if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
		{
			aGroundZ = -40.0f;
		}
		int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
		bool isHighGround = false;
		if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
		{
			aGroundZ -= HIGH_GROUND_HEIGHT;
		}

		g->PushState();
		g->mTransX -= mX;
		g->mTransY -= mY;
		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		if (mVelZ < 0)
			g->mColor.mAlpha = 255 * TodAnimateCurveFloatTime(0, -GetProjectileRect().mHeight - (mBoard->StageHas6Rows() || mBoard->StageHasRoof() ? 80.0f : 100.0f) - Sexy::IMAGE_REANIM_COBCANNON_COB->GetHeight(), mPosZ, 0.0f, 1.0f, TodCurves::CURVE_LINEAR);
		g->DrawImageF(IMAGE_COBCANNON_TARGET_SHADOW, aShadowX - 15.0f + 57.0f, aShadowY - 37.0f + (mBoard->StageHas6Rows() || mBoard->StageHasRoof() ? 80.0f : 100.0f));
		g->PopState();
	}

	TodDrawImageCelScaledF(g, IMAGE_PEA_SHADOWS, aOffsetX, (mShadowY - mPosY + aOffsetY), aCelCol, 0, aScaleX * aStretch, aScaleY);
}

//0x46EB20
void Projectile::Die()
{
	mDead = true;

	if (mProjectileType == ProjectileType::PROJECTILE_PUFF || mProjectileType == ProjectileType::PROJECTILE_SNOWPEA)
	{
		AttachmentCrossFade(mAttachmentID, "FadeOut");
		AttachmentDetach(mAttachmentID);
	}
	else
	{
		AttachmentDie(mAttachmentID);
	}
}

//0x46EBC0
Rect Projectile::GetProjectileRect()
{
	if (mProjectileType == ProjectileType::PROJECTILE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA 
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
		|| mProjectileType == ProjectileType::PROJECTILE_BEE
#endif
		)
	{
		return Rect(mX - 15, mY, mWidth + 15, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		return Rect(mX + mWidth / 2 - 115, mY + mHeight / 2 - 115, 230, 230);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
	{
		return Rect(mX + 20, mY, 60, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		return Rect(mX, mY, mWidth - 10, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SPIKE
#ifdef _PIERCING_CACTUS
		|| mProjectileType == ProjectileType::PROJECTILE_PIERCE_SPIKE
#endif
		)
	{
		return Rect(mX - 25, mY, mWidth + 25, mHeight);
	}
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	else if (mProjectileType == ProjectileType::PROJECTILE_LETTUCE)
	{
		return Rect(mX + 10, mY + 10, mWidth, mHeight);
	}
#endif
	else
	{
		return Rect(mX, mY, mWidth, mHeight);
	}
}

//0x46ECB0
void Projectile::ConvertToFireball(int theGridX)
{
	if (mHitTorchwoodGridX == theGridX)
		return;

	mProjectileType = ProjectileType::PROJECTILE_FIREBALL;
	mHitTorchwoodGridX = theGridX;
	mApp->PlayFoley(FoleyType::FOLEY_FIREPEA);

	float aOffsetX = -25.0f;
	float aOffsetY = -25.0f;
	Reanimation* aFirePeaReanim = mApp->AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_FIRE_PEA);
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
	{
		aFirePeaReanim->OverrideScale(-1.0f, 1.0f);
		aOffsetX += 80.0f;
	}
	aFirePeaReanim->SetPosition(mPosX + aOffsetX, mPosY + aOffsetY);
	aFirePeaReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
	aFirePeaReanim->mAnimRate = RandRangeFloat(50.0f, 80.0f);
	if (mFilterEffect != FilterEffect::FILTER_EFFECT_NONE) {
		aFirePeaReanim->mFilterEffect = mFilterEffect;
	}
	AttachReanim(mAttachmentID, aFirePeaReanim, aOffsetX, aOffsetY);
}

//0x46EE00
void Projectile::ConvertToPea(int theGridX)
{
	if (mHitTorchwoodGridX == theGridX)
		return;

	AttachmentDie(mAttachmentID);
	mProjectileType = ProjectileType::PROJECTILE_PEA;
	mHitTorchwoodGridX = theGridX;
	mApp->PlayFoley(FoleyType::FOLEY_THROW);
}

ProjectileDefinition& Projectile::GetProjectileDef()
{
	ProjectileDefinition& aProjectileDef = gProjectileDefinition[(int)mProjectileType];
	TOD_ASSERT(aProjectileDef.mProjectileType == mProjectileType);

	return aProjectileDef;
}

void Projectile::OverrideAngle(float theAngle)
{
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		mRotation = PI / 2;
	}

	mRotation += theAngle;
}
