#include "../Plant.h"
#include "../Zombie.h"
#include "../../LawnApp.h"
#include "ReanimationLawn.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../SexyAppFramework/Color.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../SexyAppFramework/MemoryImage.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/FilterEffect.h"
#include "../../Sexy.TodLib/Attachment.h"
#include "../../Sexy.TodLib/EffectSystem.h"

//0x46EF00
void ReanimatorCache::UpdateReanimationForVariation(Reanimation* theReanim, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation)
{
	if (theDrawVariation >= DrawVariation::VARIATION_MARIGOLD_WHITE && theDrawVariation <= DrawVariation::VARIATION_MARIGOLD_LIGHT_GREEN)
	{
		int aVariationIndex = (int)theDrawVariation - (int)DrawVariation::VARIATION_MARIGOLD_WHITE;
		Color MARIGOLD_VARIATIONS[] = {
			Color(255, 255, 255),
			Color(230, 30, 195),
			Color(250, 125, 5),
			Color(255, 145, 215),
			Color(160, 255, 245),
			Color(230, 30, 30),
			Color(5, 130, 255),
			Color(195, 55, 235),
			Color(235, 210, 255),
			Color(255, 245, 55),
			Color(180, 255, 105)
		};

		TOD_ASSERT(aVariationIndex >= 0 && aVariationIndex < LENGTH(MARIGOLD_VARIATIONS));
		theReanim->GetTrackInstanceByName("Marigold_petals")->mTrackColor = MARIGOLD_VARIATIONS[aVariationIndex];
	}
	else
	{
		switch (theDrawVariation)
		{
		/*case DrawVariation::VARIATION_IMITATER:
		{
			theReanim->mFilterEffect = FilterEffect::FILTER_EFFECT_WASHED_OUT;
			break;
		}	
		case DrawVariation::VARIATION_IMITATER_LESS:
		{
			theReanim->mFilterEffect = FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT;
			break;
		}*/
		case DrawVariation::VARIATION_ZEN_GARDEN:
			theReanim->SetFramesForLayer("anim_zengarden");
			break;
		case DrawVariation::VARIATION_ZEN_GARDEN_WATER:
			theReanim->SetFramesForLayer("anim_waterplants");
			break;
		case DrawVariation::VARIATION_AQUARIUM:
			theReanim->SetFramesForLayer("anim_idle_aquarium");
			break;
		case DrawVariation::VARIATION_SPROUT_NO_FLOWER:
			theReanim->SetFramesForLayer("anim_idle_noflower");
			break;
		case DrawVariation::VARIATION_PUMPKINSHELL_FRONT:
			theReanim->AssignRenderGroupToPrefix("Pumpkin_back", RENDER_GROUP_HIDDEN);
			break;
		case DrawVariation::VARIATION_PUMPKINSHELL_BACK:
			theReanim->AssignRenderGroupToPrefix("Pumpkin_front", RENDER_GROUP_HIDDEN);
			break;
		case DrawVariation::VARIATION_NORMAL_DEGRADE1:
		case DrawVariation::VARIATION_NORMAL_DEGRADE2:
		{
			if (theReanim->mReanimationType == REANIM_WALLNUT)
			{
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 || theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE2) {
					Image* aOverride = theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 ? IMAGE_REANIM_WALLNUT_CRACKED1 : IMAGE_REANIM_WALLNUT_CRACKED2;
					theReanim->SetImageOverride("anim_face", aOverride);
				}
			}
			else if (theReanim->mReanimationType == REANIM_TALLNUT)
			{
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 || theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE2) {
					Image* aOverride = theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 ? IMAGE_REANIM_TALLNUT_CRACKED1 : IMAGE_REANIM_TALLNUT_CRACKED2;
					theReanim->SetImageOverride("anim_idle", aOverride);
				}
			}
			else if (theReanim->mReanimationType == REANIM_PUMPKIN)
			{
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 || theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE2) {
					Image* aOverride = theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 ? IMAGE_REANIM_PUMPKIN_DAMAGE1 : IMAGE_REANIM_PUMPKIN_DAMAGE3;
					theReanim->SetImageOverride("Pumpkin_front", aOverride);
				}
			}
			else if (theReanim->mReanimationType == REANIM_GARLIC)
			{
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 || theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE2) {
					Image* aOverride = theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1 ? IMAGE_REANIM_GARLIC_BODY2 : IMAGE_REANIM_GARLIC_BODY3;
					theReanim->SetImageOverride("anim_face", aOverride);
				}
			}
			else if (theReanim->mReanimationType == REANIM_SPIKEROCK)
			{
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE1)
				{
					theReanim->AssignRenderGroupToTrack("bigspike2", RENDER_GROUP_HIDDEN);
					theReanim->AssignRenderGroupToTrack("bigspike3", RENDER_GROUP_HIDDEN);
				}
				if (theDrawVariation == DrawVariation::VARIATION_NORMAL_DEGRADE2)
				{
					theReanim->AssignRenderGroupToTrack("bigspike1", RENDER_GROUP_HIDDEN);
					theReanim->AssignRenderGroupToTrack("bigspike2", RENDER_GROUP_HIDDEN);
					theReanim->AssignRenderGroupToTrack("bigspike3", RENDER_GROUP_HIDDEN);
				}
			}
			break;
		}
		case DrawVariation::VARIATION_KERNELPULT_WITH_BUTTER:
		case DrawVariation::VARIATION_SUNSHROOM_BIG:
			break;
		case DrawVariation::VARIATION_PUMPKINSHELL_FRONT_DEGRADE1:
		case DrawVariation::VARIATION_PUMPKINSHELL_FRONT_DEGRADE2:
		{
			theReanim->AssignRenderGroupToPrefix("Pumpkin_back", RENDER_GROUP_HIDDEN);
			Image* aOverride = theDrawVariation == DrawVariation::VARIATION_PUMPKINSHELL_FRONT_DEGRADE1 ? IMAGE_REANIM_PUMPKIN_DAMAGE1 : IMAGE_REANIM_PUMPKIN_DAMAGE3;
			theReanim->SetImageOverride("Pumpkin_front", aOverride);
			break;
		}
		default:
			TOD_ASSERT(false);
			break;
		}
	}
}

//0x46F100
void ReanimatorCache::DrawReanimatorFrame(Graphics* g, float thePosX, float thePosY, ReanimationType theReanimationType, const char* theTrackName, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation, unsigned int theDrawBitVariation)
{
	Reanimation aReanim;
	aReanim.ReanimationInitializeType(thePosX, thePosY, theReanimationType);

	if (theTrackName != nullptr && aReanim.TrackExists(theTrackName))
	{
		aReanim.SetFramesForLayer(theTrackName);
	}

	if (theReanimationType == ReanimationType::REANIM_KERNELPULT && theDrawVariation != DrawVariation::VARIATION_KERNELPULT_WITH_BUTTER)
	{
		aReanim.AssignRenderGroupToTrack("Cornpult_butter", RENDER_GROUP_HIDDEN);
	}
	else if (theReanimationType == ReanimationType::REANIM_SUNFLOWER)
	{
		aReanim.mAnimTime = 0.15f;
	}
	aReanim.AssignRenderGroupToTrack("anim_waterline", RENDER_GROUP_HIDDEN);

	if (g->GetColorizeImages())
	{
		aReanim.mColorOverride = g->GetColor();
	}
	aReanim.OverrideScale(g->mScaleX, g->mScaleY);
	
	if (theDrawVariation != DrawVariation::VARIATION_NORMAL)
	{
		UpdateReanimationForVariation(&aReanim, theDrawVariation);
	}

	if (theFilterVariation != DrawFilterVariation::FILTERVARIATION_NONE)
	{
		switch (theFilterVariation)
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

	if (TestBit(theDrawBitVariation, (int)DrawBitVariation::BITVARIATION_IZOMBIE))
	{
		g->SetColorizeImages(true);
		aReanim.mFilterEffect = FilterEffect::FILTER_EFFECT_WHITE;

		float aOffsetX = g->mTransX, aOffsetY = g->mTransY;
		g->mTransX = aOffsetX + 4;
		g->mTransY = aOffsetY + 4;
		g->SetColor(Color(122, 86, 58));
		aReanim.Draw(g);

		g->mTransX = aOffsetX + 2;
		g->mTransY = aOffsetY + 2;
		g->SetColor(Color(171, 135, 107));
		aReanim.Draw(g);

		g->mTransX = aOffsetX - 2;
		g->mTransY = aOffsetY - 2;
		g->SetColor(Color(171, 135, 107));
		aReanim.Draw(g);

		g->mTransX = aOffsetX;
		g->mTransY = aOffsetY;
		g->SetColor(Color(255, 201, 160));
		aReanim.mFilterEffect = FilterEffect::FILTER_EFFECT_NONE;
		aReanim.Draw(g);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
	else
	{
		aReanim.Draw(g);
	}
}

//0x46F280
SDL3Image* ReanimatorCache::MakeBlankSDL3Image(int theWidth, int theHeight)
{
	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;
	theWidth *= scale;
	theHeight *= scale;

	SDL3Image* aImage = new SDL3Image(LawnApp::mSDLRenderer);
	aImage->mWidth = theWidth;
	aImage->mHeight = theHeight;
	aImage->mD3DData = SDL3Image::CreateRenderTarget(LawnApp::mSDLRenderer, aImage->mWidth, aImage->mHeight);
	SDL_Texture* aTexture = (SDL_Texture*)aImage->mD3DData;
	SDL_SetTextureBlendMode(aTexture, SDL_BLENDMODE_BLEND);
	SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);
	SDL_SetRenderTarget(LawnApp::mSDLRenderer, aTexture);
	SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(LawnApp::mSDLRenderer);
	SDL_SetRenderTarget(LawnApp::mSDLRenderer, anOldRenderTarget);
	return aImage;
}

MemoryImage* ReanimatorCache::ConvertSDL3ImageToMemoryImage(SDL3Image* theImage)
{
	MemoryImage* aImage = new MemoryImage();
	int aBitsCount = theImage->mWidth * theImage->mHeight;
	aImage->mWidth = theImage->mWidth;
	aImage->mHeight = theImage->mHeight;
	aImage->mBits = new unsigned long[aBitsCount + 1];
	aImage->mHasTrans = true;
	aImage->mHasAlpha = true;
	memset(aImage->mBits, 0, aBitsCount * sizeof(unsigned long));
	aImage->mBits[aBitsCount] = Sexy::MEMORYCHECK_ID;

	SDL_Texture* oldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);
	SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)theImage->mD3DData);

	SDL_Surface* surface = SDL_RenderReadPixels(LawnApp::mSDLRenderer, NULL);
	uint8_t* srcBytes = static_cast<uint8_t*>(surface->pixels);

	for (int y = 0; y < theImage->mHeight; ++y)
	{
		memcpy(aImage->mBits + y * theImage->mWidth, srcBytes + y * surface->pitch, theImage->mWidth * 4);
	}

	SDL_SetRenderTarget(LawnApp::mSDLRenderer, oldRenderTarget);
	SDL_DestroySurface(surface);

	delete theImage; // gonna dispose the SDL3Image
	return aImage;
}

void ReanimatorCache::GetPlantImageSize(SeedType theSeedType, int& theOffsetX, int& theOffsetY, int& theWidth, int& theHeight)
{
	theOffsetX = -20;
	theOffsetY = -20;
	theWidth = 120;
	theHeight = 120;

	if (theSeedType == SeedType::SEED_TALLNUT)
	{
		theOffsetY = -40;
		theHeight += 40;
	}
	else if (theSeedType == SeedType::SEED_MELONPULT || theSeedType == SeedType::SEED_WINTERMELON)
	{
		theOffsetX = -40;
		theWidth += 40;

		if (theSeedType == SeedType::SEED_WINTERMELON) {
			theOffsetY -= 10;
			theHeight += 10;
		}
	}
	else if (theSeedType == SeedType::SEED_COBCANNON)
	{
		theWidth += 80;
	}
}

//0x46F330
MemoryImage* ReanimatorCache::MakeCachedMowerFrame(LawnMowerType theMowerType)
{
	SDL3Image* aImage;
	SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);

	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;

	switch (theMowerType)
	{
	case LawnMowerType::LAWNMOWER_LAWN:
	{
		aImage = MakeBlankSDL3Image(90 * scale, 100 * scale);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aImage->mD3DData);
		SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(LawnApp::mSDLRenderer);
		Graphics aMemoryGraphics(aImage);
		aMemoryGraphics.SetLinearBlend(true);
		aMemoryGraphics.mScaleX = 0.85f * scale;
		aMemoryGraphics.mScaleY = 0.85f * scale;
		DrawReanimatorFrame(&aMemoryGraphics, 10.0f * scale, 0.0f, ReanimationType::REANIM_LAWNMOWER, "anim_normal", DrawVariation::VARIATION_NORMAL);
		break;
	}
	case LawnMowerType::LAWNMOWER_POOL:
	{
		aImage = MakeBlankSDL3Image(90 * scale, 100 * scale);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aImage->mD3DData);
		SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(LawnApp::mSDLRenderer);
		Graphics aMemoryGraphics(aImage);
		aMemoryGraphics.SetLinearBlend(true);
		aMemoryGraphics.mScaleX = 0.8f * scale;
		aMemoryGraphics.mScaleY = 0.8f * scale;
		DrawReanimatorFrame(&aMemoryGraphics, 10.0f * scale, 25.0f * scale, ReanimationType::REANIM_POOL_CLEANER, nullptr, DrawVariation::VARIATION_NORMAL);
		break;
	}
	case LawnMowerType::LAWNMOWER_ROOF:
	{
		aImage = MakeBlankSDL3Image(90 * scale, 100 * scale);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aImage->mD3DData);
		SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(LawnApp::mSDLRenderer);
		Graphics aMemoryGraphics(aImage);
		aMemoryGraphics.SetLinearBlend(true);
		aMemoryGraphics.mScaleX = 0.85f * scale;
		aMemoryGraphics.mScaleY = 0.85f * scale;
		DrawReanimatorFrame(&aMemoryGraphics, 10.0f * scale, 0.0f , ReanimationType::REANIM_ROOF_CLEANER, nullptr, DrawVariation::VARIATION_NORMAL);
		break;
	}
	case LawnMowerType::LAWNMOWER_SUPER_MOWER:
	{
		aImage = MakeBlankSDL3Image(90 * scale, 100 * scale);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aImage->mD3DData);
		SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(LawnApp::mSDLRenderer);
		Graphics aMemoryGraphics(aImage);
		aMemoryGraphics.SetLinearBlend(true);
		aMemoryGraphics.mScaleX = 0.85f * scale;
		aMemoryGraphics.mScaleY = 0.85f * scale;
		DrawReanimatorFrame(&aMemoryGraphics, 10.0f * scale, 0.0f, ReanimationType::REANIM_LAWNMOWER, "anim_tricked", DrawVariation::VARIATION_NORMAL);
		break;
	}
	default:
		TOD_ASSERT(false);
		break;
	}

	SDL_SetRenderTarget(LawnApp::mSDLRenderer, anOldRenderTarget);

	return ConvertSDL3ImageToMemoryImage(aImage);
}

//0x46F550
MemoryImage* ReanimatorCache::MakeCachedPlantFrame(SeedType theSeedType, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation, unsigned int theDrawBitVariation)
{
	int aOffsetX, aOffsetY, aWidth, aHeight;
	GetPlantImageSize(theSeedType, aOffsetX, aOffsetY, aWidth, aHeight);

	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;
	aOffsetX *= scale;
	aOffsetY *= scale;
	aWidth *= scale;
	aHeight *= scale;

	SDL3Image* aMemoryImage = MakeBlankSDL3Image(aWidth, aHeight);

	SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);
	SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aMemoryImage->mD3DData);
	SDL_SetRenderDrawColor(LawnApp::mSDLRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(LawnApp::mSDLRenderer);

	Graphics aMemoryGraphics(aMemoryImage);
	aMemoryGraphics.SetLinearBlend(true);

	aMemoryGraphics.mScaleX *= scale;
	aMemoryGraphics.mScaleY *= scale;

	PlantDefinition& aPlantDef = GetPlantDefinition(theSeedType);
	TOD_ASSERT(aPlantDef.mReanimationType != ReanimationType::REANIM_NONE);

	if (theSeedType == SeedType::SEED_POTATOMINE)
	{
		aMemoryGraphics.mScaleX *= 0.85f;
		aMemoryGraphics.mScaleY *= 0.85f;
		DrawReanimatorFrame(&aMemoryGraphics, -(int)(aOffsetX - 12.0f * scale), -(int)(aOffsetY - 12.0f * scale), aPlantDef.mReanimationType, "anim_armed", theDrawVariation, theFilterVariation, theDrawBitVariation);
	}
	else if (theSeedType == SeedType::SEED_INSTANT_COFFEE)
	{
		aMemoryGraphics.mScaleX *= 0.8f;
		aMemoryGraphics.mScaleY *= 0.8f;
		DrawReanimatorFrame(&aMemoryGraphics, -(int)(aOffsetX - 12.0f * scale), -(int)(aOffsetY - 12.0f * scale), aPlantDef.mReanimationType, "anim_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);
	}
	else if (theSeedType == SeedType::SEED_EXPLODE_O_NUT)
	{
		aMemoryGraphics.SetColorizeImages(true);
		aMemoryGraphics.SetColor(Color(255, 64, 64));
		DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);
	}
	else if (theDrawVariation == DrawVariation::VARIATION_SUNSHROOM_BIG)
	{
		DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_bigidle", theDrawVariation, theFilterVariation, theDrawBitVariation);
	}
	else
	{
		DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);

		if (theSeedType == SeedType::SEED_PEASHOOTER || theSeedType == SeedType::SEED_SNOWPEA || theSeedType == SeedType::SEED_REPEATER ||
			theSeedType == SeedType::SEED_LEFTPEATER || theSeedType == SeedType::SEED_GATLINGPEA)
		{
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_head_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);
		}
		else if (theSeedType == SeedType::SEED_SPLITPEA)
		{
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_head_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_splitpea_idle", theDrawVariation, theFilterVariation, theDrawBitVariation);
		}
		else if (theSeedType == SeedType::SEED_THREEPEATER)
		{
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_head_idle1", theDrawVariation, theFilterVariation, theDrawBitVariation);
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_head_idle3", theDrawVariation, theFilterVariation, theDrawBitVariation);
			DrawReanimatorFrame(&aMemoryGraphics, -aOffsetX, -aOffsetY, aPlantDef.mReanimationType, "anim_head_idle2", theDrawVariation, theFilterVariation, theDrawBitVariation);
		}
	}

	SDL_SetRenderTarget(LawnApp::mSDLRenderer, anOldRenderTarget);

	return ConvertSDL3ImageToMemoryImage(aMemoryImage);
}

//0x46F8A0
MemoryImage* ReanimatorCache::MakeCachedZombieFrame(ZombieType theZombieType)
{
	int maxWidth = 200;
	int maxHeight = 210;

	if (theZombieType == ZombieType::ZOMBIE_ZAMBONI)	maxWidth = 300;
	if (theZombieType == ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE)	maxWidth = 250;
	if (theZombieType == ZombieType::ZOMBIE_BUNGEE) maxHeight = 810;

	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;
	maxWidth *= scale;
	maxHeight *= scale;

	SDL3Image* aMemoryImage = MakeBlankSDL3Image(maxWidth, maxHeight);
	SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);
	SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aMemoryImage->mD3DData);
	Graphics aMemoryGraphics(aMemoryImage);
	aMemoryGraphics.SetLinearBlend(true);

	ZombieType aUseZombieType = theZombieType;
	if (theZombieType == ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE)
	{
		aUseZombieType = ZombieType::ZOMBIE_POLEVAULTER;
	}
	ZombieDefinition& aZombieDef = GetZombieDefinition(aUseZombieType);
	TOD_ASSERT(aZombieDef.mReanimationType != ReanimationType::REANIM_NONE);

	float aPosX = 40.0f * scale, aPosY = 40.0f * scale;
	if (theZombieType == ZombieType::ZOMBIE_PEA_HEAD || theZombieType == ZombieType::ZOMBIE_GATLING_HEAD || theZombieType == ZombieType::ZOMBIE_SQUASH_HEAD)
	{
		Reanimation aReanim;
		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.OverrideScale(scale, scale);
		aReanim.mIsAttachment = true;
		aReanim.SetFramesForLayer("anim_idle");
		Zombie::SetupReanimLayers(&aReanim, aUseZombieType);
		aReanim.AssignRenderGroupToPrefix("anim_hair", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToPrefix("anim_head2", RENDER_GROUP_HIDDEN);

		ReanimatorTrackInstance* aTrackInstance = aReanim.GetTrackInstanceByName("anim_head1");
		aTrackInstance->mImageOverride = IMAGE_BLANK;

		ReanimationType aHeadType;

		if (theZombieType == ZombieType::ZOMBIE_PEA_HEAD)	aHeadType = ReanimationType::REANIM_PEASHOOTER;
		else if (theZombieType == ZombieType::ZOMBIE_GATLING_HEAD)	aHeadType = ReanimationType::REANIM_GATLINGPEA;
		else if (theZombieType == ZombieType::ZOMBIE_SQUASH_HEAD)	aHeadType = ReanimationType::REANIM_SQUASH;

		Reanimation aHeadReanim;
		aHeadReanim.ReanimationInitializeType(0, 0, aHeadType);
		if (theZombieType == ZombieType::ZOMBIE_SQUASH_HEAD) aHeadReanim.PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 0, 15.0f);
		else aHeadReanim.PlayReanim("anim_head_idle", ReanimLoopType::REANIM_LOOP, 0, 15.0f);

		AttachEffect* aAttachEffect = AttachReanim(aTrackInstance->mAttachmentID, &aHeadReanim, 0.0f, 0.0f);
		aReanim.mFrameBasePose = 0;

		if (theZombieType == ZombieType::ZOMBIE_SQUASH_HEAD) TodScaleRotateTransformMatrix(aAttachEffect->mOffset, 55.0f, -15.0f, 0.2f, -0.75f, 0.75f);
		else TodScaleRotateTransformMatrix(aAttachEffect->mOffset, 65.0f, -5.0f, 0.2f, -1.0f, 1.0f);

		SexyTransform2D aOverlayMatrix;
		aReanim.GetAttachmentOverlayMatrix(aReanim.FindTrackIndex("anim_head1"), aOverlayMatrix);
		AttachmentUpdateAndSetMatrix(aTrackInstance->mAttachmentID, aOverlayMatrix);

		aReanim.Draw(&aMemoryGraphics);
	}
	else if (theZombieType == ZombieType::ZOMBIE_WALLNUT_HEAD || theZombieType == ZombieType::ZOMBIE_TALLNUT_HEAD || theZombieType == ZombieType::ZOMBIE_JALAPENO_HEAD)
	{
		Reanimation aReanim;
		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.OverrideScale(scale, scale);
		aReanim.mIsAttachment = true;
		aReanim.SetFramesForLayer("anim_idle");
		Zombie::SetupReanimLayers(&aReanim, aUseZombieType);
		aReanim.AssignRenderGroupToPrefix("anim_hair", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToPrefix("anim_head", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToPrefix("anim_head2", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToPrefix("Zombie_tie", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToPrefix("Zombie_zombotany_tie", RENDER_GROUP_NORMAL);

		ReanimatorTrackInstance* aTrackInstance = aReanim.GetTrackInstanceByName("Zombie_zombotany_body");
		aTrackInstance->mImageOverride = IMAGE_BLANK;

		ReanimationType aHeadType;

		if (theZombieType == ZombieType::ZOMBIE_WALLNUT_HEAD)	aHeadType = ReanimationType::REANIM_WALLNUT;
		else if (theZombieType == ZombieType::ZOMBIE_TALLNUT_HEAD)	aHeadType = ReanimationType::REANIM_TALLNUT;
		else if (theZombieType == ZombieType::ZOMBIE_JALAPENO_HEAD)	aHeadType = ReanimationType::REANIM_JALAPENO;

		Reanimation aHeadReanim;
		aHeadReanim.ReanimationInitializeType(0, 0, aHeadType);
		aHeadReanim.PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 0, 15.0f);

		AttachEffect* aAttachEffect = AttachReanim(aTrackInstance->mAttachmentID, &aHeadReanim, 0.0f, 0.0f);
		aReanim.mFrameBasePose = 0;

		if (theZombieType == ZombieType::ZOMBIE_WALLNUT_HEAD) TodScaleRotateTransformMatrix(aAttachEffect->mOffset, 50.0f, 0.0f, 0.2f, -0.8f, 0.8f);
		else if (theZombieType == ZombieType::ZOMBIE_TALLNUT_HEAD) TodScaleRotateTransformMatrix(aAttachEffect->mOffset, 37.0f, 0.0f, 0.2f, -0.8f, 0.8f);
		else if (theZombieType == ZombieType::ZOMBIE_JALAPENO_HEAD) TodScaleRotateTransformMatrix(aAttachEffect->mOffset, 55.0f, -5.0f, 0.2f, -1.0f, 1.0f);

		SexyTransform2D aOverlayMatrix;
		aReanim.GetAttachmentOverlayMatrix(aReanim.FindTrackIndex("Zombie_zombotany_body"), aOverlayMatrix);
		AttachmentUpdateAndSetMatrix(aTrackInstance->mAttachmentID, aOverlayMatrix);

		aReanim.Draw(&aMemoryGraphics);
	}
	else if (aZombieDef.mReanimationType == ReanimationType::REANIM_ZOMBIE)
	{
		Reanimation aReanim;
		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.SetFramesForLayer("anim_idle");
		aReanim.OverrideScale(scale, scale);
		Zombie::SetupReanimLayers(&aReanim, aUseZombieType);

		if (theZombieType == ZombieType::ZOMBIE_DOOR)
		{
			aReanim.AssignRenderGroupToTrack("anim_screendoor", RENDER_GROUP_NORMAL);
			aReanim.AssignRenderGroupToTrack("Zombie_outerarm_screendoor", RENDER_GROUP_NORMAL);
		}
		else if (theZombieType == ZombieType::ZOMBIE_FLAG)
		{
			Reanimation aReanimFlag;
			aReanimFlag.OverrideScale(scale, scale);
			aReanimFlag.ReanimationInitializeType(aPosX, aPosY, ReanimationType::REANIM_FLAG);
			aReanimFlag.SetFramesForLayer("Zombie_flag");
			aReanimFlag.Draw(&aMemoryGraphics);
		}

		if (theZombieType == ZombieType::ZOMBIE_TRAFFIC_CONE) 
		{
			aReanim.AssignRenderGroupToTrack("anim_cone", RENDER_GROUP_HELMET);
		}
		else if (theZombieType == ZombieType::ZOMBIE_PAIL) 
		{
			aReanim.AssignRenderGroupToTrack("anim_bucket", RENDER_GROUP_HELMET);
		}

		aReanim.Draw(&aMemoryGraphics);

		if (theZombieType == ZombieType::ZOMBIE_TRAFFIC_CONE || theZombieType == ZombieType::ZOMBIE_PAIL)
		{
			aReanim.mOverlayMatrix.m02 -= 2;
			aReanim.DrawRenderGroup(&aMemoryGraphics, RENDER_GROUP_HELMET);
		}
	}
	else if (aZombieDef.mReanimationType == ReanimationType::REANIM_BOSS)
	{
		Reanimation aReanim;
		aReanim.OverrideScale(scale, scale);
		aReanim.ReanimationInitializeType(-524.0f * scale, -88.0f * scale, aZombieDef.mReanimationType);
		aReanim.SetFramesForLayer("anim_head_idle");
		Reanimation aReanimDriver;
		aReanimDriver.OverrideScale(scale, scale);
		aReanimDriver.ReanimationInitializeType(46.0f * scale, 22.0f * scale, ReanimationType::REANIM_BOSS_DRIVER);
		aReanimDriver.SetFramesForLayer("anim_idle");

		aReanim.Draw(&aMemoryGraphics);
		aReanimDriver.Draw(&aMemoryGraphics);
		aReanim.AssignRenderGroupToTrack("boss_body1", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToTrack("boss_neck", RENDER_GROUP_HIDDEN);
		aReanim.AssignRenderGroupToTrack("boss_head2", RENDER_GROUP_HIDDEN);
		aReanim.Draw(&aMemoryGraphics);
	}
	else if (theZombieType == ZombieType::ZOMBIE_BALLOON)
	{
		Reanimation aReanim;
		aReanim.OverrideScale(scale, scale);
		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		Reanimation aPropellerReanim;
		aPropellerReanim.OverrideScale(scale, scale);
		aPropellerReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aPropellerReanim.SetFramesForLayer("Propeller");
		aPropellerReanim.mLoopType = ReanimLoopType::REANIM_LOOP_FULL_LAST_FRAME;

		aReanim.Draw(&aMemoryGraphics);
		aPropellerReanim.Draw(&aMemoryGraphics);
	}
	else if (theZombieType == ZombieType::ZOMBIE_BUNGEE)
	{
		Reanimation aReanim;
		int relativeY = maxHeight - 210 * scale;
		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.OverrideScale(scale, scale);

		int aCordCelHeight = IMAGE_BUNGEECORD->GetCelHeight();
		int iterat = 0;
		aMemoryGraphics.SetLinearBlend(false);
		for (float y = relativeY - aCordCelHeight; y > -aCordCelHeight; y -= aCordCelHeight)
		{
			TodDrawImageScaledF(&aMemoryGraphics, IMAGE_BUNGEECORD, (61.0f - 4.0f + 22)* scale, (-iterat * aCordCelHeight - 14)* scale + relativeY, scale, scale);
			iterat++;
		}
		aMemoryGraphics.SetLinearBlend(true);

		SDL3Image* aMemoryImage2 = MakeBlankSDL3Image(200 * scale, 210 * scale);
		Graphics aMemoryGraphics2(aMemoryImage2);
		aMemoryGraphics2.SetLinearBlend(true);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aMemoryImage2->mD3DData);
		aReanim.Draw(&aMemoryGraphics2);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)aMemoryImage->mD3DData);
		aMemoryGraphics.DrawImageF(aMemoryImage2, 0, relativeY);
	}
#ifdef _HAS_NEW_DANCERS
	else if (theZombieType == ZombieType::ZOMBIE_DANCER)
	{
		Reanimation aReanim;
		aReanim.OverrideScale(0.79872f * scale, 0.79872f * scale);
		aPosX += 8 * scale;
		aPosY += 32 * scale;

		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.PlayReanim("anim_moonwalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
		aReanim.Draw(&aMemoryGraphics);
	}
	else if (theZombieType == ZombieType::ZOMBIE_BACKUP_DANCER)
	{
		Reanimation aReanim;
		aReanim.OverrideScale(0.79872f * scale, 0.79872f * scale);
		aPosX += 8 * scale;
		aPosY += 32 * scale;

		aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
		aReanim.PlayReanim("anim_armraise", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
		aReanim.mAnimTime = 0.5f;
		aReanim.Draw(&aMemoryGraphics);
	}
#endif
	else if (theZombieType == ZombieType::ZOMBIE_BOBSLED)
	{
		{
			TodDrawImageScaledF(&aMemoryGraphics, IMAGE_ZOMBIE_BOBSLED_INSIDE, -55 * scale, 80 * scale, scale, scale);
		}

		{
			Reanimation aReanim;
			aReanim.OverrideScale(scale, scale);
			aPosX = 40.0f * scale;
			aPosY = 30.0f * scale;
			aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
			aReanim.PlayReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aReanim.mAnimTime = 1.0f;
			aReanim.Draw(&aMemoryGraphics);
		}

		{
			Reanimation aReanim;
			aReanim.OverrideScale(scale, scale);
			aPosX = 90.0f * scale;
			aPosY = 30.0f * scale;
			aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
			aReanim.PlayReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aReanim.mAnimTime = 1.0f;
			aReanim.Draw(&aMemoryGraphics);
		}

		{
			Reanimation aReanim;
			aReanim.OverrideScale(scale, scale);
			aPosX = 140.0f * scale;
			aPosY = 30.0f * scale;
			aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
			aReanim.PlayReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aReanim.mAnimTime = 1.0f;
			aReanim.Draw(&aMemoryGraphics);
		}

		{
			Reanimation aReanim;
			aReanim.OverrideScale(scale, scale);
			aPosX = 190.0f * scale;
			aPosY = 30.0f * scale;
			aReanim.ReanimationInitializeType(aPosX, aPosY, aZombieDef.mReanimationType);
			aReanim.PlayReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aReanim.mAnimTime = 1.0f;
			aReanim.Draw(&aMemoryGraphics);
		}

		{
			TodDrawImageScaledF(&aMemoryGraphics, IMAGE_ZOMBIE_BOBSLED1, -55 * scale, 80 * scale, scale, scale);
		}
	}
	else
	{
		const char* aTrackName = "anim_idle";
		if (theZombieType == ZombieType::ZOMBIE_POGO)
		{
			aTrackName = "anim_pogo";
		}
		else if (theZombieType == ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE)
		{
			aTrackName = "anim_idle";
			aPosX += 50 * scale;
		}
		else if (theZombieType == ZombieType::ZOMBIE_POLEVAULTER)
		{
			aTrackName = "anim_walk";
		}
		else if (theZombieType == ZombieType::ZOMBIE_GARGANTUAR)
		{
			aPosY = 60.0f * scale;
		}
		else if (theZombieType == ZombieType::ZOMBIE_ZAMBONI)
		{
			aPosX += 50 * scale;
		}

		aMemoryGraphics.mScaleX *= scale;
		aMemoryGraphics.mScaleY *= scale;

		DrawReanimatorFrame(&aMemoryGraphics, aPosX, aPosY, aZombieDef.mReanimationType, aTrackName, DrawVariation::VARIATION_NORMAL, DrawFilterVariation::FILTERVARIATION_NONE, DrawBitVariation::BITVARIATION_NONE);
	}

	SDL_SetRenderTarget(LawnApp::mSDLRenderer, anOldRenderTarget);

	return ConvertSDL3ImageToMemoryImage(aMemoryImage);
}

//0x46FDC0
void ReanimatorCache::ReanimatorCacheInitialize()
{
	mApp = (LawnApp*)gSexyAppBase;
	for (int i = 0; i < SeedType::NUM_SEED_TYPES; i++)
		mPlantImages[i] = nullptr;
	for (int i = 0; i < LawnMowerType::NUM_MOWER_TYPES; i++)
		mLawnMowers[i] = nullptr;
	for (int i = 0; i < ZombieType::NUM_ZOMBIE_TYPES; i++)
		mZombieImages[i] = nullptr;
}

//0x46FED0
void ReanimatorCache::ReanimatorCacheDispose()
{
	for (int i = 0; i < SeedType::NUM_SEED_TYPES; i++)
		delete mPlantImages[i];
	while (mImageVariationList.mSize != 0)
	{
		ReanimCacheImageVariation aImageVariation = mImageVariationList.RemoveHead();
		if (aImageVariation.mImage != nullptr)
			delete aImageVariation.mImage;
	}
	for (int i = 0; i < LawnMowerType::NUM_MOWER_TYPES; i++)
		delete mLawnMowers[i];
	for (int i = 0; i < ZombieType::NUM_ZOMBIE_TYPES; i++)
		delete mZombieImages[i];
}


//0x46FFB0
void ReanimatorCache::DrawCachedPlant(Graphics* g, float thePosX, float thePosY, SeedType theSeedType, DrawVariation theDrawVariation, DrawFilterVariation theFilterVariation, unsigned int theDrawBitVariation)
{
	TOD_ASSERT(theSeedType >= 0 && theSeedType < SeedType::NUM_SEED_TYPES);

	MemoryImage* aImage = nullptr;
	if (theDrawVariation != DrawVariation::VARIATION_NORMAL)
	{
		for (TodListNode<ReanimCacheImageVariation>* aNode = mImageVariationList.mHead; aNode != nullptr; aNode = aNode->mNext)
		{
			ReanimCacheImageVariation& aImageVariation = aNode->mValue;
			if (aImageVariation.mSeedType == theSeedType && aImageVariation.mDrawVariation == theDrawVariation && aImageVariation.mFilterVariation == theFilterVariation &&
				aImageVariation.mDrawBitVariation == theDrawBitVariation)
			{
				aImage = aImageVariation.mImage;
				break;
			}
		}

		if (aImage == nullptr)
		{
			aImage = MakeCachedPlantFrame(theSeedType, theDrawVariation, theFilterVariation, theDrawBitVariation);
			ReanimCacheImageVariation aNewImageVariation;
			aNewImageVariation.mSeedType = theSeedType;
			aNewImageVariation.mDrawVariation = theDrawVariation;
			aNewImageVariation.mFilterVariation = theFilterVariation;
			aNewImageVariation.mDrawBitVariation = theDrawBitVariation;
			aNewImageVariation.mImage = aImage;
			mImageVariationList.AddHead(aNewImageVariation);
		}
	}
	else if (theFilterVariation != DrawFilterVariation::FILTERVARIATION_NONE)
	{
		for (TodListNode<ReanimCacheImageVariation>* aNode = mImageVariationList.mHead; aNode != nullptr; aNode = aNode->mNext)
		{
			ReanimCacheImageVariation& aImageVariation = aNode->mValue;
			if (aImageVariation.mSeedType == theSeedType && aImageVariation.mDrawVariation == DrawVariation::VARIATION_NORMAL && aImageVariation.mFilterVariation == theFilterVariation &&
				aImageVariation.mDrawBitVariation == theDrawBitVariation)
			{
				aImage = aImageVariation.mImage;
				break;
			}
		}

		if (aImage == nullptr)
		{
			aImage = MakeCachedPlantFrame(theSeedType, DrawVariation::VARIATION_NORMAL, theFilterVariation, theDrawBitVariation);
			ReanimCacheImageVariation aNewImageVariation;
			aNewImageVariation.mSeedType = theSeedType;
			aNewImageVariation.mDrawVariation = DrawVariation::VARIATION_NORMAL;
			aNewImageVariation.mFilterVariation = theFilterVariation;
			aNewImageVariation.mDrawBitVariation = theDrawBitVariation;
			aNewImageVariation.mImage = aImage;
			mImageVariationList.AddHead(aNewImageVariation);
		}
	}
	else if (theDrawBitVariation != DrawBitVariation::BITVARIATION_NONE)
	{
		for (TodListNode<ReanimCacheImageVariation>* aNode = mImageVariationList.mHead; aNode != nullptr; aNode = aNode->mNext)
		{
			ReanimCacheImageVariation& aImageVariation = aNode->mValue;
			if (aImageVariation.mSeedType == theSeedType && aImageVariation.mDrawVariation == DrawVariation::VARIATION_NORMAL && 
				aImageVariation.mFilterVariation == DrawFilterVariation::FILTERVARIATION_NONE && aImageVariation.mDrawBitVariation == theDrawBitVariation)
			{
				aImage = aImageVariation.mImage;
				break;
			}
		}

		if (aImage == nullptr)
		{
			aImage = MakeCachedPlantFrame(theSeedType, DrawVariation::VARIATION_NORMAL, theFilterVariation, theDrawBitVariation);
			ReanimCacheImageVariation aNewImageVariation;
			aNewImageVariation.mSeedType = theSeedType;
			aNewImageVariation.mDrawVariation = DrawVariation::VARIATION_NORMAL;
			aNewImageVariation.mFilterVariation = DrawFilterVariation::FILTERVARIATION_NONE;
			aNewImageVariation.mDrawBitVariation = theDrawBitVariation;
			aNewImageVariation.mImage = aImage;
			mImageVariationList.AddHead(aNewImageVariation);
		}
	}
	else
	{
		aImage = mPlantImages[(int)theSeedType];
		if (aImage == nullptr)
		{
			aImage = MakeCachedPlantFrame(theSeedType, DrawVariation::VARIATION_NORMAL, DrawFilterVariation::FILTERVARIATION_NONE);
			mPlantImages[(int)theSeedType] = aImage;
		}
	}

	int aOffsetX, aOffsetY, aWidth, aHeight;
	GetPlantImageSize(theSeedType, aOffsetX, aOffsetY, aWidth, aHeight);

	/*if (isIZombieLevel)
	{
		Image* aFilteredImage = FilterEffectGetImage(aImage, FilterEffect::FILTER_EFFECT_WHITE);

		Graphics gFilter(*g);
		gFilter.SetColorizeImages(true);
		gFilter.mTransX = g->mTransX + 4;
		gFilter.mTransY = g->mTransY + 4;
		gFilter.SetColor(Color(122, 86, 58));
		TodDrawImageScaledF(&gFilter, aFilteredImage, thePosX + (aOffsetX * g->mScaleX), thePosY + (aOffsetY * g->mScaleY), g->mScaleX, g->mScaleY);

		gFilter.mTransX = g->mTransX + 2;
		gFilter.mTransY = g->mTransY + 2;
		gFilter.SetColor(Color(171, 135, 107));
		TodDrawImageScaledF(&gFilter, aFilteredImage, thePosX + (aOffsetX * g->mScaleX), thePosY + (aOffsetY * g->mScaleY), g->mScaleX, g->mScaleY);

		gFilter.mTransX = g->mTransX - 2;
		gFilter.mTransY = g->mTransY - 2;
		gFilter.SetColor(Color(171, 135, 107));
		TodDrawImageScaledF(&gFilter, aFilteredImage, thePosX + (aOffsetX * g->mScaleX), thePosY + (aOffsetY * g->mScaleY), g->mScaleX, g->mScaleY);

		gFilter.mTransX = g->mTransX;
		gFilter.mTransY = g->mTransY;
		gFilter.SetColor(Color(255, 201, 160));
		TodDrawImageScaledF(&gFilter, aImage, thePosX + (aOffsetX * g->mScaleX), thePosY + (aOffsetY * g->mScaleY), g->mScaleX, g->mScaleY);
	}
	else*/
	{
		TodDrawImageScaledF(g, aImage, thePosX + (aOffsetX * g->mScaleX), thePosY + (aOffsetY * g->mScaleY), g->mScaleX, g->mScaleY);
	}
}

//0x470110
void ReanimatorCache::DrawCachedMower(Graphics* g, float thePosX, float thePosY, LawnMowerType theMowerType, DrawFilterVariation theFilterVariation)
{
	TOD_ASSERT(theMowerType >= 0 && theMowerType < LawnMowerType::NUM_MOWER_TYPES);
	
	if (mLawnMowers[(int)theMowerType] == nullptr)
		mLawnMowers[(int)theMowerType] = MakeCachedMowerFrame(theMowerType);
	TodDrawImageScaledF(g, mLawnMowers[(int)theMowerType], thePosX - 20.0f, thePosY, g->mScaleX, g->mScaleY);
}

//0x470170
void ReanimatorCache::DrawCachedZombie(Graphics* g, float thePosX, float thePosY, ZombieType theZombieType, DrawFilterVariation theFilterVariation)
{
	TOD_ASSERT(theZombieType >= 0 && theZombieType < ZombieType::NUM_CACHED_ZOMBIE_TYPES);
	
	if (mZombieImages[(int)theZombieType] == nullptr)
		mZombieImages[(int)theZombieType] = MakeCachedZombieFrame(theZombieType);
	TodDrawImageScaledF(g, mZombieImages[(int)theZombieType], thePosX, thePosY, g->mScaleX, g->mScaleY);
}
