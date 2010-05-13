/*!
 * \file   Plane.cpp
 * \brief  Contains implementation for \c Plane
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "Plane.hpp"

#include <algorithm>
#include <ostream>

#include <blitz/tinyvec-et.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

namespace mcGeometry {
/*============================================================================*/
//! calculate distance to intersection
void Plane::intersect(
                const TVecDbl& position,
                const TVecDbl& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Require(tranSupport::checkDirectionVector(direction));

    // default to "does not hit" values
    hit = false;
    distance = 0.0;

    double cosine = blitz::dot(_normal, direction);

    if ( ((posSense == false) && (cosine > 0))
         || ((posSense == true) and (cosine < 0)) )
    {
        // Headed towards surface and hits it
        hit = true;
        distance = blitz::sum(_normal*(_coordinate - position));
        distance = std::max(0.0, distance/cosine);
    }
}

/*----------------------------------------------------------------------------*/
// Information taken from http://mathworld.wolfram.com/Plane.html
// Equation: n.p - n.p0     n = normal vector to plane
//      p = position
//      p0 = point on plane
bool Plane::hasPosSense(const TVecDbl& position) const
{
//    double eval = blitz::sum( _normal * position - _normal * _coordinate);
    double eval = blitz::dot(_normal, position)
                 - blitz::dot(_normal, _coordinate);

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
void Plane::normalAtPoint(
                    const TVecDbl&,
                    TVecDbl& unitNormal) const
{
    unitNormal = _normal;
}
/*----------------------------------------------------------------------------*/
std::ostream& Plane::printStream( std::ostream& os ) const
{
    os  << "[ PLANE  Point:  " << std::setw(10) << _coordinate
        << " Normal vector: " << std::setw(10) << _normal << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry

