
// put our headers at top to check for dependency problems
#include "mcgeometry/Cylinder.hpp"
#include "mcgeometry/Quadric.hpp"

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

    /********************/
    Quadric::HitAndDist theResult;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    cout << "Did hit: "  << theResult.first << endl
         << "distance: " << theResult.second << endl;

    TESTER_CHECKFORPASS(theResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(theResult.second, 4.0));

    /********************/
    particleLoc[0] = -1;
    particleLoc[1] = -2;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == true);

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    cout << "Did hit: "  << theResult.first << endl
         << "distance: " << theResult.second << endl;

    TESTER_CHECKFORPASS(theResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(theResult.second, 0.0593405544489074));

    /********************/

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    cout << "Did hit: "  << theResult.first << endl
         << "distance: " << theResult.second << endl;

    TESTER_CHECKFORPASS(theResult.first == false);
    /********************/
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
    Quadric::HitAndDist theResult;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    TESTER_CHECKFORPASS(theResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(theResult.second, 2.598076211353316));

    /********************/
    particleLoc[0] = -1;
    particleLoc[1] = -2;
    particleLoc[2] = 0.5;

    particleDir[0] = 0.707106781186547;
    particleDir[1] = 0.707106781186547;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == true);

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

//    cout << "Did hit: "  << theResult.first << endl
//         << "distance: " << theResult.second << endl;

    TESTER_CHECKFORPASS(theResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(theResult.second, 5.036796290982293));

    /********************/

    particleDir[0] = 0.707106781186547;
    particleDir[1] = -0.707106781186547;
    particleDir[2] = 0.0;

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    TESTER_CHECKFORPASS(theResult.first == true);
    TESTER_CHECKFORPASS(softEquiv(theResult.second, 1.414213562373095));

    /********************/
    // make sure that it thinks a particle going along its axis never hits it

    particleDir[0] = 0.0;
    particleDir[1] = 0.0;
    particleDir[2] = 1.0;

    theResult  = theCylinder.intersect(particleLoc, particleDir, true);

    TESTER_CHECKFORPASS(theResult.first == true);

    /********************/
    // make sure it can check "outside" correctly
    particleLoc[0] = -3;
    particleLoc[1] = -3;
    particleLoc[2] = 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("Cylinder");
    try {
        runTestA();
        runTestB();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS(false);
    }

    TESTER_PRINTRESULT();
    
    return 0;
}

