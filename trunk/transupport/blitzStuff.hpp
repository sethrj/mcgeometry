/*!
 * \file blitzStuff.hpp
 * \brief Various functions that do comparisons etc. on Blitz constructs
 * \author Seth R. Johnson
 */

#ifndef MCG_BLITZSTUFF_HPP
#define MCG_BLITZSTUFF_HPP
/*----------------------------------------------------------------------------*/

#include <cmath>
#include <blitz/tinyvec-et.h>
#include "transupport/SoftEquiv.hpp"

namespace tranSupport {
/*============================================================================*/

inline double vectorNorm(const blitz::TinyVector<double, 3>& v)
{
    return std::sqrt(blitz::dot(v, v));
}
/*! \brief Validate direction vectors in 3-D 
 *
 *  Three-D vectors must have $\norm{\vec{\Omega}} = 1$
 */
inline bool checkDirectionVector(const blitz::TinyVector<double, 3>& omega)
{
    return softEquiv(vectorNorm(omega), 1.0);
}

/*============================================================================*/
} // end namespace mcGeometry

#endif

