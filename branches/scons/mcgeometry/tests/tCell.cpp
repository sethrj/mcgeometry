/*!
 * \file tCell.cpp
 * \brief Unit tests for Cell
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "mcgeometry/Cell.hpp"
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Sphere.hpp"

#include "mcgeometry/Cell.i.hpp"
#include "mcgeometry/Plane.i.hpp"
#include "mcgeometry/Sphere.i.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;

typedef blitz::TinyVector<double, 3> TVecDbl;
typedef Cell<unsigned int> CellT;

/*============================================================================*/
void runTests() {   

    /* * * create sphere * * */
    TVecDbl center(0.0);
    double    sphRadius = 2.0;

    Sphere    theSphere(center, sphRadius);

    /* * * create plane * * */
    TVecDbl normal(0.0);

    normal[0] = 1.0;

    center[0] = 1.0;

    Plane thePlane(normal, center);

    /* * * create cell boundaries * * */
    CellT::SASVec cellBoundaries;

    // define it as inside sphere to the right of plane
    cellBoundaries.push_back(std::make_pair(&theSphere, false));
    cellBoundaries.push_back(std::make_pair(&thePlane,  true));


    /* * * create the cell * * */
    CellT theCell(cellBoundaries, 211, 1);
    TESTER_CHECKFORPASS(theCell.getUserId() == 211);
    TESTER_CHECKFORPASS(theCell.getIndex() == 1);
    TESTER_CHECKFORPASS(theCell.isDeadCell() == false);

    /* * * CHECK VARIOUS POINTS * * */
    TVecDbl particleLoc(0.0);

    particleLoc[0] = 1.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == true);

    particleLoc[0] = 0.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == false);

    particleLoc[1] = 3.5;
    TESTER_CHECKFORPASS(theCell.isPointInside(particleLoc) == false);

    /* * * FIND DISTANCES * * */
    particleLoc[0] = 1.5;
    particleLoc[1] = 0.0;

    TVecDbl particleDir(0.0);

    particleDir[0] = -1.0;

    Surface* hitSurface;
    bool     quadricSense;
    double   distance;

    theCell.intersect(particleLoc, particleDir, hitSurface, quadricSense,
                      distance);

    TESTER_CHECKFORPASS( hitSurface   == &thePlane );
    TESTER_CHECKFORPASS( quadricSense == true );
    TESTER_CHECKFORPASS( softEquiv(distance, 0.5) );
    
    particleLoc[0] = 1.5;
    particleDir[0] = 1.0;

//    cout << theSphere << endl << thePlane << endl;
//    cout << "Position: " << particleLoc
//         << " Direction: " << particleDir << endl;

    theCell.intersect(particleLoc, particleDir, hitSurface, quadricSense,
                      distance);

    TESTER_CHECKFORPASS( hitSurface   == &theSphere );
    TESTER_CHECKFORPASS( quadricSense == false );
    TESTER_CHECKFORPASS( softEquiv(distance, 0.5) );

    // =================================================================== //
    /* * * check stuff with negated cells * * */
    CellT::CellFlags invFlags =
        static_cast<CellT::CellFlags>(CellT::NEGATED | CellT::DEADCELL);

    // define it as not ( inside sphere to the left of plane)
    cellBoundaries.resize(0);
    cellBoundaries.push_back(std::make_pair(&theSphere, false));
    cellBoundaries.push_back(std::make_pair(&thePlane,  false));

    /* * * create the cell * * */
    CellT invCell(cellBoundaries, 1337, 3, invFlags);
    TESTER_CHECKFORPASS(invCell.getUserId() == 1337);
    TESTER_CHECKFORPASS(invCell.getIndex() == 3);
    TESTER_CHECKFORPASS(invCell.isDeadCell() == true);
    TESTER_CHECKFORPASS(invCell.isNegated() == true);

    particleLoc = 0.0;
    particleLoc[0] = 2.5;
    TESTER_CHECKFORPASS(invCell.isPointInside(particleLoc) == true);

    particleLoc[0] = 1.5;
    TESTER_CHECKFORPASS(invCell.isPointInside(particleLoc) == true);

    particleLoc[0] = 0.5;
    TESTER_CHECKFORPASS(invCell.isPointInside(particleLoc) == false);

    particleLoc[0] = -2.5;
    TESTER_CHECKFORPASS(invCell.isPointInside(particleLoc) == true);
}

/*============================================================================*/
int main(int, char**) {
   TESTER_INIT("Cell");
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
