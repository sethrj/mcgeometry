/*!
 * \file tMCGeometry.cpp
 * \brief Unit tests for MCGeometry
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "mcgeometry/MCGeometry.hpp"
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Sphere.hpp"

#include <iostream>
#include <vector>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorPrint.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;
typedef std::vector<signed int> intVec;

void createGeometry( MCGeometry& theGeom) {
    /* * * create sphere * * */
    doubleVec center(3,0.0);
    double      sphRadius = 3.0;

    Sphere    theSphere(center, sphRadius);

    /* * * create planes * * */
    doubleVec normal(3,0.0);

    normal[1] = 1.0;

    center[1] = 1.0;
    Plane plane1(normal, center);

    center[1] = 0.0;
    Plane plane2(normal, center);

    center[1] = -1.0;
    Plane plane3(normal, center);

    normal.assign(3,0.0);
    center.assign(3,0.0);
    normal[0] = 1.0;
    center[0] = 0.0;
    Plane plane4(normal, center);

    //========== ADD SURFACES
    theGeom.addSurface(5, theSphere);

    theGeom.addSurface(1, plane1);
    theGeom.addSurface(2, plane2);
    theGeom.addSurface(3, plane3);
    theGeom.addSurface(4, plane4);


    //========== ADD CELLS
    intVec theSurfaces(4,0);

    theSurfaces[0] = -5;
    theSurfaces[1] = -1;
    theSurfaces[2] =  3;
    theSurfaces[3] =  4;

    theGeom.addCell(10, theSurfaces);

    theSurfaces[0] = -5;
    theSurfaces[1] = -1;
    theSurfaces[2] =  2;
    theSurfaces[3] = -4;

    theGeom.addCell(20, theSurfaces);

    theSurfaces[0] = -5;
    theSurfaces[1] = -2;
    theSurfaces[2] =  3;
    theSurfaces[3] = -4;

    theGeom.addCell(30, theSurfaces);

    theSurfaces.resize(2);
    theSurfaces[0] = -5;
    theSurfaces[1] =  1;

    theGeom.addCell(40, theSurfaces);

    theSurfaces[0] = -5;
    theSurfaces[1] = -3;

    theGeom.addCell(50, theSurfaces);

    theSurfaces.resize(1);
    theSurfaces[0] = 5;

    theGeom.addCell(60, theSurfaces);

}
/*============================================================================*/
void testGeometryErrorChecking()
{
    intVec theSurfaces(1, 0);
    doubleVec center(3,0.0);

    Sphere aSphere(center, 2.0);

    //    in practice,
    //    YOU SHOULD NEVER KEEP ON GOING IF MCGEOMETRY THROWS AN ERROR, OK?
    //    (i.e. it may lose memory, count an extra surface, etc.)

    {
        MCGeometry theGeom;

        createGeometry(theGeom);

        //===== try adding a duplicate surface
        bool caughtError = false;

        try {
            theGeom.addSurface(2, aSphere);
        } 
        catch (tranSupport::tranError &theErr) {
            caughtError = true;
        }
        TESTER_CHECKFORPASS(caughtError);
    }

    {
        MCGeometry theGeom;

        createGeometry(theGeom);

        //===== try adding a fake cell
        theSurfaces[0] = 1337;

        bool caughtError = false;
        try {
            theGeom.addCell(100, theSurfaces);
        } 
        catch (tranSupport::tranError &theErr) {
            caughtError = true;
        }
        TESTER_CHECKFORPASS(caughtError);
    }

    {
        MCGeometry theGeom;

        createGeometry(theGeom);

        //===== try adding a duplicate cell
        theSurfaces[0] = 2;

        bool caughtError = false;
        try {
            theGeom.addCell(10, theSurfaces);
        } 
        catch (tranSupport::tranError &theErr) {
            caughtError = true;
        }
        TESTER_CHECKFORPASS(caughtError);
    }
}
/*============================================================================*/
void runTests() {   

    MCGeometry theGeom;

    createGeometry(theGeom);

//    theGeom.debugPrint();

    // ==== test a variety of locations and directions
    unsigned int numLocs = 8;
    std::vector< doubleVec > locations(numLocs);
    std::vector< doubleVec > directions(4);

    int index = 0;
    for (unsigned int j = 0; j < numLocs/2; j++) {
        for (unsigned int i = 0; i < 2; i++) {
            locations[index].assign(3, 0.0);
            locations[index][0] = -0.5 + i * 1.0;
            locations[index][1] = -1.5 + j * 1.0;

//            cout << "Location  " << index << ": " << locations[index] << endl;
            index++;
        }
    }
    
    index = 0;
    for (int j = 0; j < 2; j++) {
        for (unsigned int i = 0; i < 2; i++) {
            directions[index].assign(3, 0.0);
            directions[index][i] = (1.0 - 2*j);
//            cout << "Direction " << index << ": " << directions[index] << endl;
            index++;
        }
    }

    // ==== starting cells
    unsigned int expectedStartingCells[] = {50, 50, 30, 10, 20, 10, 40, 40};
    bool correctStartingCells = true;

    for (unsigned int i = 0; i < numLocs; i++) {
        correctStartingCells = 
            correctStartingCells &&
            (theGeom.findCell(locations[i]) == expectedStartingCells[i]);
    }
    TESTER_CHECKFORPASS(correctStartingCells);

    // ==== ending cells
    unsigned int expectedEndingCells[][4]
        = { {60, 30, 60, 60},
            {60, 10, 60, 60},
            {10, 20, 60, 50},
            {60, 40, 30, 50},
            {10, 40, 60, 30},
            {60, 40, 20, 50},
            {60, 60, 60, 20},
            {60, 60, 60, 10}
        };
    bool correctEndingCells = true;

    doubleVec    newPosition;
    unsigned int newCellId;
    double       distance;
    MCGeometry::ReturnStatus returnStatus;

    // run this test twice so that we can be sure it's using the hood
    // connectivity
    for (int iter = 0; iter < 2; iter++)
    {
        correctEndingCells = true;

        for (unsigned int startLoc = 0; startLoc < numLocs; startLoc++) {
            for (unsigned int dir = 0; dir < 4; dir++) {
                theGeom.intersect(  locations[startLoc],
                                    directions[dir],
                                    expectedStartingCells[startLoc],
                                    newPosition,
                                    newCellId,
                                    distance,
                                    returnStatus);

                correctEndingCells = 
                    correctEndingCells &&
                    (newCellId == expectedEndingCells[startLoc][dir]);
            }
        }
        TESTER_CHECKFORPASS(correctEndingCells);
        
    }

//    theGeom.debugPrint();
}

/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("MCGeometry");
    try {
        testGeometryErrorChecking();
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
