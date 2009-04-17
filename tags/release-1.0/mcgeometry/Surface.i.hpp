/*!
 * \file Surface.i.hpp
 * \brief Implementation of the \c Surface class
 * \author Jeremy L. Conlin and Seth R. Johnson
 */

#ifndef MCG_SURFACE_I_HPP
#define MCG_SURFACE_I_HPP

#include <algorithm>

#include "Surface.hpp"

#include "transupport/dbc.hpp"

namespace mcGeometry {

/*----------------------------------------------------------------------------*/

//! Does the evaluated quadric equation result in a positive or negative sense?
// Defining this function here gives us the ability to make the same decision 
// for all Surface surfaces in one place.
inline bool Surface::_hasPosSense(const double eval) const{
    // Positive sense includes points "on" the surface (i.e. includes zero)
    // negative sense if the evaluated is strictly less than zero
    return ( eval >= 0 );
}
/*----------------------------------------------------------------------------*/
inline void Surface::_calcQuadraticIntersect(
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
/*----------------------------------------------------------------------------*/
#endif

