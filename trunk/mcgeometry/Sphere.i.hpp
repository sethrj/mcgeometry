/*!
 * \file   Sphere.i.hpp
 * \brief  Contains inline implementation for the \c Sphere class
 * \author Seth R. Johnson
 */

#ifndef MCG_SPHERE_I_HPP
#define MCG_SPHERE_I_HPP

#include <iostream>
#include <blitz/tinyvec-et.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Sphere.hpp"
#include "Surface.i.hpp"

namespace mcGeometry {
/*============================================================================*/

/*----------------------------------------------------------------------------*/
// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
inline bool Sphere::hasPosSense(const TVecDbl& position) const
{
    double eval = 0.0;
    double temp;

    temp = position[0]-_center[0];
    eval += temp*temp;

    temp = position[1]-_center[1];
    eval += temp*temp;

    temp = position[2]-_center[2];
    eval += temp*temp;

    eval -= _radius*_radius;

    return _hasPosSense(eval);
}

/*----------------------------------------------------------------------------*/
inline void Sphere::intersect(
        const TVecDbl& position, 
        const TVecDbl& direction, const bool posSense,
        bool& hit, double& distance ) const
{
    Require(tranSupport::checkDirectionVector(direction));

    TVecDbl trLoc(position - _center);

    // find distance and whether it intercepts
    _calcQuadraticIntersect(
            1,  // A
            blitz::dot(trLoc, direction),  // B
            (blitz::dot(trLoc, trLoc) - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
inline void Sphere::normalAtPoint(
        const TVecDbl& position,
        TVecDbl& unitNormal ) const
{
    // (position is on the outer edge of the sphere, we hope)
    Require(softEquiv(tranSupport::vectorNorm(position - _center),_radius));

    // for a sphere, normal is a line from "position" to the origin
    unitNormal = position - _center;
    double normValue = tranSupport::vectorNorm(unitNormal);

    unitNormal /= normValue;

    // make sure it actually is a normal vector
    Ensure(tranSupport::checkDirectionVector(unitNormal));
}

/*----------------------------------------------------------------------------*/
//! output a stream which prints the Sphere's characteristics
inline std::ostream& Sphere::_output( std::ostream& os ) const
{
    os  << "[ SPHERE Center: " << std::setw(10) << _center
        << " Radius: " << std::setw(5) << _radius
        << " ]";
    return os;
}

/*============================================================================*/

/*----------------------------------------------------------------------------*/
// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
inline bool SphereO::hasPosSense(
        const TVecDbl& position ) const
{
    return _hasPosSense(
              blitz::dot(position, position) - _radius*_radius);
}

/*----------------------------------------------------------------------------*/
inline void SphereO::intersect(
        const TVecDbl& position, 
        const TVecDbl& direction, const bool posSense,
        bool& hit, double& distance ) const
{
    Require(tranSupport::checkDirectionVector(direction));

    // find distance and whether it intercepts
    _calcQuadraticIntersect(
            1,  // A
            blitz::dot(position, direction),  // B
            (blitz::dot(position, position) - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
inline void SphereO::normalAtPoint(
        const TVecDbl& position,
        TVecDbl& unitNormal ) const
{
    // (position is on the outer edge of the sphere, we hope)
    Require(softEquiv(tranSupport::vectorNorm(position),_radius));

    unitNormal = position / blitz::dot(position, position);

    // make sure it actually is a normal vector
    Ensure(tranSupport::checkDirectionVector(unitNormal));
}
/*----------------------------------------------------------------------------*/
//! output a stream which prints the SphereO's characteristics
inline std::ostream& SphereO::_output( std::ostream& os ) const
{
    os  << "[ SPHEREO Radius: " << std::setw(5) << _radius
        << " ]";
    return os;
}

/*============================================================================*/
} // end namespace mcGeometry
#endif

