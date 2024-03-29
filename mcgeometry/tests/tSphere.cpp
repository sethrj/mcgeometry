/*!
 * \file tSphere.cpp
 * \brief Test the Sphere geometry capability
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "mcgeometry/Sphere.hpp"
#include "mcgeometry/Surface.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using mcGeometry::Sphere;
using mcGeometry::Surface;

using std::cout;
using std::endl;


typedef blitz::TinyVector<double, 3> TVecDbl;

/*============================================================================*/
void runTests() {
    /* * * create sphere * * */
    TVecDbl center(0.0);
    double      sphRadius = 2.0;

    center[0] = 1.0;

    Sphere    theSphere(center, sphRadius);
    TESTER_CHECKFORPASS(theSphere.isReflecting() == false);

//    cout << theSphere << endl;

    /* * * create "particle" * * */
    TVecDbl particleLoc(0.0);
    TVecDbl particleDir(0.0);

    particleLoc[0] = 1.5;
    particleDir[1] = 1.0;

//    cout << "particle loc: " << particleLoc
//            << " direction: " << particleDir << endl;

    /********************/
    bool    didHit;
    double  distance;

    TESTER_CHECKFORPASS(theSphere.hasPosSense(particleLoc) == false);

    theSphere.intersect(particleLoc, particleDir, false,
        didHit, distance);

//    cout << "Did hit: " << didHit << endl
//        << "distance: " << distance << endl;

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 1.936491673103709));

    /********************/
    particleLoc = -1, -1, 0.5;

    particleDir = 0.707106781186547, 0.707106781186547, 0.0;

//    cout << "particle loc: " << particleLoc
//         << " direction: " << particleDir << endl;

    TESTER_CHECKFORPASS(theSphere.hasPosSense(particleLoc) == true);

    theSphere.intersect(particleLoc, particleDir, true,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 0.318544705827648));

    /********************/

    particleDir = 0.707106781186547, -0.707106781186547, 0.0;

    theSphere.intersect(particleLoc, particleDir, true,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    Surface* newSphere = theSphere.clone(182);

    TESTER_CHECKFORPASS(newSphere->getUserId() == 182);
    TESTER_CHECKFORPASS(newSphere->isReflecting() == false);

    delete newSphere;
}
/*============================================================================*/
int main(int, char**) {
    TESTER_INIT("Sphere");
    try {
        runTests();
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
