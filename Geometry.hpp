#pragma once





namespace Geometry
{





/** Returns the square of the distance between two points, in whatever representation they are provided. */
template <typename T1, typename T2>
double distanceSquared(const T1 & aPt1, const T2 & aPt2)
{
	auto diffX = aPt1.x() - aPt2.x();
	auto diffY = aPt1.y() - aPt2.y();
	return diffX * diffX + diffY * diffY;
}





/** Returns the square of the distance between a point and a line, in whatever representation they are provided. */
template <typename T1, typename LT1, typename LT2>
double distanceSquared(const T1 & aPt, const LT1 & aLinePt1, const LT2 & aLinePt2)
{
	const double dx = aPt.x() - aLinePt1.x();  // vektor AX
	const double dy = aPt.y() - aLinePt1.y();
	const double px = aLinePt2.x() - aLinePt1.x();  // vektor AB
	const double py = aLinePt2.y() - aLinePt1.y();
	const double ss = px * dx + py * dy;  // skalarni soucin AX * AB
	const double ds = px * px + py * py;  // druha mocnina vzdalenosti

	if (ss < 0)
	{
		// vratit vzdalenost od bodu A ^2
		return dx * dx + dy * dy;
	}
	if (ss > ds)
	{
		// vratit vzdalenost od bodu B ^2
		return (aLinePt2.x() - aPt.x()) * (aLinePt2.x() - aPt.x()) + (aLinePt2.y() - aPt.y()) * (aLinePt2.y() - aPt.y());
	}

	// vratit vzdalenost od primky AB ^2
	if (std::abs(px * px + py * py) < 1e-30)
	{
		return dx * dx + dy * dy;
	}
	else
	{
		return (py * dx - px * dy) * (py * dx - px * dy) / (px * px + py * py);
	}
}





}  // namespace Geometry
