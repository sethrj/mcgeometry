/*!
 * \file VectorMath.hpp
 * \brief Does useful things to double vectors.
 * \author Seth R. Johnson
 * 
 */

#ifndef TS_VECTORMATH_HPP
#define TS_VECTORMATH_HPP
/*----------------------------------------------------------------------------*/

#include "dbc.hpp"

#include <vector>
#include <numeric>
#include <iomanip>
#include <cmath>

namespace tranSupport {

/*----------------------------------------------------------------------------*/

//! subtract b from a
inline void vectorMinusEq( std::vector<double> &a, 
                          const std::vector<double> &b) {
   Require(a.size() == b.size());

   std::vector<double>::iterator aIter       = a.begin();
   std::vector<double>::const_iterator bIter = b.begin();

   while(aIter != a.end()) {
      // add value of bIter to aIter
      *aIter -= *bIter;

      ++aIter;
      ++bIter;
   }
}

//! add b to a
inline void vectorPlusEq( std::vector<double> &a, 
                          const std::vector<double> &b) {
   Require(a.size() == b.size());

   std::vector<double>::iterator aIter       = a.begin();
   std::vector<double>::const_iterator bIter = b.begin();

   while(aIter != a.end()) {
      // add value of bIter to aIter
      *aIter += *bIter;

      ++aIter;
      ++bIter;
   }
}

inline double vectorDot( const std::vector<double> &a,
                         const std::vector<double> &b) {
   return std::inner_product(a.begin(), a.end(),
                               b.begin(), 0.0);
}

inline double vectorSum( const std::vector<double> &a) {
   return std::accumulate(a.begin(), a.end(), 0.0);
}

inline double vectorNorm( const std::vector<double> &a) {
   double theSum = 0.0;

   std::vector<double>::const_iterator it = a.begin();
   while(it != a.end()) {
      theSum += (*it) * (*it);
      ++it;
   }

   return std::sqrt(theSum);
}

inline double distance( const std::vector<double> &a,
                        const std::vector<double> &b) {
   Require(a.size() == b.size());
   double theSum = 0.0;
   double tempDiff;

   std::vector<double>::const_iterator itA = a.begin();
   std::vector<double>::const_iterator itB = b.begin();
   while(itA != a.end()) {
      tempDiff = *itA - *itB;
      theSum += tempDiff * tempDiff;
      ++itA;
      ++itB;
   }

   return std::sqrt(theSum);
}


/*----------------------------------------------------------------------------*/
}
/*============================================================================*/
#endif
