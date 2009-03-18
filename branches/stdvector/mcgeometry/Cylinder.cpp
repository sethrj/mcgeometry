/*!
 * \file   Cylinder.cpp
 * \brief  Code that, due to polymorphism, has no point in being inlined.
 * \author Jeremy L. Conlin and Seth R. Johnson
 */

#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

#include "Surface.hpp"
#include "Cylinder.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace mcGeometry {
/*============================================================================*/
/*----------------------------------------------------------------------------*/
// Equation: (X-P)^2 - [(X-P).U]^2 - R^2 = 0
//      X = position 
//      P = point on axis of cylinder
//      U = direction vector of cylinder axis
bool Cylinder::hasPosSense(const std::vector<double>& position) const
{
    Require(position.size() == 3);
    double eval(0);
    double diff(0);
    double tmpValue(0);

    for( int i = 0; i < 3; ++i ){
        // Perform (X-P)^2
        diff = position[i] - _pointOnAxis[i];
        eval += diff*diff;

        // Perform [(X-P).U]^2
        tmpValue += diff*_axis[i];
    }

    eval -= tmpValue*tmpValue + _radius*_radius;

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
// TODO: optimization needed here
void Cylinder::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction,
        const bool posSense,
        bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    double A(0.0);   // 1-(direction._axis)^2
    for( int i = 0; i < 3; ++i ){
        A += direction[i]*_axis[i];
    }
    A = 1-A*A;

    double B(0.0);
    double b(0.0);      // Temporary variable
    std::vector<double> bvec(3,0.0);
    std::vector<double> diff(position);
    tranSupport::vectorMinusEq(diff, _pointOnAxis);
    b = tranSupport::vectorDot3(diff, _axis);
    for( int i = 0; i < 3; ++i ) bvec[i] = _axis[i]*b;
    tranSupport::vectorMinusEq(diff, bvec);
    B = tranSupport::vectorDot3(diff, direction);

    double C(0.0);
    double c(0.0);
    diff = position;
    tranSupport::vectorMinusEq(diff, _pointOnAxis);
    c = tranSupport::vectorDot3(diff, _axis);
    C = tranSupport::vectorDot3(diff, diff) - c*c - _radius*_radius;

    _calcQuadraticIntersect(A, B, C, posSense, hit, distance);
}
/*----------------------------------------------------------------------------*/
void Cylinder::normalAtPoint(
                const std::vector<double>& position,
                std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);

    // "unitNormal" is now particle location translated to cylinder
    unitNormal = position;
    tranSupport::vectorMinusEq(unitNormal, _pointOnAxis);

    double cosTheta = tranSupport::vectorDot3(unitNormal, _axis);

    // make "unitNormal" the un-normalized difference to the axis (position
    // minus projection)
    for (int i = 0; i < 3; ++i)
        unitNormal[i] -= _axis[i] * cosTheta;

    // the vector length should now be just the radius, if we have done it right
    Check(softEquiv(tranSupport::vectorNorm(unitNormal), _radius));

    for (int i = 0; i < 3; ++i)
        unitNormal[i] /= _radius;

    Ensure(softEquiv(tranSupport::vectorNorm(unitNormal), 1.0));
}
/*----------------------------------------------------------------------------*/
std::ostream& Cylinder::_output( std::ostream& os ) const {
    os  << "[ CYL   Point:  " << std::setw(10) << _pointOnAxis
        << " Axis: " << std::setw(10) << _axis 
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
