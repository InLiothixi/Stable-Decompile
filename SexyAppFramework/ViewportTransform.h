#pragma once

#include "Point.h"
#include "Rect.h"

namespace Sexy
{

// Controls how a stable logical coordinate system is presented on a physical
// render target.  Gameplay and widgets can continue to use logical units while
// the renderer allocates targets at the actual output resolution.
enum class ViewportScaleMode
{
	Fit,			// Preserve the aspect ratio and letterbox/pillarbox as needed.
	IntegerFit,	// Use an integer scale when possible, otherwise fall back to Fit.
	Fill,		// Preserve the aspect ratio and crop the excess.
	Stretch		// Fill the output without preserving the aspect ratio.
};

class ViewportTransform
{
public:
	ViewportTransform();
	ViewportTransform(
		int theLogicalWidth,
		int theLogicalHeight,
		int theOutputWidth,
		int theOutputHeight,
		ViewportScaleMode theScaleMode = ViewportScaleMode::Fit
	);

	// Returns false and resets the transform if any dimension is invalid.
	bool Configure(
		int theLogicalWidth,
		int theLogicalHeight,
		int theOutputWidth,
		int theOutputHeight,
		ViewportScaleMode theScaleMode = ViewportScaleMode::Fit
	);
	void Reset();

	bool IsValid() const;
	bool IsPhysicalPointInViewport(double theX, double theY) const;

	FPoint LogicalToPhysical(double theX, double theY) const;
	FPoint LogicalToPhysical(const FPoint& thePoint) const;
	FRect LogicalToPhysical(const FRect& theRect) const;

	// The unclamped overload is useful for pointer capture and dragging.  It
	// returns false for letterbox bars or points outside the physical output,
	// but still reports the corresponding logical coordinate.
	bool PhysicalToLogical(double theX, double theY, FPoint& thePoint) const;
	FPoint PhysicalToLogicalUnclamped(double theX, double theY) const;
	FPoint PhysicalToLogicalClamped(double theX, double theY) const;
	FRect PhysicalToLogical(const FRect& theRect) const;

	int GetLogicalWidth() const;
	int GetLogicalHeight() const;
	int GetOutputWidth() const;
	int GetOutputHeight() const;
	double GetScaleX() const;
	double GetScaleY() const;
	ViewportScaleMode GetScaleMode() const;
	const FRect& GetPresentationRect() const;

	// Portion of the logical canvas visible in the output.  This is the full
	// logical canvas for Fit and may be smaller for Fill.
	FRect GetVisibleLogicalRect() const;

private:
	int mLogicalWidth;
	int mLogicalHeight;
	int mOutputWidth;
	int mOutputHeight;
	double mScaleX;
	double mScaleY;
	ViewportScaleMode mScaleMode;
	FRect mPresentationRect;
};

}
