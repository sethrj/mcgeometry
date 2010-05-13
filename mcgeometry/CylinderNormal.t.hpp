/*!
 * \file   CylinderNormal.t.hpp
 * \brief  Contains templated implementation for \c CylinderNormal
 * \author Seth R. Johnson
 */
#ifndef mcgeometry_CylinderNormal_t_hpp
#define mcgeometry_CylinderNormal_t_hpp
/*----------------------------------------------------------------------------*/
#include "CylinderNormal.hpp"

#include <ostream>

#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"

namespace mcGeometry {
/*============================================================================*/
template<unsigned int axis>
bool CylinderNormal<axis>::hasPosSense(
                const TVecDbl& position) const
{
    TVecDbl trPos(position - _pointOnAxis);

    return _hasPosSense( _dotProduct(trPos, trPos) - _radius * _radius);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void CylinderNormal<axis>::intersect(
                const TVecDbl& position,
                const TVecDbl& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    double A = 1 - direction[axis] * direction[axis];

    // (minor TODO: define a translate function that ignores the cylinder axis)
    TVecDbl trPos(position - _pointOnAxis);

    // _dotProduct ignores the value on the cylinderaxis
    double B = _dotProduct(direction, trPos);

    double C = _dotProduct(trPos, trPos) - _radius*_radius;

    _calcQuadraticIntersect(A, B, C, posSense, hit, distance);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void CylinderNormal<axis>::normalAtPoint(
                        const TVecDbl& position,
                        TVecDbl& unitNormal) const
{
    //move so we're relative to the axis
    unitNormal = position - _pointOnAxis;

    //zero component that's not on the axis
    unitNormal[axis] = 0.0;

    //normalize (since we know we are on the surface of the cylinder, we don't
    // have to do an extra dot product)
    unitNormal /= _radius;

    Ensure(tranSupport::checkDirectionVector(unitNormal));
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
std::ostream& CylinderNormal<axis>::printStream( std::ostream& os ) const
{
    os  << "[ CYL" << 'X' + axis
        << "   Point:  " << std::setw(10) << _pointOnAxis
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif

