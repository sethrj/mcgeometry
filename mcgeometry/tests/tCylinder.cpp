/*!
 * \file tCylinder.cpp
 * \brief Unit tests for Cylinder
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "mcgeometry/Cylinder.hpp"
#include "mcgeometry/Surface.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/constants.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"
//#include "transupport/VectorPrint.hpp"



using namespace mcGeometry;

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;

/*============================================================================*/
void runTestA() {
    /* * * create cylinder * * */
    double radius(3.0);
    doubleVec center(3,0.0);
    doubleVec axis(3,0.0);

    axis[0] = 1.0;    // Cylinder parallel to  x-axis

    center[0] = 1.0;    // Cylinder runs through the point (1,1,0)
    center[1] = 1.0;

    Cylinder theCylinder(center, axis, radius);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);


    particleLoc[0] = 1.5;
    particleDir[1] = 1.0;
//    cout << "particleLoc: " << particleLoc << endl;
//    cout << "particleDir: " << particleDir << endl;

    /********************/
    bool    didHit;
    double  distance;


    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

//  cout << "Did hit: "  << didHit << endl
//       << "distance: " << distance << endl;

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 4.0));

    /********************/
    particleLoc[0] = -1;
    particleLoc[1] = -2;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == true);

    theCylinder.intersect(particleLoc, particleDir, true,
        didHit, distance);

//    cout << "Did hit: "  << didHit << endl
//         << "distance: " << distance << endl;

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 0.0593405544489074));

    /********************/

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    theCylinder.intersect(particleLoc, particleDir, true,
        didHit, distance);

//    cout << "Did hit: "  << didHit << endl
//         << "distance: " << distance << endl;

    TESTER_CHECKFORPASS(didHit == false);
    /********************/
    Surface* newCyl = theCylinder.clone(182);

    TESTER_CHECKFORPASS(newCyl->getUserId() == 182);

    delete newCyl;
}
/*============================================================================*/
// this test can use Bielajew's "ccylz" to test against to be really sure
void runTestB() {
    /* * * create cylinder * * */
    double radius(3.0);
    doubleVec center(3,0.0);
    doubleVec axis(3,0.0);

    axis[2] = 1.0;    // Cylinder parallel to  z-axis

    Cylinder theCylinder(center, axis, radius);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);
    doubleVec particleDir(3,0.0);

    particleLoc[0] = 1.5;
    particleDir[1] = 1.0;

    /********************/
    bool    didHit;
    double  distance;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 2.598076211353316));

    /********************/
    particleLoc[0] = -1;
    particleLoc[1] = -2;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

//    cout << "Did hit: "  << didHit << endl
//         << "distance: " << distance << endl;

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 5.036796290982293));

    /********************/

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 1.414213562373095));

    /********************/
    // make sure that it thinks a particle going along its axis never hits it

    particleDir[0] = 0.0;
    particleDir[1] = 0.0;
    particleDir[2] = 1.0;

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    // make sure it can check "outside" correctly
    particleLoc[0] = -3;
    particleLoc[1] = -3;
    particleLoc[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == true);
}
/*============================================================================*/
// test some positive sense stuff
void runTestC() {
    /* * * create cylinder * * */
    double radius = 1.0;
    doubleVec center(3,0.0);
    doubleVec axis(3,0.0);

    axis[0] = 1.0;    // Cylinder parallel to x-axis

    Cylinder theCylinder(center, axis, radius);

    /* * * create "particle" * * */
    doubleVec particleLoc(3,0.0);

    particleLoc[0] = 0.0;
    particleLoc[1] = -0.7;
    particleLoc[2] = 0.5;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    particleLoc[0] = -0.726551;
    particleLoc[1] = -0.79317;
    particleLoc[2] = 0.544158;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);
    
    particleLoc[0] = 0.;
    particleLoc[1] = 0.79317;
    particleLoc[2] = 0.544158;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);
}
/*============================================================================*/
// test some surface normal stuff
void runTestD() {
    /* * * create cylinder 45 degrees in xy plane* * */
    double radius = 1.0;
    doubleVec center(3,0.0);
    doubleVec axis(3,0.0);

    axis[0] = tranSupport::constants::SQRTHALF;
    axis[1] = tranSupport::constants::SQRTHALF;

    Cylinder theCylinder(center, axis, radius);

    doubleVec particleLoc(3,0.0);

    particleLoc[0] = -tranSupport::constants::SQRTHALF;
    particleLoc[1] =  tranSupport::constants::SQRTHALF;
    particleLoc[2] = 0.0;

    doubleVec expectedNormal(3,0.0);
    expectedNormal[0] = -tranSupport::constants::SQRTHALF;
    expectedNormal[1] =  tranSupport::constants::SQRTHALF;
    expectedNormal[2] = 0.0;


    doubleVec returnedNormal;
    theCylinder.normalAtPoint(particleLoc, returnedNormal);
    TESTER_CHECKFORPASS(softEquiv(returnedNormal, expectedNormal, 1.e-14));

    particleLoc[0] += 2.0;
    particleLoc[1] += 2.0;
    theCylinder.normalAtPoint(particleLoc, returnedNormal);
    TESTER_CHECKFORPASS(softEquiv(returnedNormal, expectedNormal, 1.e-14));
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("Cylinder");
    try {
        runTestA();
        runTestB();
        runTestC();
        runTestD();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS(false);
    }

    TESTER_PRINTRESULT();
    
    return 0;
}

