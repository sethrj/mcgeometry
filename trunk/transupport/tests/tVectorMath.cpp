/*!
 * \file tVectorMath.cpp
 * \brief Unit tests for vector math
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "transupport/VectorMath.hpp"
//#include "transupport/VectorPrint.hpp"

#include <iostream>
#include <vector>
#include "transupport/dbc.hpp"
#include "transupport/VectorComp.hpp"
#include "transupport/UnitTester.hpp"

typedef std::vector<double> doubleVector;
using tranSupport::softEquiv;
using std::cout;
using std::endl;
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
   TESTER_INIT("VectorMath");

   int vecLength = 5;
   doubleVector vec1(vecLength), vec2(vecLength);

   for (int i = 0; i < vecLength; i++) {
      vec1[i] = i / 2.0;
      vec2[i] = (vecLength - i + 1) * 3;
   }

// cout << vec1 << vec2 << endl;

   // ===== check the summation ===== //
   TESTER_CHECKFORPASS(softEquiv(tranSupport::vectorSum(vec1), 5.0));
   
   // ===== check the dot product ===== //
   TESTER_CHECKFORPASS(softEquiv(tranSupport::vectorDot(vec1, vec2), 45.0));
   
   // ===== check the distance ===== //
   TESTER_CHECKFORPASS(softEquiv(tranSupport::distance(vec1, vec2),
                                                 26.9722079185224, 1e-14));

   TESTER_PRINTRESULT();
   
   return 0;
}
/*----------------------------------------------------------------------------*/
