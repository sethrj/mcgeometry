/*!
 * \file tPlane.cc
 * \brief Test the Quadric geometry capability
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "Surfaces/Plane.hpp"
#include "Surfaces/Quadric.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"
// #include "transupport/VectorPrint.hpp"

using std::cout;
using std::endl;


typedef std::vector<double> doubleVec;

/*============================================================================*/
void runTests() {
    /* * * create plane * * */
    doubleVec center(3,0.0);
    doubleVec normal(3,0.0);

    normal[0] = 1.0;

    center[0] = 1.0;
    center[1] = 1.0;

    Plane thePlane(normal, center);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);


    particleLoc[0] = 1.5;
    particleDir[1] = 1.0;

    /********************/
    Quadric::HitAndDist planeResult;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    planeResult  = thePlane.intersect(particleLoc, particleDir, true);

    TESTER_CHECKFORPASS(planeResult.first == false);

    /********************/
    particleLoc[0] = -1;
    particleLoc[1] = -1;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

//    cout << "Did hit: "  << planeResult.first << endl
//         << "distance: " << planeResult.second << endl;

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 2.828427124746190));

    /* *******************\
     * move particle 
     ****************** */

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    /********************/
    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 2.828427124746190));
    /********************/
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("Plane");
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
