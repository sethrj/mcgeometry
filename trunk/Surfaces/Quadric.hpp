

#ifndef QUADRIC_HPP
#define QUADRIC_HPP

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include "transupport/dbc.hpp"

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

    //! create a copy of 
//    virtual clone() = 0;

    // TODO: also need a method to calculate whether a point has a positive 
    // sense to this quadric without doing all the distance calc
    virtual bool hasPosSense(std::vector<double>& position) const = 0;

    // This allows operator<< to have access to the private and protected 
    // members for writing to output.  Of course care must be taken that these
    // members are not changed.
    friend std::ostream& operator<<( std::ostream&, const Quadric& );

    // NOTE: this must be public if we ever have a generic Quadric
    virtual ~Quadric()  { /* * */ }

protected:
    Quadric() { /* * */ }

    bool _hasPosSense(double eval) const;
    HitAndDist _calcQuadraticIntersect( double A, double B, double C,
            bool posSense ) const;
};
/*----------------------------------------------------------------------------*/

// Defining this function here gives us the ability to make the same decision 
// for all Quadric surfaces in one place.
inline bool Quadric::_hasPosSense(double eval) const{
    if ( 0 <= eval )
        return true;        // Positive sense includes points "on" the surface
    else
        return false;       // Negative sense
}

inline Quadric::HitAndDist Quadric::_calcQuadraticIntersect(
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

