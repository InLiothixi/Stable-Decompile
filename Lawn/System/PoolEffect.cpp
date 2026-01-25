#include "PoolEffect.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../GameConstants.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../SexyAppFramework/DDImage.h"
#include "../../SexyAppFramework/Graphics.h"
#include "../../SexyAppFramework/DDInterface.h"
#include "../../SexyAppFramework/D3DInterface.h"
#include "../../SexyAppFramework/SDL3Image.h"

//0x469A60
void PoolEffect::PoolEffectInitialize()
{
    TodHesitationBracket aHesitation("PoolEffectInitialize");

    mApp = gLawnApp;

    mCausticImage = new SDL3Image(LawnApp::mSDLRenderer);
    mCausticImage->mWidth = CAUSTIC_IMAGE_WIDTH;
    mCausticImage->mHeight = CAUSTIC_IMAGE_HEIGHT;
    mCausticImage->mBits = new unsigned long[CAUSTIC_IMAGE_WIDTH * CAUSTIC_IMAGE_HEIGHT + 1];
    memset(mCausticImage->mBits, 0xFF, CAUSTIC_IMAGE_WIDTH * CAUSTIC_IMAGE_HEIGHT * 4);
    mCausticImage->mBits[CAUSTIC_IMAGE_WIDTH * CAUSTIC_IMAGE_HEIGHT] = MEMORYCHECK_ID;
    mCausticImage->mD3DData = SDL_CreateTexture(LawnApp::mSDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, mCausticImage->mWidth, mCausticImage->mHeight);
    SDL_SetTextureBlendMode((SDL_Texture*)mCausticImage->mD3DData, SDL_BLENDMODE_BLEND);

    mCausticGrayscaleImage = new unsigned char[256 * 256];
    MemoryImage* aCausticGrayscaleImage = (MemoryImage*)IMAGE_POOL_CAUSTIC_EFFECT;
    int index = 0;
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            mCausticGrayscaleImage[index] = (unsigned char)aCausticGrayscaleImage->mBits[index];
            index++;
        }
    }
}

void PoolEffect::PoolEffectDispose()
{
    delete mCausticImage;
    delete[] mCausticGrayscaleImage;
}

//0x469BC0
unsigned int PoolEffect::BilinearLookupFixedPoint(unsigned int u, unsigned int v)
{
    int x0 = (u >> 16) & 255;
    int y0 = (v >> 16) & 255;
    int x1 = (x0 + 1) & 255;
    int y1 = (y0 + 1) & 255;

    float fx = (u & 0xFFFF) / 65536.0f;
    float fy = (v & 0xFFFF) / 65536.0f;

    int p00 = mCausticGrayscaleImage[y0 * 256 + x0];
    int p10 = mCausticGrayscaleImage[y0 * 256 + x1];
    int p01 = mCausticGrayscaleImage[y1 * 256 + x0];
    int p11 = mCausticGrayscaleImage[y1 * 256 + x1];

    float val =
        p00 * (1 - fx) * (1 - fy) +
        p10 * fx * (1 - fy) +
        p01 * (1 - fx) * fy +
        p11 * fx * fy;

    return (unsigned int)val;
}

//0x469CA0
void PoolEffect::UpdateWaterEffect(Sexy::Graphics* g)
{

    std::vector<Uint32> buffer(CAUSTIC_IMAGE_WIDTH * CAUSTIC_IMAGE_HEIGHT);

    for (int y = 0; y < CAUSTIC_IMAGE_HEIGHT; y++)
    {
        int timeV1 = (256 - y) << 17;
        int timeV0 = y << 17;

        for (int x = 0; x < CAUSTIC_IMAGE_WIDTH; x++)
        {
            int idx = y * CAUSTIC_IMAGE_WIDTH + x;
            Uint32 oldPixel = mCausticImage->mBits[idx];  

            int timeU = x << 17;
            int timePool0 = mPoolCounter << 16;
            int timePool1 = ((mPoolCounter & 65535) + 1) << 16;

            int a1 = (unsigned char)BilinearLookupFixedPoint(timeU - timePool1 / 6,
                timeV1 + timePool0 / 8);
            int a0 = (unsigned char)BilinearLookupFixedPoint(timeU + timePool0 / 10,
                timeV0);
            unsigned char a = (unsigned char)((a0 + a1) / 2);

            unsigned char alpha;
            if (a >= 160U)
                alpha = 255 - 2 * (a - 160U);
            else if (a >= 128U)
                alpha = 5 * (a - 128U);
            else
                alpha = 0;

           mCausticImage->mBits[idx] = buffer[idx] = (oldPixel & 0x00FFFFFF) + (((int)alpha / 3) << 24);
        }
    }

    SDL_UpdateTexture(
        (SDL_Texture*)mCausticImage->mD3DData,
        nullptr,
        buffer.data(),
        CAUSTIC_IMAGE_WIDTH * sizeof(Uint32)
    );
}

//0x469DE0
void PoolEffect::PoolEffectDraw(Sexy::Graphics* g, bool theIsNight)
{
    float aGridSquareX = IMAGE_POOL->GetWidth() / 15.0f;
    float aGridSquareY = IMAGE_POOL->GetHeight() / 5.0f;
    float aOffsetArray[3][16][6][2] = { 0 };
    for (int x = 0; x <= 15; x++)
    {
        for (int y = 0; y <= 5; y++)
        {
            aOffsetArray[2][x][y][0] = x / 15.0f;
            aOffsetArray[2][x][y][1] = y / 5.0f;
            if (x != 0 && x != 15 && y != 0 && y != 5)
            {
                float aPoolPhase = mPoolCounter * 2 * PI;
                float aWaveTime1 = aPoolPhase / 800.0;
                float aWaveTime2 = aPoolPhase / 150.0;
                float aWaveTime3 = aPoolPhase / 900.0;
                float aWaveTime4 = aPoolPhase / 800.0;
                float aWaveTime5 = aPoolPhase / 110.0;
                float xPhase = x * 3.0f * 2 * PI / 15.0f;
                float yPhase = y * 3.0f * 2 * PI / 5.0f;

                aOffsetArray[0][x][y][0] = sin(yPhase + aWaveTime2) * 0.002f + sin(yPhase + aWaveTime1) * 0.005f;
                aOffsetArray[0][x][y][1] = sin(xPhase + aWaveTime5) * 0.01f + sin(xPhase + aWaveTime3) * 0.015f + sin(xPhase + aWaveTime4) * 0.005f;
                aOffsetArray[1][x][y][0] = sin(yPhase * 0.2f + aWaveTime2) * 0.015f + sin(yPhase * 0.2f + aWaveTime1) * 0.012f ;
                aOffsetArray[1][x][y][1] = sin(xPhase * 0.2f + aWaveTime5) * 0.005f + sin(xPhase * 0.2f + aWaveTime3) * 0.015f + sin(xPhase * 0.2f + aWaveTime4) * 0.02f;
                aOffsetArray[2][x][y][0] += sin(yPhase + aWaveTime1 * 1.5f) * 0.004f + sin(yPhase + aWaveTime2 * 1.5f) * 0.005f;
                aOffsetArray[2][x][y][1] += sin(xPhase * 4.0f + aWaveTime5 * 2.5f) * 0.005f + sin(xPhase * 2.0f + aWaveTime3 * 2.5f) * 0.04f + sin(xPhase * 3.0f + aWaveTime4 * 2.5f) * 0.02f;
            }
            else
            {
                aOffsetArray[0][x][y][0] = 0.0f;
                aOffsetArray[0][x][y][1] = 0.0f;
                aOffsetArray[1][x][y][0] = 0.0f;
                aOffsetArray[1][x][y][1] = 0.0f;
            }
        }
    }

    int aIndexOffsetX[6] = { 0, 0, 1, 0, 1, 1 };
    int aIndexOffsetY[6] = { 0, 1, 1, 0, 1, 0 };
    TriVertex aVertArray[3][150][3];

    for (int x = 0; x < 15; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            for (int aLayer = 0; aLayer < 3; aLayer++)
            {
                TriVertex* pVert = &aVertArray[aLayer][x * 10 + y * 2][0];
                for (int aVertIndex = 0; aVertIndex < 6; aVertIndex++, pVert++)
                {
                    int aIndexX = x + aIndexOffsetX[aVertIndex];
                    int aIndexY = y + aIndexOffsetY[aVertIndex];
                    if (aLayer == 2)
                    {
                        pVert->x = (704.0f / 15.0f) * aIndexX + 45.0f;
                        pVert->y = 30.0f * aIndexY + 288.0f;
                        pVert->u = aOffsetArray[2][aIndexX][aIndexY][0] + aIndexX / 15.0f;
                        pVert->v = aOffsetArray[2][aIndexX][aIndexY][1] + aIndexY / 5.0f;

                        if (!g->mClipRect.Contains(pVert->x, pVert->y))
                        {
                            pVert->color = 0x00FFFFFFUL;
                        }
                        else if (aIndexX == 0 || aIndexX == 15 || aIndexY == 0)
                        {
                            pVert->color = 0x20FFFFFFUL;
                        }
                        else if (theIsNight)
                        {
                            pVert->color = 0x30FFFFFFUL;
                        }
                        else
                        {
                            pVert->color = aIndexX <= 7 ? 0xC0FFFFFFUL : 0x80FFFFFFUL;
                        }
                    }
                    else
                    {
                        pVert->color = 0xFFFFFFFFUL;
                        pVert->x = aIndexX * aGridSquareX + 35.0f;
                        pVert->y = aIndexY * aGridSquareY + 279.0f;
                        pVert->u = aOffsetArray[aLayer][aIndexX][aIndexY][0] + aIndexX / 15.0f;
                        pVert->v = aOffsetArray[aLayer][aIndexX][aIndexY][1] + aIndexY / 5.0f;
                        if (!g->mClipRect.Contains(pVert->x, pVert->y))
                        {
                            pVert->color = 0x00FFFFFFUL;
                        }
                    }
                }
            }
        }
    }

    if (theIsNight)
    {
        g->DrawTrianglesTex(IMAGE_POOL_BASE_NIGHT, aVertArray[0], 150);
        g->DrawTrianglesTex(IMAGE_POOL_SHADING_NIGHT, aVertArray[1], 150);
    }
    else
    {
        g->DrawTrianglesTex(IMAGE_POOL_BASE, aVertArray[0], 150);
        g->DrawTrianglesTex(IMAGE_POOL_SHADING, aVertArray[1], 150);
    }
    UpdateWaterEffect(g);
    SDL_SetRenderTextureAddressMode(LawnApp::mSDLRenderer, SDL_TEXTURE_ADDRESS_WRAP, SDL_TEXTURE_ADDRESS_WRAP);
    g->DrawTrianglesTex(mCausticImage, aVertArray[2], 150);
    SDL_SetRenderTextureAddressMode(LawnApp::mSDLRenderer, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);

}

void PoolEffect::PoolEffectUpdate()
{
    ++mPoolCounter;
}