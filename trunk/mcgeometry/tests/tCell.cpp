/*!
 * \file tCell.cpp
 * \brief Unit tests for Cell
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "mcgeometry/Cell.hpp"
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Sphere.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;

/*============================================================================*/
void runTests() {   

    /* * * create sphere * * */
    doubleVec center(3,0.0);
    double    sphRadius = 2.0;

    Sphere    theSphere(center, sphRadius);

    /* * * create plane * * */
    doubleVec normal(3,0.0);

    normal[0] = 1.0;

    center[0] = 1.0;

    Plane thePlane(normal, center);

    /* * * create cell boundaries * * */
    Cell::QASVec cellBoundaries(2);

    // define it as inside sphere to the right of plane
    cellBoundaries[0] = std::make_pair(&theSphere, false);
    cellBoundaries[1] = std::make_pair(&thePlane,  true);


    /* * * create the cell * * */
    Cell theCell(cellBoundaries);

    /* * * CHECK VARIOUS POINTS * * */
    doubleVec particleLoc(3,0.0);

    particleLoc[0] = 1.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == true);

    particleLoc[0] = 0.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == false);

    particleLoc[1] = 3.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == false);

    /* * * FIND DISTANCES * * */
    particleLoc[0] = 1.5;
    particleLoc[1] = 0.0;

    doubleVec particleDir(3,0.0);

    particleDir[0] = -1.0;

    Quadric* hitQuadric;
    bool     quadricSense;
    double   distance;

    theCell.intersect(particleLoc, particleDir, hitQuadric, quadricSense,
                      distance);

    TESTER_CHECKFORPASS( hitQuadric   == &thePlane );
    TESTER_CHECKFORPASS( quadricSense == true );
    TESTER_CHECKFORPASS( softEquiv(distance, 0.5) );
    
    particleLoc[0] = 1.5;
    particleDir[0] = 1.0;

//    cout << theSphere << endl << thePlane << endl;
//    cout << "Position: " << particleLoc
//         << " Direction: " << particleDir << endl;

    theCell.intersect(particleLoc, particleDir, hitQuadric, quadricSense,
                      distance);

    TESTER_CHECKFORPASS( hitQuadric   == &theSphere );
    TESTER_CHECKFORPASS( quadricSense == false );
    TESTER_CHECKFORPASS( softEquiv(distance, 0.5) );
}

/*============================================================================*/
int main(int argc, char *argv[]) {
   TESTER_INIT("Cell");
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
