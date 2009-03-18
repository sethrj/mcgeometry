/*!
 * \file Cylinder.t.hpp
 * \brief Template code for cylinder along a particular axis.
 * \author Seth R. Johnson
 */

#ifndef MCG_CYLINDER_T_HPP
#define MCG_CYLINDER_T_HPP

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
template<unsigned int axis>
bool CylinderNormal<axis>::hasPosSense(
                const TVecDbl& position) const
{
    return _hasPosSense( _dotProduct(position, position) - _radius * _radius);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void CylinderNormal<axis>::intersect(
                const TVecDbl& position, 
                const TVecDbl& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    //temporary for now
    Insist(softEquiv(_pointOnAxis, TVecDbl(0.0)),
            "As of now, CylinderNormal must be on the axis.");

    double A = _dotProduct(direction, direction);

    double B = _dotProduct(direction, position);

    double C = _dotProduct(position, position) - _radius*_radius;

    return _calcQuadraticIntersect(A, B, C, posSense,
                                   hit, distance);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
void CylinderNormal<axis>::normalAtPoint(
                        const TVecDbl& position,
                        TVecDbl& unitNormal) const
{
    Insist(0, "Not yet implemented.");

    Ensure(tranSupport::checkDirectionVector(unitNormal));
}
/*----------------------------------------------------------------------------*/
template<>
std::ostream& CylinderNormal<0>::_output( std::ostream& os ) const
{
    os  << "[ CYLX: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*----------------------------------------------------------------------------*/
template<>
std::ostream& CylinderNormal<1>::_output( std::ostream& os ) const
{
    os  << "[ CYLY: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*----------------------------------------------------------------------------*/
template<>
std::ostream& CylinderNormal<2>::_output( std::ostream& os ) const
{
    os  << "[ CYLZ: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif

