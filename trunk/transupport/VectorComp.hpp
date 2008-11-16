/*!
 * \file VectorComp.hpp
 * \brief Recursive comparisons on vectors of vectors of ... of doubles
 * \author Seth R. Johnson
 * 
 * Checks to see if two floating point numbers are equal to within a tolerance; 
 * also checks vectors of doubles, or vectors of vectors of doubles, etc.
 * Also has support for checking non-negativity, non-zeroness, etc. of vectors 
 * of vectors.
 */

#ifndef TS_VECTORCOMP_HPP
#define TS_VECTORCOMP_HPP

#include <cmath>
#include "dbc.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace tranSupport {
/*----------------------------------------------------------------------------*/
namespace vectorCompStuff {
/*----------------------------------------------------------------------------*/
//! Functor that does soft equivalence on two floating points
class SoftEquivFunctor {
public:
   SoftEquivFunctor() : eps(1.e-15)
   { /* * */ }

   //! create with a non-default epsilon
   SoftEquivFunctor(double _eps) : eps(_eps)
   { /* * */ }
   
   //! compare target to reference, see if they're close enough
   bool operator() (const double target, const double ref) const
   {
      using std::fabs;

      // check relative error, zero-safe
      if (fabs(target - ref) <= fabs(ref) * eps) {
         return true;
      }

      // if the reference number is zero, allow epsilon
      if (ref == 0) {
         return (fabs(target) <= eps);
      }

      // if reference is non-zero and it failed the first test
      return false;
   } 

private:
   //! the tolerated relative error
   const double eps;
};

/*----------------------------------------------------------------------------*/
//! Functor that does soft equivalence on two floating points
class IsNonZeroFunctor {
public:
   IsNonZeroFunctor()
   { /* * */ }
   
   //! test if target is nonzero
   bool operator() (const double target) const
   {
      if (target == 0) {
         return false;
      }

      return true;
   } 
};

/*----------------------------------------------------------------------------*/
//! Functor that does soft equivalence on two floating points
class IsNonNegativeFunctor {
public:
   IsNonNegativeFunctor()
   { /* * */ }
   
   //! test that the target is positive
   bool operator() (const double target) const
   {
      if (target < 0.0) {
         return false;
      }

      return true;
   } 
};


/*============================================================================*/

//! Recursively templated to perform theOperation on the innermost doubles
template <class T, class functorT>
inline bool vectorComp(   const T& target,
                          const T& ref,
                          functorT& theOperation)
{
   Require(target.size() == ref.size());

   typename T::const_iterator tIter   = target.begin();
   typename T::const_iterator refIter = ref.begin();

   while(refIter != ref.end()) {
      // check the values of the current iterate
      if (vectorComp(*tIter, *refIter, theOperation) == false) {
         return false;
      }
      tIter++;
      refIter++;
   }
   return true;   
}

//! \brief Called at the innermost loop
template <class functorT>
inline bool vectorComp( const double& target, const double& ref,
                        functorT& theOperation)
{
   bool result = theOperation(target, ref);

   return result;
} 

/*============================================================================*/

//! Recursively templated to perform theOperation on the innermost double
template <class T, class functorT>
inline bool vectorComp( const T& target,
                        functorT& theOperation)
{
   typename T::const_iterator tIter   = target.begin();

   while(tIter != target.end()) {
      // check the values of the current iterate
      if (vectorComp(*tIter, theOperation) == false) {
         return false;
      }
      tIter++;
   }
   return true;   
}

//! \brief Called at the innermost loop
template <class functorT>
inline bool vectorComp( const double& target,
                          functorT& theOperation)
{
   bool result = theOperation(target);

   return result;
} 
/*============================================================================*/

} // end VectorComp namespace
/*----------------------------------------------------------------------------*/

//! \brief Check the equivalence of two standard vectors of doubles, or of more vectors
template <class T>
inline bool softEquiv(const T& target,
                        const T& ref, double eps = 1.e-15)
{
   vectorCompStuff::SoftEquivFunctor myFunctor(eps);

   return vectorCompStuff::vectorComp(target, ref, myFunctor);
}

//! \brief Check the equivalence of two standard vectors of doubles, or of more vectors
template <class T>
inline bool hasNoZeros(const T& target)
{
   vectorCompStuff::IsNonZeroFunctor myFunctor;

   return vectorCompStuff::vectorComp(target, myFunctor);
}

//! \brief Check the equivalence of two standard vectors of doubles, or of more vectors
template <class T>
inline bool hasNoNegatives(const T& target)
{
   vectorCompStuff::IsNonNegativeFunctor myFunctor;

   return vectorCompStuff::vectorComp(target, myFunctor);
}

/*----------------------------------------------------------------------------*/
}
#endif
