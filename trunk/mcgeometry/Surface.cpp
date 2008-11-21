/*!
 * \file Surface.cpp
 * \brief The parent abstract class of all the other surfaces
 * \author Jeremy L. Conlin and Seth R. Johnson
 * 
 */

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include "transupport/dbc.hpp"
#include "Surface.hpp"

namespace mcGeometry {
/*----------------------------------------------------------------------------*/

void Surface::_calcQuadraticIntersect(
        const double A, const double B, const double C, const bool posSense,
        bool& particleHitsSurface, double& distanceToIntercept) const
{
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
}

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
