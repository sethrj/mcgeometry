/*!
 * \file VectorPrint.hpp
 * \brief Implementation of a function that prints a vector
 * \author Seth R. Johnson
 * 
 * Checks to see if two floating point numbers are equal to within a tolerance; 
 * also checks vectors of doubles, or vectors of vectors of doubles, etc.
 *
 */

#ifndef TS_VECTORPRINT_HPP
#define TS_VECTORPRINT_HPP

#include <iomanip>
#include <vector>
#include "dbc.hpp"

/*----------------------------------------------------------------------------*/

namespace std {

//! print a vector compactly
inline std::ostream& operator<<( std::ostream& os,
                                 const std::vector<double>& vec)
{
   std::vector<double>::const_iterator it = vec.begin();
   int i = 0;
   os << "<";
   while (it != vec.end()) {
      if (it != vec.begin()) {
         os << ",";
      }
      os << *it;
      //os << std::setw(4) << i << std::setw(8) << *it << std::endl;
      it++;
      i++;
   }
   os << ">";
   return os;
}

}

/*----------------------------------------------------------------------------*/
#endif
