#pragma once

#include <vector>
#include <QPointF>





// fwd:
class SpringNet;





/** Represents a single point, either a fixed one or a moving one, that can define a spring endpoint. */
class Point:
	public QPointF
{
	using Super = QPointF;

	/** A fixed point cannot be moved at all. */
	bool mIsFixed;


public:

	Point(double aX, double aY, bool aIsFixed = false):
		Super(aX, aY),
		mIsFixed(aIsFixed)
	{
	}

	Point(const QPointF & aPos, bool aIsFixed = false):
		Super(aPos),
		mIsFixed(aIsFixed)
	{
	}

	bool isFixed() const { return mIsFixed; }

	void set(double aX, double aY)
	{
		setX(aX);
		setY(aY);
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

	/** Returns the square of the distance between the specified point and the spring. */
	double distanceSquared(QPointF aPt);
};





class SpringNet
{
	std::vector<Point> mPoints;
	std::vector<Spring> mSprings;


public:

	/** Object type, for functions handling multiple object types. */
	enum class ObjectType
	{
		None,
		Point,
		Spring,
	};


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
	size_t nearestPointIdx(QPointF aQueryPt);

	/** Returns the index of the point nearest to the specified coords.
	Throws a std::runtime_error if there are no points in the network. */
	size_t nearestSpringIdx(QPointF aQueryPt);

	/** Removes everything from the containers. */
	void clear();

	/** Performs one round of spring-based point position adjustment. */
	void adjust();

	/** Returns {true, ptIdx} when the query position is within snap distance of a point,
	{false, ?} if too far or no points. */
	std::pair<bool, size_t> snapToPoint(QPointF aQueryPt, double aPointSnapDistSq);

	/** Returns the object nearest to the specified position. */
	std::pair<ObjectType, size_t> nearestObject(QPointF aScenePos, double aSnapDistSq);

};
