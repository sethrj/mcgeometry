/*!
 * \file   Cylinder.cpp
 * \brief  Contains inline implementation for \c Cylinder
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "Cylinder.hpp"

#include <ostream>

#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"

namespace mcGeometry {
/*============================================================================*/
// Equation: (X-P)^2 - [(X-P).U]^2 - R^2 = 0
//      X = position
//      P = point on axis of cylinder
//      U = direction vector of cylinder axis
bool Cylinder::hasPosSense(const TVecDbl& position) const
{
    TVecDbl trPos(position - _pointOnAxis); // x - P

    double b = blitz::dot(trPos, _axis);

    double eval = blitz::dot(trPos, trPos) - b*b - _radius*_radius;

    return _hasPosSense(eval);
}

/*----------------------------------------------------------------------------*/
void Cylinder::intersect(
        const TVecDbl& position,
        const TVecDbl& direction,
        const bool posSense,
        bool& hit, double& distance) const
{
    Require(tranSupport::checkDirectionVector(direction));

    double temp = blitz::dot(direction, _axis);

    double A = 1 - temp * temp;

    TVecDbl trPos(position - _pointOnAxis);

    double B = blitz::dot(direction,
            trPos - _axis * blitz::dot(trPos, _axis));


    temp = blitz::dot(trPos, _axis);
    double C = blitz::dot(trPos, trPos) - temp*temp - _radius*_radius;

    _calcQuadraticIntersect(A, B, C, posSense, hit, distance);
}
/*----------------------------------------------------------------------------*/
void Cylinder::normalAtPoint(
        const TVecDbl& position,
        TVecDbl& unitNormal) const
{
    // "unitNormal" is now particle location translated to cylinder
    unitNormal = position - _pointOnAxis;

    double cosTheta = blitz::dot(unitNormal, _axis);

    // make "unitNormal" the un-normalized difference to the axis (position
    // minus projection)
    unitNormal -= _axis * cosTheta;

    unitNormal /= _radius;

    Ensure(tranSupport::checkDirectionVector(unitNormal));
}

/*----------------------------------------------------------------------------*/
std::ostream& Cylinder::printStream( std::ostream& os ) const
{
    os  << "[ CYL   Point:  " << std::setw(10) << _pointOnAxis
        << " Axis: " << std::setw(10) << _axis
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry

