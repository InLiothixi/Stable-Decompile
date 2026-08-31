#pragma once

#include "../GameConstants.h"
#include "../SexyAppFramework/Point.h"
#include "../SexyAppFramework/Rect.h"

#include <cmath>

// Gameplay continues to use stable board-local design units.  This class owns
// the geometry of that coordinate space and its placement on a runtime canvas,
// so simulation/save coordinates do not need to become display pixels.
enum class BoardGridLayout
{
	FiveRows,
	SixRows,
	Roof
};

class BoardGeometry
{
public:
	static constexpr int kDesignWidth = BOARD_WIDTH;
	static constexpr int kDesignHeight = BOARD_HEIGHT;
	static constexpr int kColumnCount = 9;
	static constexpr int kStorageRowCount = 6;
	static constexpr int kFiveRowCount = 5;
	static constexpr int kSixRowCount = 6;
	static constexpr int kLawnOriginX = LAWN_XMIN;
	static constexpr int kLawnOriginY = LAWN_YMIN;
	static constexpr int kColumnWidth = 80;
	static constexpr int kFiveRowHeight = 100;
	static constexpr int kSixRowHeight = 85;
	static constexpr int kRoofRowHeight = 85;
	static constexpr int kHighGroundHeight = HIGH_GROUND_HEIGHT;
	static constexpr int kRoofSlopedColumnCount = 5;
	static constexpr int kRoofSlopePerColumn = 20;
	static constexpr int kRoofGridTopAdjustment = -10;
	static constexpr float kRoofSlopeEndX = 440.0f;
	static constexpr float kRoofSlopePerPixel = 0.25f;

private:
	Sexy::Rect mCanvasBounds;
	BoardGridLayout mGridLayout;

	static int Clamp(int theValue, int theMin, int theMax)
	{
		if (theValue < theMin)
			return theMin;
		if (theValue > theMax)
			return theMax;
		return theValue;
	}

public:
	BoardGeometry() :
		mCanvasBounds(0, 0, kDesignWidth, kDesignHeight),
		mGridLayout(BoardGridLayout::FiveRows)
	{
	}

	BoardGeometry(const Sexy::Rect& theCanvasBounds, BoardGridLayout theGridLayout) :
		mCanvasBounds(theCanvasBounds),
		mGridLayout(theGridLayout)
	{
		if (mCanvasBounds.mWidth <= 0)
			mCanvasBounds.mWidth = kDesignWidth;
		if (mCanvasBounds.mHeight <= 0)
			mCanvasBounds.mHeight = kDesignHeight;
	}

	// Centers an unscaled gameplay board in a logical canvas.  This is the
	// current runtime mode: wider canvases reveal more presentation space while
	// all mechanics retain their original local units.
	static BoardGeometry CenteredInCanvas(const Sexy::Rect& theCanvas, BoardGridLayout theGridLayout)
	{
		return BoardGeometry(
			Sexy::Rect(
				theCanvas.mX + (theCanvas.mWidth - kDesignWidth) / 2,
				theCanvas.mY + (theCanvas.mHeight - kDesignHeight) / 2,
				kDesignWidth,
				kDesignHeight),
			theGridLayout);
	}

	// Available to presentation code that wants to display the same stable
	// board-local coordinate space inside an arbitrary rectangle.
	static BoardGeometry ScaleToFitCanvas(const Sexy::Rect& theCanvas, BoardGridLayout theGridLayout)
	{
		if (theCanvas.mWidth <= 0 || theCanvas.mHeight <= 0)
			return BoardGeometry(Sexy::Rect(0, 0, kDesignWidth, kDesignHeight), theGridLayout);

		int aWidth = theCanvas.mWidth;
		int aHeight = static_cast<int>(std::lround(
			static_cast<double>(aWidth) * kDesignHeight / kDesignWidth));
		if (aHeight > theCanvas.mHeight)
		{
			aHeight = theCanvas.mHeight;
			aWidth = static_cast<int>(std::lround(
				static_cast<double>(aHeight) * kDesignWidth / kDesignHeight));
		}

		return BoardGeometry(
			Sexy::Rect(
				theCanvas.mX + (theCanvas.mWidth - aWidth) / 2,
				theCanvas.mY + (theCanvas.mHeight - aHeight) / 2,
				aWidth,
				aHeight),
			theGridLayout);
	}

	BoardGridLayout GetGridLayout() const
	{
		return mGridLayout;
	}

	Sexy::Rect GetLocalBounds() const
	{
		return Sexy::Rect(0, 0, kDesignWidth, kDesignHeight);
	}

	bool ContainsLocalPoint(int theX, int theY) const
	{
		return theX >= 0 && theX < kDesignWidth &&
			theY >= 0 && theY < kDesignHeight;
	}

	const Sexy::Rect& GetCanvasBounds() const
	{
		return mCanvasBounds;
	}

	Sexy::Point GetCanvasOrigin() const
	{
		return Sexy::Point(mCanvasBounds.mX, mCanvasBounds.mY);
	}

	double GetCanvasScaleX() const
	{
		return static_cast<double>(mCanvasBounds.mWidth) / kDesignWidth;
	}

	double GetCanvasScaleY() const
	{
		return static_cast<double>(mCanvasBounds.mHeight) / kDesignHeight;
	}

	Sexy::FPoint LocalToCanvas(double theX, double theY) const
	{
		return Sexy::FPoint(
			mCanvasBounds.mX + theX * GetCanvasScaleX(),
			mCanvasBounds.mY + theY * GetCanvasScaleY());
	}

	Sexy::FPoint CanvasToLocal(double theX, double theY) const
	{
		return Sexy::FPoint(
			(theX - mCanvasBounds.mX) / GetCanvasScaleX(),
			(theY - mCanvasBounds.mY) / GetCanvasScaleY());
	}

	Sexy::Point CanvasToLocalPixel(int theX, int theY) const
	{
		const Sexy::FPoint aPoint = CanvasToLocal(theX, theY);
		return Sexy::Point(
			static_cast<int>(std::floor(aPoint.mX)),
			static_cast<int>(std::floor(aPoint.mY)));
	}

	Sexy::Rect LocalToCanvasRect(const Sexy::Rect& theRect) const
	{
		const Sexy::FPoint aTopLeft = LocalToCanvas(theRect.mX, theRect.mY);
		const Sexy::FPoint aBottomRight = LocalToCanvas(
			theRect.mX + theRect.mWidth,
			theRect.mY + theRect.mHeight);
		return Sexy::Rect(
			static_cast<int>(std::lround(aTopLeft.mX)),
			static_cast<int>(std::lround(aTopLeft.mY)),
			static_cast<int>(std::lround(aBottomRight.mX - aTopLeft.mX)),
			static_cast<int>(std::lround(aBottomRight.mY - aTopLeft.mY)));
	}

	int GetActiveRowCount() const
	{
		return mGridLayout == BoardGridLayout::SixRows ? kSixRowCount : kFiveRowCount;
	}

	int GetRowHeight() const
	{
		return mGridLayout == BoardGridLayout::FiveRows ? kFiveRowHeight : kSixRowHeight;
	}

	bool IsValidColumn(int theGridX) const
	{
		return theGridX >= 0 && theGridX < kColumnCount;
	}

	bool IsValidStorageRow(int theGridY) const
	{
		return theGridY >= 0 && theGridY < kStorageRowCount;
	}

	bool IsValidStorageCell(int theGridX, int theGridY) const
	{
		return IsValidColumn(theGridX) && IsValidStorageRow(theGridY);
	}

	bool IsValidPlayableRow(int theGridY) const
	{
		return theGridY >= 0 && theGridY < GetActiveRowCount();
	}

	bool IsValidPlayableCell(int theGridX, int theGridY) const
	{
		return IsValidColumn(theGridX) && IsValidPlayableRow(theGridY);
	}

	int ClampGridX(int theGridX) const
	{
		return Clamp(theGridX, 0, kColumnCount - 1);
	}

	int ClampGridY(int theGridY) const
	{
		return Clamp(theGridY, 0, GetActiveRowCount() - 1);
	}

	int PixelToGridX(int theX) const
	{
		if (theX < kLawnOriginX)
			return -1;

		return ClampGridX((theX - kLawnOriginX) / kColumnWidth);
	}

	int PixelToGridY(int theX, int theY) const
	{
		const int aGridX = PixelToGridX(theX);
		if (aGridX == -1 || theY < kLawnOriginY)
			return -1;

		if (mGridLayout == BoardGridLayout::Roof && aGridX < kRoofSlopedColumnCount)
			theY -= (kRoofSlopedColumnCount - 1 - aGridX) * kRoofSlopePerColumn;

		return ClampGridY((theY - kLawnOriginY) / GetRowHeight());
	}

	int GridToPixelX(int theGridX) const
	{
		return theGridX * kColumnWidth + kLawnOriginX;
	}

	int GridToPixelY(int theGridX, int theGridY, bool theHighGround = false) const
	{
		int aY;
		if (mGridLayout == BoardGridLayout::Roof)
		{
			const int aSlopeOffset = theGridX < kRoofSlopedColumnCount
				? (kRoofSlopedColumnCount - theGridX) * kRoofSlopePerColumn
				: 0;
			aY = theGridY * kRoofRowHeight + aSlopeOffset + kLawnOriginY + kRoofGridTopAdjustment;
		}
		else
		{
			aY = theGridY * GetRowHeight() + kLawnOriginY;
		}

		if (theHighGround)
			aY -= kHighGroundHeight;
		return aY;
	}

	float GetRoofSlopeOffset(float theLocalX) const
	{
		if (mGridLayout != BoardGridLayout::Roof || theLocalX >= kRoofSlopeEndX)
			return 0.0f;
		return (kRoofSlopeEndX - theLocalX) * kRoofSlopePerPixel;
	}

	Sexy::Rect GetCellBounds(int theGridX, int theGridY, bool theHighGround = false) const
	{
		return Sexy::Rect(
			GridToPixelX(theGridX),
			GridToPixelY(theGridX, theGridY, theHighGround),
			kColumnWidth,
			GetRowHeight());
	}
};
