/*!
 * \file tQuadric.cc
 * \brief Test the Quadric geometry capability
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "Surfaces/Plane.hpp"
#include "Surfaces/Sphere.hpp"
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
    /* * * create sphere * * */
    doubleVec center(3,0.0);
    double      sphRadius = 2.0;

    center[0] = 1.0;

    Sphere    theSphere(center, sphRadius);

//    cout << theSphere << endl;

    /* * * create plane * * */
    doubleVec normal(3,0.0);

    normal[0] = 1.0;

    center[0] = 1.0;
    center[1] = 1.0;

    Plane thePlane(normal, center);

//    cout << thePlane << endl;

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);
    bool isParticlePositiveWrtSurface;

    Quadric::HitAndDist sphereResult, planeResult;

    particleLoc[0] = 1.5;

    particleDir[1] = 1.0;


//    cout << "particle loc: " << particleLoc
//            << " direction: " << particleDir << endl;

    /********************/
    TESTER_CHECKFORPASS(theSphere.hasPosSense(particleLoc) == false);
    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    sphereResult = theSphere.intersect(particleLoc, particleDir, false);
    planeResult  = thePlane.intersect(particleLoc, particleDir, true);

//    cout << "Did hit: " << sphereResult.first << endl
//        << "distance: " << sphereResult.second << endl;

    TESTER_CHECKFORPASS(sphereResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(sphereResult.second, 1.936491673103709));

    TESTER_CHECKFORPASS(planeResult.first == false);
    /* *******************\
     * move particle 
     ****************** */
    particleLoc[0] = -1;
    particleLoc[1] = -1;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;


//    cout << "particle loc: " << particleLoc
//         << " direction: " << particleDir << endl;
    /********************/
    TESTER_CHECKFORPASS(theSphere.hasPosSense(particleLoc) == true);
    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    sphereResult = theSphere.intersect(particleLoc, particleDir, true);
    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

//    cout << "Did hit: "  << planeResult.first << endl
//         << "distance: " << planeResult.second << endl;

    TESTER_CHECKFORPASS(sphereResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(sphereResult.second, 0.318544705827648));

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 2.828427124746190));
    /* *******************\
     * move particle 
     ****************** */

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    /********************/
    sphereResult = theSphere.intersect(particleLoc, particleDir, true);
    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

    TESTER_CHECKFORPASS(sphereResult.first == false);

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 2.828427124746190));
    /********************/
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("Quadric");
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
