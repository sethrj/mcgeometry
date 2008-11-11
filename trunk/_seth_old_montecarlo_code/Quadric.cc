/*!
 * \file Quadric.hh
 * \brief Quadric surface geomerty
 * \author Seth R. Johnson
 * 
 */


#include <vector>
#include "transupport/dbc.hh"
#include "transupport/VectorComp.hh"
#include "transupport/VectorMath.hh"
#include "Quadric.hh"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace monteCarlo {

/*----------------------------------------------------------------------------*/
Quadric::HitAndDist Quadric::evalQuadric( const double A,
														const double B,
														const double C,
														const bool isLocPos)
{
	bool particleHitsSurface = false;
	double distanceToIntercept = -1.0;

	double Q = B*B - A*C;

	if (Q < 0) {
		particleHitsSurface = false;
		distanceToIntercept = 0.0;
	} else {
		if (isLocPos == false) { //inside the surface (negative orientation)
			if (B <= 0) {
				// headed away from the surface
				if (A > 0) {
					// surface is curving upward
					particleHitsSurface = true;
					distanceToIntercept = (std::sqrt(Q) - B)/A;
				} else {
					// surface curving away and headed in, never hits it
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				}
			} else {
				//particle is heading toward the surface
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, -C/(std::sqrt(Q) + B));
			}
		} else {
			//particle is outside
			if (B >= 0) {
				//particle headed away
				if (A >= 0) {
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				} else {
					particleHitsSurface = true;
					distanceToIntercept = -(std::sqrt(Q) + B)/A;
				}
			} else {
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, C/(std::sqrt(Q) - B));
			}
		}
	}
	Ensure( distanceToIntercept >= 0.0 );

	return std::make_pair(particleHitsSurface, distanceToIntercept);
}

/*============================================================================*/
/*============================================================================*/
bool Sphere::isLocPositive( const std::vector<double> &location) const
{
	double rSquared = 0.0;

	for (int i = 0; i < 3; ++i) {
		double temp = location[i] - m_center[i];
		rSquared += temp * temp;
	}

	return (rSquared > m_radius * m_radius);
}
/*============================================================================*/
Quadric::HitAndDist Sphere::getIntercept(
									const std::vector<double> &location,
									const std::vector<double> &direction,
									const bool isLocPos) const
{
	Require(direction.size() == 3);
	Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));
	Require(location.size() == 3);

	double selfDot = 0.0;
	double B = 0.0;
	double movedLoc;
	
	// subtract out the center of the sphere
	for (int i = 0; i < 3; ++i) {
		movedLoc = (location[i] - m_center[i]);
		B +=  movedLoc * direction[i]; 
		selfDot += movedLoc * movedLoc;
	}

	// find distance and whether it intercepts
	Quadric::HitAndDist theResult =
		Quadric::evalQuadric(
			1,  // A
			B,  // B
			selfDot - m_radius * m_radius, //C
			isLocPos);

	return theResult;
}

Sphere::~Sphere() {
	/* * */
//	cout << "I just got *destroyed*!" << endl;
}
/*----------------------------------------------------------------------------*/
}
