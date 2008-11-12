

#ifndef QUADRIC_HPP
#define QUADRIC_HPP

#include <vector>
#include <utility>
#include <cmath>
#include "transupport/dbc.hpp"
//#include <iostream>

/*----------------------------------------------------------------------------*/
class Quadric {
public:
    typedef std::pair<bool, double> HitAndDist;

    // Intersect will determine if the particle at Postion and Direction
    // will intersect with the surface.  It returns a std::pair HitAndDist
    // It assumes that the PosSense defines whether the particles thinks it
    // has a positive sense with respect to the surface
    virtual HitAndDist intersect(const std::vector<double>& position, 
                                 const std::vector<double>& direction,
                                 bool PosSense) const = 0;

    // TODO: also need a method to calculate whether a point has a positive 
    // sense to this quadric without doing all the distance calc
    // bool hasPosSense(std::vector<double>& position) const = 0;

//        friend std::ostream& operator<<( std::ostream& os, QuadricSurface );

protected:
    Quadric() { /* * */ }
    virtual ~Quadric()  { /* * */ }

    HitAndDist calcQuadraticIntersect( double A, double B, double C,
            bool posSense ) const;
};
/*----------------------------------------------------------------------------*/

inline Quadric::HitAndDist Quadric::calcQuadraticIntersect(
        double A, double B, double C, bool posSense) const
{
	bool particleHitsSurface = false;
	double distanceToIntercept = -1.0;

	double Q = B*B - A*C;

	if (Q < 0) {
		particleHitsSurface = false;
		distanceToIntercept = 0.0;
	} 
    else {
        if (not posSense) { //inside the surface (negative orientation)
			if (B <= 0) {   // headed away from the surface
				if (A > 0) {    // surface is curving upward
					particleHitsSurface = true;
					distanceToIntercept = (std::sqrt(Q) - B)/A;
				} 
                else {  // surface curving away and headed in, never hits it
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				}
			} 
            else {  // particle is heading toward the surface
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, -C/(std::sqrt(Q) + B));
			}
		} 
        else {  // particle is outside
			if (B >= 0) {   // particle headed away
				if (A >= 0) {
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				} 
                else {
					particleHitsSurface = true;
					distanceToIntercept = -(std::sqrt(Q) + B)/A;
				}
			} 
            else {
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, C/(std::sqrt(Q) - B));
			}
		}
	}
    Ensure( distanceToIntercept >= 0.0 );

	return std::make_pair(particleHitsSurface, distanceToIntercept);
}

/*----------------------------------------------------------------------------*/

#endif

