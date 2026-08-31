#include "ViewportTransform.h"

#include <algorithm>
#include <cmath>

namespace Sexy
{

ViewportTransform::ViewportTransform()
{
	Reset();
}

ViewportTransform::ViewportTransform(
	int theLogicalWidth,
	int theLogicalHeight,
	int theOutputWidth,
	int theOutputHeight,
	ViewportScaleMode theScaleMode)
{
	Reset();
	Configure(theLogicalWidth, theLogicalHeight, theOutputWidth, theOutputHeight, theScaleMode);
}

bool ViewportTransform::Configure(
	int theLogicalWidth,
	int theLogicalHeight,
	int theOutputWidth,
	int theOutputHeight,
	ViewportScaleMode theScaleMode)
{
	if (theLogicalWidth <= 0 || theLogicalHeight <= 0 ||
		theOutputWidth <= 0 || theOutputHeight <= 0)
	{
		Reset();
		return false;
	}

	mLogicalWidth = theLogicalWidth;
	mLogicalHeight = theLogicalHeight;
	mOutputWidth = theOutputWidth;
	mOutputHeight = theOutputHeight;
	mScaleMode = theScaleMode;

	const double aScaleX = static_cast<double>(theOutputWidth) / theLogicalWidth;
	const double aScaleY = static_cast<double>(theOutputHeight) / theLogicalHeight;

	if (theScaleMode == ViewportScaleMode::Stretch)
	{
		mScaleX = aScaleX;
		mScaleY = aScaleY;
		mPresentationRect = FRect(0.0, 0.0, theOutputWidth, theOutputHeight);
		return true;
	}

	double aScale = theScaleMode == ViewportScaleMode::Fill
		? (std::max)(aScaleX, aScaleY)
		: (std::min)(aScaleX, aScaleY);
	if (theScaleMode == ViewportScaleMode::IntegerFit && aScale >= 1.0)
		aScale = (std::max)(1.0, std::floor(aScale));

	mScaleX = aScale;
	mScaleY = aScale;
	const double aPresentationWidth = theLogicalWidth * aScale;
	const double aPresentationHeight = theLogicalHeight * aScale;
	mPresentationRect = FRect(
		(theOutputWidth - aPresentationWidth) * 0.5,
		(theOutputHeight - aPresentationHeight) * 0.5,
		aPresentationWidth,
		aPresentationHeight
	);
	return true;
}

void ViewportTransform::Reset()
{
	mLogicalWidth = 0;
	mLogicalHeight = 0;
	mOutputWidth = 0;
	mOutputHeight = 0;
	mScaleX = 0.0;
	mScaleY = 0.0;
	mScaleMode = ViewportScaleMode::Fit;
	mPresentationRect = FRect();
}

bool ViewportTransform::IsValid() const
{
	return mLogicalWidth > 0 && mLogicalHeight > 0 &&
		mOutputWidth > 0 && mOutputHeight > 0 &&
		mScaleX > 0.0 && mScaleY > 0.0;
}

bool ViewportTransform::IsPhysicalPointInViewport(double theX, double theY) const
{
	if (!IsValid() || theX < 0.0 || theY < 0.0 ||
		theX >= mOutputWidth || theY >= mOutputHeight)
	{
		return false;
	}

	const FPoint aLogicalPoint = PhysicalToLogicalUnclamped(theX, theY);
	return aLogicalPoint.mX >= 0.0 && aLogicalPoint.mY >= 0.0 &&
		aLogicalPoint.mX < mLogicalWidth && aLogicalPoint.mY < mLogicalHeight;
}

FPoint ViewportTransform::LogicalToPhysical(double theX, double theY) const
{
	if (!IsValid())
		return FPoint();
	return FPoint(
		mPresentationRect.mX + theX * mScaleX,
		mPresentationRect.mY + theY * mScaleY
	);
}

FPoint ViewportTransform::LogicalToPhysical(const FPoint& thePoint) const
{
	return LogicalToPhysical(thePoint.mX, thePoint.mY);
}

FRect ViewportTransform::LogicalToPhysical(const FRect& theRect) const
{
	const FPoint aTopLeft = LogicalToPhysical(theRect.mX, theRect.mY);
	return FRect(
		aTopLeft.mX,
		aTopLeft.mY,
		theRect.mWidth * mScaleX,
		theRect.mHeight * mScaleY
	);
}

bool ViewportTransform::PhysicalToLogical(double theX, double theY, FPoint& thePoint) const
{
	thePoint = PhysicalToLogicalUnclamped(theX, theY);
	return IsPhysicalPointInViewport(theX, theY);
}

FPoint ViewportTransform::PhysicalToLogicalUnclamped(double theX, double theY) const
{
	if (!IsValid())
		return FPoint();
	return FPoint(
		(theX - mPresentationRect.mX) / mScaleX,
		(theY - mPresentationRect.mY) / mScaleY
	);
}

FPoint ViewportTransform::PhysicalToLogicalClamped(double theX, double theY) const
{
	FPoint aPoint = PhysicalToLogicalUnclamped(theX, theY);
	if (!IsValid())
		return aPoint;
	aPoint.mX = (std::clamp)(aPoint.mX, 0.0, static_cast<double>(mLogicalWidth));
	aPoint.mY = (std::clamp)(aPoint.mY, 0.0, static_cast<double>(mLogicalHeight));
	return aPoint;
}

FRect ViewportTransform::PhysicalToLogical(const FRect& theRect) const
{
	const FPoint aTopLeft = PhysicalToLogicalUnclamped(theRect.mX, theRect.mY);
	if (!IsValid())
		return FRect();
	return FRect(
		aTopLeft.mX,
		aTopLeft.mY,
		theRect.mWidth / mScaleX,
		theRect.mHeight / mScaleY
	);
}

int ViewportTransform::GetLogicalWidth() const
{
	return mLogicalWidth;
}

int ViewportTransform::GetLogicalHeight() const
{
	return mLogicalHeight;
}

int ViewportTransform::GetOutputWidth() const
{
	return mOutputWidth;
}

int ViewportTransform::GetOutputHeight() const
{
	return mOutputHeight;
}

double ViewportTransform::GetScaleX() const
{
	return mScaleX;
}

double ViewportTransform::GetScaleY() const
{
	return mScaleY;
}

ViewportScaleMode ViewportTransform::GetScaleMode() const
{
	return mScaleMode;
}

const FRect& ViewportTransform::GetPresentationRect() const
{
	return mPresentationRect;
}

FRect ViewportTransform::GetVisibleLogicalRect() const
{
	if (!IsValid())
		return FRect();

	FRect aVisibleRect = PhysicalToLogical(FRect(0.0, 0.0, mOutputWidth, mOutputHeight));
	const double aLeft = (std::max)(0.0, aVisibleRect.mX);
	const double aTop = (std::max)(0.0, aVisibleRect.mY);
	const double aRight = (std::min)(static_cast<double>(mLogicalWidth), aVisibleRect.mX + aVisibleRect.mWidth);
	const double aBottom = (std::min)(static_cast<double>(mLogicalHeight), aVisibleRect.mY + aVisibleRect.mHeight);
	return FRect(aLeft, aTop, (std::max)(0.0, aRight - aLeft), (std::max)(0.0, aBottom - aTop));
}

}
