#pragma once

#include <vector>





// fwd:
class SpringNet;





class Point
{
	double mX, mY;

	/** A fixed point cannot be moved at all. */
	bool mIsFixed;

public:
	Point(double aX, double aY, bool aIsFixed = false):
		mX(aX),
		mY(aY),
		mIsFixed(aIsFixed)
	{
	}

	double x() const { return mX; }
	double y() const { return mY; }
	bool isFixed() const { return mIsFixed; }

	void set(double aX, double aY)
	{
		mX = aX;
		mY = aY;
	}
};





/** A spring between two points (specified as indices into the point array in SpringNet). */
class Spring
{
	SpringNet & mParentNet;
	double mIdealLength;
	double mForce;
	size_t mPointIdx1;
	size_t mPointIdx2;

public:
	Spring(SpringNet & aParentNet, double aIdealLength, double aForce, size_t aPointIdx1, size_t aPointIdx2):
		mParentNet(aParentNet),
		mIdealLength(aIdealLength),
		mForce(aForce),
		mPointIdx1(aPointIdx1),
		mPointIdx2(aPointIdx2)
	{
	}

	size_t pointIdx1() const { return mPointIdx1; }
	size_t pointIdx2() const { return mPointIdx2; }
	const Point & point1() const;
	const Point & point2() const;
	double idealLength() const { return mIdealLength; }
	double force() const { return mForce; }
	double currentLength() const;
	double diffX() const;
	double diffY() const;

	/** Returns the other point than the specified one.
	UB if aPointIdx is neither mPointIdx1 nor mPointIdx2. */
	const Point & otherPoint(size_t aPointIdx) const;

	/** Returns the length, projected from a sloped measurement onto a flat floor. */
	static double projectLengthToFloor(double aLength, double aHeightDifference);
};





class SpringNet
{
	std::vector<Point> mPoints;
	std::vector<Spring> mSprings;


public:

	SpringNet();

	const std::vector<Point> & points() const { return mPoints; }
	const std::vector<Spring> & springs() const { return mSprings; }

	size_t numPoints() const { return mPoints.size(); }
	size_t numSprings() const { return mSprings.size(); }

	const Point & point(size_t aIdx) const { return mPoints[aIdx]; }
	const Spring & spring(size_t aIdx) const { return mSprings[aIdx]; }

	/** Adds a new point with the specified properties, returns a reference to it. */
	const Point & addPoint(double aX, double aY, bool aIsFixed);

	/** Adds a new spring with the specified properties, returns a reference to it. */
	const Spring & addSpring(double aIdealLength, double aForce, size_t aPointIdx1, size_t aPointIdx2);

	/** Returns the index of the point nearest to the specified coords.
	Throws a std::runtime_error if there are no points in the network. */
	size_t nearestPointIdx(double aX, double aY);

	/** Returns the index of the point nearest to the specified coords.
	Throws a std::runtime_error if there are no points in the network. */
	size_t nearestSpringIdx(double aX, double aY);

	/** Performs one round of spring-based point position adjustment. */
	void adjust();
};
