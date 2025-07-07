#include "SpringNet.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <map>





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





double Spring::currentLength() const
{
	auto pt1 = point1();
	auto pt2 = point2();
	auto dx = pt1.x() - pt2.x();
	auto dy = pt1.y() - pt2.y();
	return std::sqrt(dx * dx + dy * dy);
}





double Spring::diffX() const
{
	return point1().x() - point2().x();
}





double Spring::diffY() const
{
	return point1().y() - point2().y();
}





const Point & Spring::otherPoint(size_t aPointIdx) const
{
	if (aPointIdx == mPointIdx1)
	{
		return point1();
	}
	else
	{
		return point2();
	}
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





void SpringNet::adjust()
{
	std::map<size_t, std::vector<size_t>> springsAtPoints;  // ptIDx -> [springIdx, ...]
	auto numS = mSprings.size();
	for (size_t idx = 0; idx < numS; ++idx)
	{
		const auto & s = mSprings[idx];
		springsAtPoints[s.pointIdx1()].push_back(idx);
		springsAtPoints[s.pointIdx2()].push_back(idx);
	}

	auto newPoints = mPoints;
	for (const auto & s: springsAtPoints)
	{
		auto ptIdx = s.first;
		const auto & pt = mPoints[ptIdx];
		if (pt.isFixed())
		{
			continue;
		}

		double nx = pt.x(), ny = pt.y();
		for (const auto sIdx: s.second)
		{
			const auto & spring = mSprings[sIdx];
			auto lenDif = spring.idealLength() - spring.currentLength();
			// For movable points divide the difference between the two points:
			if (!spring.otherPoint(ptIdx).isFixed())
			{
				lenDif = lenDif / 2;
			}

			if (spring.pointIdx1() != ptIdx)
			{
				lenDif = -lenDif;
			}
			nx += spring.diffX() * lenDif * spring.force() / spring.idealLength();
			ny += spring.diffY() * lenDif * spring.force() / spring.idealLength();
		}
		newPoints[ptIdx].set(nx, ny);
	}
	mPoints = newPoints;
}
