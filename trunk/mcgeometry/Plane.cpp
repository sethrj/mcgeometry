/*!
 * \file Plane.cpp
 * \brief Object code for Planes that don't need to be inlined 
 * \author Seth R. Johnson
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

#include "Plane.hpp"

namespace mcGeometry {
/*============================================================================*/
/*----------------------------------------------------------------------------*/
//! calculate distance to intersection
void Plane::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    // default to "does not hit" values
    hit = false;
    distance = 0.0;

    double cosine = tranSupport::vectorDot3(_normal, direction);

    if ( ((posSense == false) && (cosine > 0))
         || ((posSense == true) and (cosine < 0)) )
    {
        // Headed towards surface and hits it
        hit = true;
        for( int i = 0; i < 3; ++i ) {
            distance += _normal[i]*(_coordinate[i] - position[i]);
        }
        distance = std::max(0.0, distance/cosine);
    }
}

/*----------------------------------------------------------------------------*/
// Information taken from http://mathworld.wolfram.com/Plane.html
// Equation: n.p - n.p0     n = normal vector to plane
//      p = position
//      p0 = point on plane
bool Plane::hasPosSense(const std::vector<double>& position) const{
    Require(position.size() == 3);
    double eval(0);
    for( int i = 0; i < 3; ++i ){
        eval += _normal[i]*position[i] - _normal[i]*_coordinate[i];
    }

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
void Plane::normalAtPoint(  const std::vector<double>& position,
                            std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);
    // "position" should be on the surface
    IfDbc(  double eval(0);
            for( int i = 0; i < 3; ++i )
                eval += _normal[i]*position[i] - _normal[i]*_coordinate[i];
         );
    Require(softEquiv(eval, 0.0));
    
    unitNormal = _normal;
}
/*----------------------------------------------------------------------------*/
std::ostream& Plane::_output( std::ostream& os ) const {
    os  << "[ PLANE  Point:  " << std::setw(10) << _coordinate 
        << " Normal vector: " << std::setw(10) << _normal << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry

