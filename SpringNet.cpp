#include "SpringNet.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <map>

#include "Geometry.hpp"





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





Point & Spring::point1()
{
	return mParentNet.point(mPointIdx1);
}





Point & Spring::point2()
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





double Spring::distanceSquared(QPointF aPt)
{
	return Geometry::distanceSquared(aPt, point1(), point2());
}





///////////////////////////////////////////////////////////////////////////////
// SpringNet:

SpringNet::SpringNet()
{

}





void SpringNet::addPoint(QPointF aPos, bool aIsFixed)
{
	mPoints.push_back(std::make_shared<Point>(aPos, aIsFixed));
}





void SpringNet::addSpring(double aIdealLength, double aForce, size_t aPointIdx1, size_t aPointIdx2)
{
	mSprings.push_back(std::make_shared<Spring>(*this, aIdealLength, aForce, aPointIdx1, aPointIdx2));
}





size_t SpringNet::nearestPointIdx(QPointF aQueryPt)
{
	if (mPoints.empty())
	{
		throw std::runtime_error("No points to query");
	}
	auto num = mPoints.size();
	auto res = 0;
	auto minDist = Geometry::distanceSquared(*mPoints[0], aQueryPt);
	for (size_t idx = 1; idx < num; ++idx)
	{
		auto dist = Geometry::distanceSquared(*mPoints[idx], aQueryPt);
		if (dist < minDist)
		{
			minDist = dist;
			res = idx;
		}
	}
	return res;
}





size_t SpringNet::nearestSpringIdx(QPointF aQueryPt)
{
	if (mSprings.empty())
	{
		throw std::runtime_error("No springs to query");
	}
	auto num = mSprings.size();
	auto res = 0;
	auto minDist = mSprings[0]->distanceSquared(aQueryPt);
	for (size_t idx = 1; idx < num; ++idx)
	{
		auto dist = mSprings[idx]->distanceSquared(aQueryPt);
		if (dist < minDist)
		{
			minDist = dist;
			res = idx;
		}
	}
	return res;
}





void SpringNet::clear()
{
	mSprings.clear();
	mPoints.clear();
}





void SpringNet::adjust()
{
	std::map<size_t, std::vector<size_t>> springsAtPoints;  // ptIDx -> [springIdx, ...]
	auto numS = mSprings.size();
	for (size_t idx = 0; idx < numS; ++idx)
	{
		const auto & s = mSprings[idx];
		springsAtPoints[s->pointIdx1()].push_back(idx);
		springsAtPoints[s->pointIdx2()].push_back(idx);
	}

	auto newPoints = mPoints;
	for (const auto & s: springsAtPoints)
	{
		auto ptIdx = s.first;
		const auto & pt = mPoints[ptIdx];
		if (pt->isFixed())
		{
			continue;
		}

		double nx = pt->x(), ny = pt->y();
		for (const auto sIdx: s.second)
		{
			const auto & spring = mSprings[sIdx];
			auto lenDif = spring->idealLength() - spring->currentLength();
			// For movable points divide the difference between the two points:
			if (!spring->otherPoint(ptIdx).isFixed())
			{
				lenDif = lenDif / 2;
			}

			if (spring->pointIdx1() != ptIdx)
			{
				lenDif = -lenDif;
			}
			nx += spring->diffX() * lenDif * spring->force() / spring->idealLength();
			ny += spring->diffY() * lenDif * spring->force() / spring->idealLength();
		}
		newPoints[ptIdx]->set(nx, ny);
	}
	mPoints = newPoints;
}




std::pair<bool, size_t> SpringNet::snapToPoint(QPointF aQueryPt, double aPointSnapDistSq)
{
	if (mPoints.empty())
	{
		return {false, 0};
	}
	auto nearestPtIdx = nearestPointIdx(aQueryPt);
	const auto & pt = point(nearestPtIdx);
	auto lenSq = Geometry::distanceSquared(aQueryPt, pt);
	return {(lenSq < aPointSnapDistSq), nearestPtIdx};
}





std::pair<SpringNet::ObjectType, size_t> SpringNet::nearestObject(QPointF aScenePos, double aSnapDistSq)
{
	if (mPoints.empty())
	{
		return {ObjectType::None, 0};
	}
	auto ptIdx = nearestPointIdx(aScenePos);
	if (mSprings.empty())
	{
		return {ObjectType::Point, ptIdx};
	}
	auto ptDistSq = Geometry::distanceSquared(aScenePos, *mPoints[ptIdx]);
	auto springIdx = nearestSpringIdx(aScenePos);
	auto springDistSq = Geometry::distanceSquared(aScenePos, mSprings[springIdx]->point1(), mSprings[springIdx]->point2());
	if (ptDistSq < aSnapDistSq)
	{
		return {ObjectType::Point, ptIdx};
	}
	if (springDistSq < aSnapDistSq)
	{
		return {ObjectType::Spring, springIdx};
	}
	if (springDistSq < ptDistSq)
	{
		return {ObjectType::Spring, springIdx};
	}
	else
	{
		return {ObjectType::Point, ptIdx};
	}
}






void SpringNet::removePoint(size_t aIdx)
{
	if (aIdx >= mPoints.size())
	{
		throw std::runtime_error("Point index out of bounds.");
	}

	// Remove all springs connected to the point:
	std::erase_if(mSprings, [aIdx](const SpringPtr & aSpring)
		{
			return ((aSpring->pointIdx1() == aIdx) || (aSpring->pointIdx2() == aIdx));
		}
	);

	// Shift down all point indices within springs:
	for (auto & spring: mSprings)
	{
		if (spring->pointIdx1() > aIdx)
		{
			spring->setPointIdx1(spring->pointIdx1() - 1);
		}
		if (spring->pointIdx2() > aIdx)
		{
			spring->setPointIdx2(spring->pointIdx2() - 1);
		}
	}

	// Remove the point:
	mPoints.erase(mPoints.begin() + aIdx);
}





void SpringNet::removeSpring(size_t aIdx)
{
	if (aIdx >= mSprings.size())
	{
		throw std::runtime_error("Spring index out of bounds.");
	}
	mSprings.erase(mSprings.begin() + aIdx);
}
