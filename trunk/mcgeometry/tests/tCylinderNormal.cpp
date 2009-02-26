/*!
 * \file tCylinderNormal.cpp
 * \brief Unit tests for \c Cylinder<unsigned int>
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



using namespace mcGeometry;

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;

/*============================================================================*/
// this test can use Bielajew's "ccylz" to test against to be really sure
void runTestZ() {
    /* * * create cylinder * * */
    double radius(3.0);
    doubleVec center(3,0.0);
    doubleVec axis(3,0.0);

    axis[2] = 1.0;    // Cylinder parallel to  z-axis

    CylinderZ theCylinder(center, radius);

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

    radius = 1.0;
    CylinderZ anotherCylinder(center, radius);

    particleLoc[0] = -0.7;
    particleLoc[1] = 0.5;
    particleLoc[2] = 0.0;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);

    particleLoc[0] = -0.79317;
    particleLoc[1] = 0.544158;
    particleLoc[2] = -0.726551;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);
    
    particleLoc[0] = 0.79317;
    particleLoc[1] = 0.544158;
    particleLoc[2] = 0.;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("CylinderNormal");
    try {
        runTestZ();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS( CAUGHT_UNEXPECTED_EXCEPTION );
    }

    TESTER_PRINTRESULT();
    
    return 0;
}

