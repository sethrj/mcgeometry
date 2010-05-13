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

//! Return the norm of a vector (the square root of the sum of the squares)
template<typename T>
inline double vectorNorm(const T& v)
{
    return std::sqrt(blitz::dot(v, v));
}
/*----------------------------------------------------------------------------*/
//! Return the distance between two points
template<typename T>
inline double distance(
        const T& a,
        const T& b)
{
    return vectorNorm(a - b);
}
/*----------------------------------------------------------------------------*/
/*! \brief Validate direction vectors in 3-D 
 *
 *  Three-D vectors must have \f$ \left| \bm{\Omega} \right| = 1 \f$
 */
inline bool checkDirectionVector(const blitz::TinyVector<double, 3>& omega)
{
    return softEquiv(vectorNorm(omega), 1.0);
}

/*============================================================================*/
} // end namespace mcGeometry

#endif

