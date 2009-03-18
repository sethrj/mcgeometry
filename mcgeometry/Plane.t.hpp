/*!
 * \file Plane.t.hpp
 * \brief Non-inlined code for plane normal (templated on axies)
 * \author Seth R. Johnson
 */

#ifndef MCG_PLANE_T_HPP
#define MCG_PLANE_T_HPP

#include <vector>
#include <algorithm>
#include <iostream>
#include <blitz/tinyvec-et.h>
#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Surface.hpp"

namespace mcGeometry {
/*============================================================================*/
template<unsigned int axis>
bool PlaneNormal<axis>::hasPosSense(
                    const TVecDbl& position) const
{
    return _hasPosSense(position[axis] - _coordinate);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void PlaneNormal<axis>::intersect(
                    const TVecDbl& position, 
                    const TVecDbl& direction,
                    const bool posSense,
                    bool& hit, double& distance) const
{
    Require(tranSupport::checkDirectionVector(direction));

    // default to "does not hit" values
    hit = false;
    distance = 0.0;

    if (    ((posSense == false) && (direction[axis] > 0))
         || ((posSense == true)  && (direction[axis] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[axis]) / direction[axis];
        distance = std::max(0.0, distance);
    }
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void PlaneNormal<axis>::normalAtPoint( const TVecDbl& position,
                                       TVecDbl& unitNormal) const
{
    // "position" should be on the surface
    Require(softEquiv(position[axis] - _coordinate, 0.0));

    unitNormal = 0.0;
    unitNormal[axis] = 1.0;
}

/*----------------------------------------------------------------------------*/
// template specialize 0, 1, and 2
// but don't provide generic so that the compiler will fail if axis > 2
template<>
std::ostream& PlaneNormal<0>::_output( std::ostream& os ) const {
    os  << "[ PLANE X  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
std::ostream& PlaneNormal<1>::_output( std::ostream& os ) const {
    os  << "[ PLANE Y  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
std::ostream& PlaneNormal<2>::_output( std::ostream& os ) const {
    os  << "[ PLANE Z  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif 

