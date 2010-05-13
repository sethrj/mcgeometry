/*!
 * \file   PlaneNormal.t.hpp
 * \brief  Contains templated implementation for  PlaneNormal
 * \author Seth R. Johnson
 */
#ifndef mcgeometry_PlaneNormal_t_hpp
#define mcgeometry_PlaneNormal_t_hpp
/*----------------------------------------------------------------------------*/
#include "PlaneNormal.hpp"

#include <algorithm>
#include <iostream>

#include <blitz/tinyvec-et.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"
#include "transupport/SoftEquiv.hpp"

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
        bool& hit,
        double& distance) const
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
void PlaneNormal<axis>::normalAtPoint(
        const TVecDbl&,
        TVecDbl& unitNormal) const
{
    unitNormal = 0.0;
    unitNormal[axis] = 1.0;
}

/*----------------------------------------------------------------------------*/
template<unsigned int axis>
std::ostream& PlaneNormal<axis>::printStream( std::ostream& os ) const
{
    os  << "[ PLANE " << 'X' + axis
        << " Point:  " << std::setw(10) << _coordinate
        << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif

