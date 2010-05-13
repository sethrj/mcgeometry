/*!
 * \file   Surface.cpp
 * \brief  Implementation of the \c Surface class
 * \author Jeremy L. Conlin and Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "Surface.hpp"

#include <algorithm>
#include <ostream>
#include "transupport/dbc.hpp"

namespace mcGeometry {
/*============================================================================*/
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

/*============================================================================*/
//! \brief Output a general Surface-descended surface
// for polymorphism, we have to call a inherited method
std::ostream& operator<<( std::ostream& os, const Surface& q)
{
    return q.printStream(os);
}

//! \brief Output a Surface and its corresponding sense
std::ostream& operator<<(
        std::ostream& os,
        const std::pair<Surface*, bool>& surfAndSense)
{
    if (surfAndSense.second == true) {
        os << "+";
    }
    else {
        os << "-";
    }

    os << surfAndSense.first->getUserId();
//    os << *(surfAndSense.first);

    return os;
}

/*============================================================================*/
} // end namespace mcGeometry

