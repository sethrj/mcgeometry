/*!
 * \file   inst_PlaneNormal.cpp
 * \brief  Explicit instantiation for normal plane
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "PlaneNormal.t.hpp"
/*----------------------------------------------------------------------------*/
namespace mcGeometry {
    template class PlaneNormal<0u>;
    template class PlaneNormal<1u>;
    template class PlaneNormal<2u>;
}

