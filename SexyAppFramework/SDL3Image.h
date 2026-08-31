#ifndef __SDL3IMAGE_H__
#define __SDL3IMAGE_H__

#include <SDL3/SDL.h>
#include "MemoryImage.h"

namespace Sexy
{
	class SDL3Image : public MemoryImage
	{
		public:
			enum SDL_BlendModes {
				NONE = -1,
				BLEND,
				ADD,
				BLEND_PREMUL,
				ADD_PREMUL,
				MOD,
				MUL
			};
			Uint32					GetBlendMode(SDL_BlendModes theDrawMode);
		public:
			SDL_Renderer* mRenderer;
			static SDL_Texture*	CreateRenderTarget(SDL_Renderer* theRenderer, int theWidth, int theHeight);
			SDL_Texture* GetTexture(Image* image);
		public:
			SDL3Image(SDL_Renderer* renderer);
			virtual ~SDL3Image();

			void					SetLinearBlend(SDL_Texture* theTexture, bool blend, bool pixelart);

			virtual bool			PolyFill3D(const Point theVertices[], int theNumVertices, const Rect* theClipRect, const Color& theColor, int theDrawMode, int tx, int ty, bool convex);

			virtual void			FillRect(const Rect& theRect, const Color& theColor, int theDrawMode);
			virtual void			DrawRect(const Rect& theRect, const Color& theColor, int theDrawMode);
			virtual void			ClearRect(const Rect& theRect);
			virtual void			DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode);
			virtual void			DrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode);
			virtual void			FillScanLines(Span* theSpans, int theSpanCount, const Color& theColor, int theDrawMode);
			virtual void			FillScanLinesWithCoverage(Span* theSpans, int theSpanCount, const Color& theColor, int theDrawMode, const BYTE* theCoverage, int theCoverX, int theCoverY, int theCoverWidth, int theCoverHeight);
			virtual void			Blt(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart);
			virtual void			BltF(Image* theImage, float theX, float theY, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart);
			virtual void			BltRotated(Image* theImage, float theX, float theY, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY, bool blend, bool pixelart);
			virtual void			StretchBlt(Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart);
			virtual void			BltMatrix(Image* theImage, float x, float y, const SexyMatrix3& theMatrix, const Rect& theClipRect, const Color& theColor, int theDrawMode, const Rect& theSrcRect, bool blend, bool pixelart);
			virtual void			BltTrianglesTex(Image* theTexture, const TriVertex theVertices[][3], int theNumTriangles, const Rect& theClipRect, const Color& theColor, int theDrawMode, float tx, float ty, bool blend, bool pixelart);

			virtual void			BltMirror(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart);
			virtual void			StretchBltMirror(Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool blend, bool pixelart);
	};
}

#endif
