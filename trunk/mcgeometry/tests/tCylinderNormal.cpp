/*!
 * \file tCylinderNormal.cpp
 * \brief Unit tests for \c Cylinder<unsigned int>
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "mcgeometry/Cylinder.hpp"
#include "mcgeometry/Surface.hpp"

#include "mcgeometry/Cylinder.i.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/constants.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"



using namespace mcGeometry;
using tranSupport::constants::SQRTHALF;

using std::cout;
using std::endl;

typedef blitz::TinyVector<double, 3> TVecDbl;

/*============================================================================*/
// this test can use Bielajew's "ccylz" to test against to be really sure
void runTestZ() {
    /* * * create cylinder * * */
    double radius(3.0);
    TVecDbl center(1.0, 0.0, 99.0);

    CylinderZ theCylinder(center, radius);

    /* * * create "particle" * * */
    TVecDbl particleLoc(2.5, 0.0, 0.0);
    TVecDbl particleDir(0.0, 1.0, 0.0);

    /********************/
    bool    didHit;
    double  distance;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 2.598076211353316));

    /********************/
    particleLoc = 0, -2, 0.5;

    particleDir = 0.707106781186547, 0.707106781186547, 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == false);

    theCylinder.intersect(particleLoc, particleDir, false,
                          didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 5.036796290982293));

    /********************/

    particleDir = 0.707106781186547, -0.707106781186547, 0.0;

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 1.414213562373095));

    /********************/
    // make sure that it thinks a particle going along its axis never hits it

    particleDir = 0.0, 0.0, 1.0;

    theCylinder.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    // make sure it can check "outside" correctly
    particleLoc = -2.0, -3.0, 0.0;

    TESTER_CHECKFORPASS(theCylinder.hasPosSense(particleLoc) == true);

    radius = 1.0;
    CylinderZ anotherCylinder(center, radius);

    particleLoc = 0.3, 0.5, 0.0;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);

    particleLoc = 0.20683, 0.544158, -0.726551;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);
    
    particleLoc = 1.79317,  0.544158, 0.0;

    TESTER_CHECKFORPASS(anotherCylinder.hasPosSense(particleLoc) == false);

    /********************/
    // make sure it gets the right surface normal
    particleLoc = -SQRTHALF + 1.0, -SQRTHALF, 3.0;

    TVecDbl expectedNormal(-SQRTHALF, -SQRTHALF, 0.0);
    TVecDbl returnedNormal;
    anotherCylinder.normalAtPoint(particleLoc, returnedNormal);
    TESTER_CHECKFORPASS(softEquiv(returnedNormal, expectedNormal, 1.e-14));

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
    
    if (!TESTER_HASPASSED()) {
        return 1;
    }

    return 0;
}

