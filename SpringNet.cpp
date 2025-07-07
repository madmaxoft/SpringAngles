#include "SpringNet.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>





///////////////////////////////////////////////////////////////////////////////
// Global:
namespace {

/** Returns the square of the distance between the two selected points. */
double distSquared(double aX1, double aY1, double aX2, double aY2)
{
	return ((aX1 - aX2) * (aX1 - aX2) + (aY1 - aY2) * (aY1 - aY2));
}

}





///////////////////////////////////////////////////////////////////////////////
// Spring:

const Point & Spring::point1() const
{
	return mParentNet.point(mPointIdx1);
}





const Point & Spring::point2() const
{
	return mParentNet.point(mPointIdx2);
}





double Spring::projectLengthToFloor(double aLength, double aHeightDifference)
{
	assert(aLength > aHeightDifference);

	return std::sqrt(aLength * aLength - aHeightDifference * aHeightDifference);
}





///////////////////////////////////////////////////////////////////////////////
// SpringNet:

SpringNet::SpringNet()
{

}





const Point & SpringNet::addPoint(double aX, double aY, bool aIsFixed)
{
	mPoints.emplace_back(aX, aY, aIsFixed);
	return mPoints.back();
}





const Spring & SpringNet::addSpring(double aIdealLength, double aForce, size_t aPointIdx1, size_t aPointIdx2)
{
	mSprings.emplace_back(*this, aIdealLength, aForce, aPointIdx1, aPointIdx2);
	return mSprings.back();
}





size_t SpringNet::nearestPointIdx(double aX, double aY)
{
	if (mPoints.empty())
	{
		throw std::runtime_error("No points to query");
	}
	auto num = mPoints.size();
	auto res = 0;
	auto minDist = distSquared(mPoints[0].x(), mPoints[0].y(), aX, aY);
	for (size_t idx = 1; idx < num; ++idx)
	{
		auto dist = distSquared(mPoints[idx].x(), mPoints[idx].y(), aX, aY);
		if (dist < minDist)
		{
			minDist = dist;
			res = idx;
		}
	}
	return res;
}
