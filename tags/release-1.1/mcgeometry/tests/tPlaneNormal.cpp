/*!
 * \file tPlaneNormal.cpp
 * \brief Test the Plane geometry capabilities
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/

// put our headers at top to check for dependency problems
#include "mcgeometry/PlaneNormal.hpp"

#include <iostream>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;


typedef blitz::TinyVector<double, 3> TVecDbl;

/*============================================================================*/
void testPlaneX() {
    /* * * create plane * * */
    PlaneX thePlane(1.0);

    /* * * create "particle" * * */
    TVecDbl particleLoc(0.0);
    TVecDbl particleDir(0.0);


    particleLoc[0] = 0.9;
    particleDir[1] = -1.0;

    /********************/
    bool    didHit;
    double  distance;


    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    thePlane.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    particleLoc[0] = 1.5;
    particleLoc[1] = -1;
    particleLoc[2] = 0.5;

    particleDir[0] = -0.894427190999916;
    particleDir[1] = 0.447213595499958;
    particleDir[2] = 0.0;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    thePlane.intersect(particleLoc, particleDir, true,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 0.559016994374947));

    /********************/
    Surface* newPlane = thePlane.clone(123);

    TESTER_CHECKFORPASS(newPlane->getUserId() == 123);

    delete newPlane;
}/*============================================================================*/
void testPlaneY() {
    /* * * create plane * * */
    PlaneY thePlane(1.0);

    /* * * create "particle" * * */
    TVecDbl particleLoc(0.0);
    TVecDbl particleDir(0.0);


    particleLoc[1] = 0.9;
    particleDir[2] = -1.0;

    /********************/
    bool    didHit;
    double  distance;


    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    thePlane.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    particleLoc = 0.5, 1.5, -1;
    particleDir = 0.0, -0.894427190999916, 0.447213595499958 ;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    thePlane.intersect(particleLoc, particleDir, true,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 0.559016994374947));

    /********************/
    Surface* newPlane = thePlane.clone(123);

    TESTER_CHECKFORPASS(newPlane->getUserId() == 123);

    delete newPlane;
}/*============================================================================*/
void testPlaneZ() {
    /* * * create plane * * */
    PlaneZ thePlane(1.0);

    /* * * create "particle" * * */
    TVecDbl particleLoc(0.0);
    TVecDbl particleDir(0.0);


    particleLoc[2] = 0.9;
    particleDir[0] = -1.0;

    /********************/
    bool    didHit;
    double  distance;


    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == false);

    thePlane.intersect(particleLoc, particleDir, false,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == false);

    /********************/
    particleLoc = -1, 0.5, 1.5;

    particleDir = 0.447213595499958, 0.0, -0.894427190999916;

    TESTER_CHECKFORPASS(thePlane.hasPosSense(particleLoc) == true);

    thePlane.intersect(particleLoc, particleDir, true,
        didHit, distance);

    TESTER_CHECKFORPASS(didHit == true);
    TESTER_CHECKFORPASS(softEquiv(distance, 0.559016994374947));

    /********************/
    Surface* newPlane = thePlane.clone(123);

    TESTER_CHECKFORPASS(newPlane->getUserId() == 123);

    delete newPlane;
}

/*============================================================================*/
int main(int, char**) {
    TESTER_INIT("Plane");
    try {
        testPlaneX();
        testPlaneY();
        testPlaneZ();
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
