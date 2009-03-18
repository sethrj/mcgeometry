/*!
 * \file Plane.i.hpp
 * \brief  Contains inline implementation for \c Plane and \c PlaneNormal
 * \author Seth R. Johnson
 */

#ifndef MCG_PLANE_I_HPP
#define MCG_PLANE_I_HPP

#include <algorithm>
#include <iostream>

#include <blitz/tinyvec-et.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Plane.hpp"
#include "Surface.i.hpp"

namespace mcGeometry {
/*============================================================================*/
/*----------------------------------------------------------------------------*/
//! calculate distance to intersection
inline void Plane::intersect(
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
inline bool Plane::hasPosSense(const TVecDbl& position) const
{
//    double eval = blitz::sum( _normal * position - _normal * _coordinate);
    double eval = blitz::dot(_normal, position)
                 - blitz::dot(_normal, _coordinate);

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
inline void Plane::normalAtPoint(
                    const TVecDbl& position,
                    TVecDbl& unitNormal) const
{
    // "position" should be on the surface
    Require(softEquiv(blitz::dot(_normal, position)
                        - blitz::dot(_normal, _coordinate), 0.0));
    
    unitNormal = _normal;
}
/*----------------------------------------------------------------------------*/
inline std::ostream& Plane::_output( std::ostream& os ) const
{
    os  << "[ PLANE  Point:  " << std::setw(10) << _coordinate 
        << " Normal vector: " << std::setw(10) << _normal << " ]";
    return os;
}
/*============================================================================*/
template<unsigned int axis>
inline bool PlaneNormal<axis>::hasPosSense(
                    const TVecDbl& position) const
{
    return _hasPosSense(position[axis] - _coordinate);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void PlaneNormal<axis>::intersect(
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
inline void PlaneNormal<axis>::normalAtPoint(
                    const TVecDbl& position,
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
inline std::ostream& PlaneNormal<0>::_output( std::ostream& os ) const {
    os  << "[ PLANE X  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
inline std::ostream& PlaneNormal<1>::_output( std::ostream& os ) const {
    os  << "[ PLANE Y  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
inline std::ostream& PlaneNormal<2>::_output( std::ostream& os ) const {
    os  << "[ PLANE Z  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif 

