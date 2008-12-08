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

//! decrement a by b
inline void vectorMinusEq( std::vector<double> &a, 
                          const std::vector<double> &b)
{
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

//! increment a by b
inline void vectorPlusEq( std::vector<double> &a, 
                          const std::vector<double> &b)
{
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

//! get the dot product of two vectors
inline double vectorDot( const std::vector<double> &a,
                         const std::vector<double> &b) {
   return std::inner_product(a.begin(), a.end(),
                               b.begin(), 0.0);
}

//! Get the dot product of two length-3 vectors
inline double vectorDot3( const std::vector<double> &a,
                          const std::vector<double> &b)
{
    Require(a.size() == 3);
    Require(b.size() == 3);

    double dotProduct = 0.0;
    for (unsigned int i = 0; i < 3; ++i) {
        dotProduct += a[i] * b[i];
    }
    return dotProduct;
}


//! get the total element-wise sum of a vector
inline double vectorSum( const std::vector<double> &a) {
   return std::accumulate(a.begin(), a.end(), 0.0);
}

//! get the vector's 2-norm
// square root of sum of squares
inline double vectorNorm( const std::vector<double> &a) {
   double theSum = 0.0;

   std::vector<double>::const_iterator it = a.begin();
   while(it != a.end()) {
      theSum += (*it) * (*it);
      ++it;
   }

   return std::sqrt(theSum);
}

//! get the vector's 2-norm
// (square root of sum of squares)
inline double vectorNorm3( const std::vector<double> &a) {
    Require(a.size() == 3);

    double theSum = 0.0;
    for (unsigned int i = 0; i < 3; ++i) {
        theSum += a[i] * a[i];
    }
    return std::sqrt(theSum);
}


//! get the distance between two vectors
// square root of sum of differences squared
inline double distance( const std::vector<double> &a,
                        const std::vector<double> &b)
{
    Require(a.size() == 3);
    Require(b.size() == 3);
    double theSum = 0.0;
    double tempDiff;

    for (unsigned int i = 0; i < 3; ++i) {
        tempDiff = a[i] - b[i];
        theSum  += tempDiff * tempDiff;
    }

    return std::sqrt(theSum);
}


/*----------------------------------------------------------------------------*/
}
/*============================================================================*/
#endif
