/*!
 * \file MCGeometry
 * \brief Unit tests for MCGeometry
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "MCGeometry.hpp"
#include "Surfaces/Plane.hpp"
#include "Surfaces/Sphere.hpp"

#include <iostream>
#include <vector>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;

void createGeometry( MCGeometry& theGeom) {
    /* * * create sphere * * */
    doubleVec center(3,0.0);
    double      sphRadius = 2.0;

    center[0] = 1.0;

    Sphere    theSphere(center, sphRadius);

    /* * * create plane * * */
    doubleVec normal(3,0.0);

    normal[0] = 1.0;

    center[0] = 1.0;
    center[1] = 1.0;

    Plane thePlane(normal, center);

    // ADD SURFACES
    theGeom.addSurface(1, theSphere);
    theGeom.addSurface(2, thePlane);

    // make sure we can't add the same surface twice
    bool caughtError = false;

    try {
        theGeom.addSurface(2, theSphere);
    } 
    catch (tranSupport::tranError &theErr) {
        //cout     << "Caught error:" << endl
        //        << theErr.what();
        caughtError = true;
    }
    TESTER_CHECKFORPASS(caughtError);
}
/*============================================================================*/
void runTests() {   

    MCGeometry theGeom;

    createGeometry(theGeom);

//   TESTER_CHECKFORPASS(softEquiv(1.0, 1.0));
}

/*============================================================================*/
int main(int argc, char *argv[]) {
   TESTER_INIT("MCGeometry");
   try {
      runTests();
   }
   catch (tranSupport::tranError &theErr) {
      cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
         << theErr.what() << endl;
      TESTER_CHECKFORPASS(false);
   }

   TESTER_PRINTRESULT();
   
   return 0;
}
