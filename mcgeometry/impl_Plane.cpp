/*!
 * \file   impl_Plane.cpp
 * \brief  Explicitly instantiates the plane normal to an axis
 * \author Seth R. Johnson
 */
#include "Plane.hpp"
#include "Plane.t.hpp"

namespace mcGeometry {

    template class PlaneNormal<0>;
    template class PlaneNormal<1>;
    template class PlaneNormal<2>;

} // end namespace mcGeometry

