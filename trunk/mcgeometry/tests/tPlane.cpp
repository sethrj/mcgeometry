/*!
 * \file tPlane.cpp
 * \brief Test the Plane geometry capabilities
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Surface.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"
// #include "transupport/VectorPrint.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;


typedef std::vector<double> doubleVec;

/*============================================================================*/
void testPlane() {
    /* * * create plane * * */
    doubleVec center(3,0.0);
    doubleVec normal(3,0.0);

    normal[0] = 0.707106781186547;
    normal[1] = 0.707106781186547;

    center[0] = 1.0;
    center[1] = 1.0;

    Plane thePlane(normal, center);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);


    particleLoc[0] = 2.01;
    particleDir[1] = 1.0;

    /********************/
    Surface::HitAndDist planeResult;

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

    particleDir[0] = 0.894427190999916;
    particleDir[1] = -0.447213595499958;
    particleDir[2] = 0.0;

    /********************/
    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 8.944271909999159));
    /********************/
    Surface* newPlane = thePlane.clone(182);

    TESTER_CHECKFORPASS(newPlane->getUserId() == 182);

    delete newPlane;
}
/*============================================================================*/
void testPlaneX() {
    /* * * create plane * * */
    PlaneX thePlane(1.0);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);


    particleLoc[0] = 0.9;
    particleDir[1] = -1.0;

    /********************/
    Surface::HitAndDist planeResult;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    planeResult  = thePlane.intersect(particleLoc, particleDir, false);

    TESTER_CHECKFORPASS(planeResult.first == false);

    /********************/
    particleLoc[0] = 1.5;
    particleLoc[1] = -1;
    particleLoc[2] = 0.5;

    particleDir[0] = -0.894427190999916;
    particleDir[1] = 0.447213595499958;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    planeResult  = thePlane.intersect(particleLoc, particleDir, true);

    TESTER_CHECKFORPASS(planeResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(planeResult.second, 0.559016994374947));

    /********************/
    Surface* newPlane = thePlane.clone(182);

    TESTER_CHECKFORPASS(newPlane->getUserId() == 182);

    delete newPlane;
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("Plane");
    try {
        testPlane();
        testPlaneX();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS(false);
    }

    TESTER_PRINTRESULT();
    
    return 0;
}
