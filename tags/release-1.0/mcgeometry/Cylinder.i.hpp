/*!
 * \file   Cylinder.i.hpp
 * \brief  Contains inline implementation for \c Cylinder
 * \author Seth R. Johnson
 */

#ifndef MCG_CYLINDER_I_HPP
#define MCG_CYLINDER_I_HPP

#include <iostream>

#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Cylinder.hpp"
#include "Surface.i.hpp"

namespace mcGeometry {
/*============================================================================*/
/*----------------------------------------------------------------------------*/
// Equation: (X-P)^2 - [(X-P).U]^2 - R^2 = 0
//      X = position 
//      P = point on axis of cylinder
//      U = direction vector of cylinder axis
inline bool Cylinder::hasPosSense(const TVecDbl& position) const
{
    TVecDbl trPos(position - _pointOnAxis); // x - P

    double b = blitz::dot(trPos, _axis);

    double eval = blitz::dot(trPos, trPos) - b*b - _radius*_radius;

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
inline void Cylinder::intersect(
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
inline void Cylinder::normalAtPoint(
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
inline std::ostream& Cylinder::_output( std::ostream& os ) const {
    os  << "[ CYL   Point:  " << std::setw(10) << _pointOnAxis
        << " Axis: " << std::setw(10) << _axis 
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
template<unsigned int axis>
inline bool CylinderNormal<axis>::hasPosSense(
                const TVecDbl& position) const
{
    TVecDbl trPos(position - _pointOnAxis);

    return _hasPosSense( _dotProduct(trPos, trPos) - _radius * _radius);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void CylinderNormal<axis>::intersect(
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
inline void CylinderNormal<axis>::normalAtPoint(
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

//! \cond
template<>
inline double CylinderNormal<0>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[1] * y[1] + x[2] * y[2];
}

template<>
inline double CylinderNormal<1>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[0] * y[0] + x[2] * y[2];
}

template<>
inline double CylinderNormal<2>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[0] * y[0] + x[1] * y[1];
}
//! \endcond
/*----------------------------------------------------------------------------*/
template<>
inline std::ostream& CylinderNormal<0>::_output( std::ostream& os ) const
{
    os  << "[ CYLX   Point:  " << std::setw(10) << _pointOnAxis
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}

template<>
inline std::ostream& CylinderNormal<1>::_output( std::ostream& os ) const
{
    os  << "[ CYLY   Point:  " << std::setw(10) << _pointOnAxis
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}

template<>
inline std::ostream& CylinderNormal<2>::_output( std::ostream& os ) const
{
    os  << "[ CYLZ   Point:  " << std::setw(10) << _pointOnAxis
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif

