/*!
 * \file   Sphere.cpp
 * \brief  Non-inlined code for spheres
 * \author Jeremy L. Conlin
 */

#include <vector>
#include <cmath>
#include <iostream>
#include <blitz/tinyvec-et.h>
#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Surface.hpp"
#include "Sphere.hpp"

namespace mcGeometry {
/*============================================================================*/

/*----------------------------------------------------------------------------*/
// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
bool Sphere::hasPosSense(const TVecDbl& position) const
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
void Sphere::intersect(
        const TVecDbl& position, 
        const TVecDbl& direction, const bool posSense,
        bool& hit, double& distance ) const
{
    Require(tranSupport::checkDirectionVector(direction));

    double selfDot = 0.0;
    double B = 0.0;
    double movedLoc;
    
    // subtract out the center of the sphere
    for (int i = 0; i < 3; ++i) {
        movedLoc = (position[i] - _center[i]);
        B +=  movedLoc * direction[i]; 
        selfDot += movedLoc * movedLoc;
    }

    // find distance and whether it intercepts
    _calcQuadraticIntersect(
            1,  // A
            B,  // B
            (selfDot - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
void Sphere::normalAtPoint(
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
std::ostream& Sphere::_output( std::ostream& os ) const {
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
bool SphereO::hasPosSense(
        const TVecDbl& position ) const
{
    return _hasPosSense(
              position[0]*position[0]
            + position[1]*position[1]
            + position[2]*position[2] 
                - _radius*_radius);
}

/*----------------------------------------------------------------------------*/
void SphereO::intersect(
        const TVecDbl& position, 
        const TVecDbl& direction, const bool posSense,
        bool& hit, double& distance ) const
{
    Require(tranSupport::checkDirectionVector(direction));

    double selfDot = 0.0;
    double B = 0.0;
    
    // subtract out the center of the sphere
    for (int i = 0; i < 3; ++i) {
        B +=  position[i] * direction[i]; 
        selfDot += position[i] * position[i];
    }

    // find distance and whether it intercepts
    _calcQuadraticIntersect(
            1,  // A
            B,  // B
            (selfDot - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
void SphereO::normalAtPoint(
        const TVecDbl& position,
        TVecDbl& unitNormal ) const
{
    // (position is on the outer edge of the sphere, we hope)
    Require(softEquiv(tranSupport::vectorNorm(position),_radius));

    // for a sphere, normal is a line from "position" to the origin
    double normValue = 0.0;
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] = position[i];
        normValue += unitNormal[i] * unitNormal[i];
    }

    // normalize it
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] /= normValue;
    }

    // make sure it actually is a normal vector
    Ensure(tranSupport::checkDirectionVector(unitNormal));
}
/*----------------------------------------------------------------------------*/
//! output a stream which prints the SphereO's characteristics
std::ostream& SphereO::_output( std::ostream& os ) const {
    os  << "[ SPHEREO Radius: " << std::setw(5) << _radius
        << " ]";
    return os;
}

/*============================================================================*/
} // end namespace mcGeometry

