#include <SDL3/SDL.h>
#include "SDL3Image.h"
#include "MemoryImage.h"
#include "TriVertex.h"
#include "SexyMatrix.h"
#include <vector>
#include "../LawnApp.h"

using namespace Sexy;

Uint32  SDL3Image::GetBlendMode(SDL_BlendModes theDrawMode)
{
    switch (theDrawMode) {
    case NONE: return SDL_BLENDMODE_NONE;
    case BLEND: return SDL_BLENDMODE_BLEND;
    case BLEND_PREMUL: return SDL_BLENDMODE_BLEND_PREMULTIPLIED;
    case ADD: return SDL_BLENDMODE_ADD;
    case ADD_PREMUL: return SDL_BLENDMODE_ADD_PREMULTIPLIED;
    case MOD: return SDL_BLENDMODE_MOD;
    case MUL: return SDL_BLENDMODE_MUL;
    default: return SDL_BLENDMODE_INVALID;
    }
}

SDL_Texture* Sexy::SDL3Image::GetTexture(Image* image)
{
    if (auto memoryImage = dynamic_cast<MemoryImage*>(image))
    {
        if (memoryImage->mD3DData == nullptr)
        {
            auto bits = memoryImage->GetBits();
            auto surface = SDL_CreateSurfaceFrom(memoryImage->mWidth, memoryImage->mHeight, SDL_PIXELFORMAT_ARGB8888, bits, memoryImage->mWidth * 4);
            memoryImage->mD3DData = (void*)SDL_CreateTextureFromSurface(mRenderer, surface);
            SDL_SetTextureBlendMode((SDL_Texture*)memoryImage->mD3DData, SDL_BLENDMODE_BLEND);
            SDL_DestroySurface(surface);
        }
        return (SDL_Texture*)memoryImage->mD3DData;
    }

    return nullptr;
}

SDL3Image::SDL3Image(SDL_Renderer* renderer)
    : mRenderer(renderer)
{
}

SDL3Image::~SDL3Image()
{
    if ((SDL_Texture*)mD3DData)
    {
        SDL_DestroyTexture((SDL_Texture*)mD3DData);
    }
}

bool SDL3Image::PolyFill3D(const Point theVertices[], int theNumVertices, const Rect* theClipRect, const Color& theColor, int theDrawMode, int tx, int ty, bool convex)
{
    return false;
}

void SDL3Image::SetLinearBlend(SDL_Texture* theTexture, bool blend, bool pixelart)
{
    SDL_SetTextureScaleMode(theTexture, pixelart ? SDL_ScaleMode::SDL_SCALEMODE_PIXELART : blend ? SDL_ScaleMode::SDL_SCALEMODE_LINEAR : SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
}

void SDL3Image::FillRect(const Rect& theRect, const Color& theColor, int theDrawMode)
{
    SDL_FRect rect = { (float)theRect.mX, (float)theRect.mY, (float)theRect.mWidth, (float)theRect.mHeight };
    SDL_SetRenderDrawColor(mRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
    SDL_SetRenderDrawBlendMode(mRenderer, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_RenderFillRect(mRenderer, &rect);
}

void SDL3Image::DrawRect(const Rect& theRect, const Color& theColor, int theDrawMode)
{
    SDL_FRect rect = { (float)theRect.mX, (float)theRect.mY, (float)theRect.mWidth + 1, (float)theRect.mHeight + 1 };
    SDL_SetRenderDrawColor(mRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
    SDL_SetRenderDrawBlendMode(mRenderer, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_RenderRect(mRenderer, &rect);
}

void SDL3Image::ClearRect(const Rect& theRect)
{
}

void SDL3Image::DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode)
{
    SDL_SetRenderDrawColor(mRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
    SDL_RenderLine(mRenderer, theStartX, theStartY, theEndX, theEndY);
}

void SDL3Image::DrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode)
{
    SDL_SetRenderDrawColor(mRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
    SDL_RenderLine(mRenderer, theStartX, theStartY, theEndX, theEndY);
}

void SDL3Image::FillScanLines(Span* theSpans, int theSpanCount, const Color& theColor, int theDrawMode)
{
}

void SDL3Image::FillScanLinesWithCoverage(Span* theSpans, int theSpanCount, const Color& theColor, int theDrawMode, const BYTE* theCoverage, int theCoverX, int theCoverY, int theCoverWidth, int theCoverHeight)
{
}

void SDL3Image::Blt(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect dstRect = { (float)theX, (float)theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_RenderTexture(mRenderer, texture, &srcRect, &dstRect);
}

void SDL3Image::BltF(Image* theImage, float theX, float theY, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect dstRect = { theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderTexture(mRenderer, texture, &srcRect, &dstRect);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}

void SDL3Image::BltRotated(Image* theImage, float theX, float theY, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect destRect = { theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };
    SDL_FPoint center = { theRotCenterX, theRotCenterY };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderTextureRotated(mRenderer, texture, &srcRect, &destRect, theRot, &center, SDL_FLIP_NONE);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}

void SDL3Image::StretchBlt(Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect destRect = { (float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth, (float)theDestRect.mHeight };
    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderTexture(mRenderer, texture, &srcRect, &destRect);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}

void SDL3Image::BltMatrix(Image* theImage, float x, float y, const SexyMatrix3& theMatrix, const Rect& theClipRect, const Color& theColor, int theDrawMode, const Rect& theSrcRect, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    float w2 = theSrcRect.mWidth / 2.0f;
    float h2 = theSrcRect.mHeight / 2.0f;

    float u0 = (float)theSrcRect.mX / theImage->mWidth;
    float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / theImage->mWidth;
    float v0 = (float)theSrcRect.mY / theImage->mHeight;
    float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / theImage->mHeight;

    SDL_FColor color = { theColor.mRed / 255.0f, theColor.mGreen / 255.0f, theColor.mBlue / 255.0f, theColor.mAlpha / 255.0f };

    SDL_Vertex vertices[4] =
    {
        { { -w2, -h2 }, color, { u0, v0 } },
        { { w2, -h2 }, color, { u1, v0 } },
        { { -w2, h2 }, color, { u0, v1 } },
        { { w2, h2 }, color, { u1, v1 } },
    };

    int indices[6] = { 0, 1, 2, 1, 3, 2 };

    for (int i = 0; i < 4; i++)
    {
        SexyVector3 v(vertices[i].position.x, vertices[i].position.y, 1);
        v = theMatrix * v;
        vertices[i].position.x = v.x + x;
        vertices[i].position.y = v.y + y;
    }

    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };

    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderGeometry(mRenderer, texture, vertices, 4, indices, 6);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}

void SDL3Image::BltTrianglesTex(Image* theTexture, const TriVertex theVertices[][3], int theNumTriangles, const Rect& theClipRect, const Color& theColor, int theDrawMode, float tx, float ty, bool blend, bool pixelart)
{
    auto texture = GetTexture(theTexture);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    std::vector<SDL_Vertex> vertices;
    vertices.resize(theNumTriangles * 3);

    for (int i = 0; i < theNumTriangles; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[i * 3 + j].position.x = theVertices[i][j].x + tx;
            vertices[i * 3 + j].position.y = theVertices[i][j].y + ty;
            vertices[i * 3 + j].tex_coord.x = theVertices[i][j].u;
            vertices[i * 3 + j].tex_coord.y = theVertices[i][j].v;
            if (theVertices[i][j].color != 0)
            {
                vertices[i * 3 + j].color.r = ((theVertices[i][j].color >> 16) & 0xff) / 255.0f;
                vertices[i * 3 + j].color.g = ((theVertices[i][j].color >> 8) & 0xff) / 255.0f;
                vertices[i * 3 + j].color.b = (theVertices[i][j].color & 0xff) / 255.0f;
                vertices[i * 3 + j].color.a = ((theVertices[i][j].color >> 24) & 0xff) / 255.0f;
            }
            else
            {
                vertices[i * 3 + j].color.r = theColor.mRed / 255.0f;
                vertices[i * 3 + j].color.g = theColor.mGreen / 255.0f;
                vertices[i * 3 + j].color.b = theColor.mBlue / 255.0f;
                vertices[i * 3 + j].color.a = theColor.mAlpha / 255.0f;
            }
        }
    }

    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };

    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderGeometry(mRenderer, texture, vertices.data(), vertices.size(), nullptr, 0);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}

void SDL3Image::BltMirror(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect destRect = { (float)theX, (float)theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_RenderTextureRotated(mRenderer, texture, &srcRect, &destRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
}

void SDL3Image::StretchBltMirror(Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart)
{
    auto texture = GetTexture(theImage);

    if (texture == nullptr)
        return;

    SetLinearBlend(texture, blend, pixelart);

    SDL_FRect srcRect = { (float)theSrcRect.mX,(float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight };
    SDL_FRect destRect = { (float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth, (float)theDestRect.mHeight };
    SDL_Rect clipRect = { theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight };
    SDL_SetTextureColorMod(texture, theColor.mRed, theColor.mGreen, theColor.mBlue);
    SDL_SetTextureAlphaMod(texture, theColor.mAlpha);
    SDL_SetTextureBlendMode(texture, GetBlendMode((SDL_BlendModes)theDrawMode));
    SDL_SetRenderClipRect(mRenderer, &clipRect);
    SDL_RenderTextureRotated(mRenderer, texture, &srcRect, &destRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_SetRenderClipRect(mRenderer, nullptr);
}