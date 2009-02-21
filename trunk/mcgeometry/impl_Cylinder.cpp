/*!
 * \file   impl_Cylinder.cpp
 * \brief  Explicitly instantiates the cylinder along an axis
 * \author Seth R. Johnson
 */
#include "Cylinder.hpp"
#include "Cylinder.t.hpp"

namespace mcGeometry {

    template class CylinderNormal<0>;
    template class CylinderNormal<1>;
    template class CylinderNormal<2>;

} // end namespace mcGeometry

