/*!
 * \file MCGeometry
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

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;
typedef std::vector<signed int> IntVec;

void createGeometry( MCGeometry& theGeom) {
    /* * * create sphere * * */
    doubleVec center(3,0.0);
    double      sphRadius = 2.0;

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
    IntVec theSurfaces(4,0);

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
    //===== try adding a duplicate surface
    bool caughtError = false;

    try {
        theGeom.addSurface(2, theSphere);
    } 
    catch (tranSupport::tranError &theErr) {
        caughtError = true;
    }
    TESTER_CHECKFORPASS(caughtError);

    //===== try adding a fake cell
    theSurfaces[0] = 1337;

    caughtError = false;
    try {
        theGeom.addCell(100, theSurfaces);
    } 
    catch (tranSupport::tranError &theErr) {
        caughtError = true;
    }
    TESTER_CHECKFORPASS(caughtError);

    //===== try adding a duplicate cell
    theSurfaces[0] = 2;

    caughtError = false;
    try {
        theGeom.addCell(10, theSurfaces);
    } 
    catch (tranSupport::tranError &theErr) {
        caughtError = true;
    }
    TESTER_CHECKFORPASS(caughtError);
}
/*============================================================================*/
void runTests() {   

    MCGeometry theGeom;

    createGeometry(theGeom);

    theGeom.debugPrint();

    // ==== test a variety of locations and directions
    std::vector< doubleVec > locations(4);
    std::vector< doubleVec > directions(4);

    int index = 0;
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 2; i++) {
            locations[index].assign(3, 0.0);
            locations[index][0] = -0.5 + i * 1.0;
            locations[index][1] = -0.5 + j * 1.0;

            directions[index].assign(3, 0.0);
            directions[index][i] = (1.0 - 2*j);

            cout << "Location  " << index << ": " << locations[index] << endl;
            cout << "Direction " << index << ": " << directions[index] << endl;
            index++;
        }
    }

    // ==== starting cells
    unsigned int expectedStartingCells[] = {30, 10, 20, 10};

    for (int i = 0; i < 4; i++) {
        TESTER_CHECKFORPASS(theGeom.findCell(locations[i]) ==
                expectedStartingCells[i]);
    }

    // ==== ending cells
    unsigned int expectedEndingCells[][4]
        = { {10, 20, 60, 50},
            {60, 40, 30, 50},
            {10, 40, 60, 30},
            {60, 40, 20, 50} };

    doubleVec    newPosition;
    unsigned int newCellId;
    double       distance;
    MCGeometry::ReturnStatus returnStatus;

    for (int startLoc = 0; startLoc < 4; startLoc++) {
        for (int dir = 0; dir < 4; dir++) {
            theGeom.intersect(  locations[startLoc],
                                directions[dir],
                                expectedStartingCells[startLoc],
                                newPosition,
                                newCellId,
                                distance,
                                returnStatus);

          TESTER_CHECKFORPASS(newCellId == expectedEndingCells[startLoc][dir]);
        }
    }

}

/*============================================================================*/
int main(int argc, char *argv[]) {
   TESTER_INIT("MCGeometry");
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
