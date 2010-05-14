/*!
 * \file   inst_CylinderNormal.cpp
 * \brief  Explicit instantiation for normal cylinder
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "CylinderNormal.t.hpp"
/*----------------------------------------------------------------------------*/
namespace mcGeometry {
    template class CylinderNormal<0u>;
    template class CylinderNormal<1u>;
    template class CylinderNormal<2u>;
}

