/*!
 * \file trickyGeometry.cpp
 * \brief Test some tricky geometries
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "createGeometry.hpp"
#include "mcgeometry/MCGeometry.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include "transupport/constants.hpp"
#include "transupport/VectorPrint.hpp"
#include "extra/mtrand.h"

using namespace mcGeometry;

using std::cout;
using std::endl;

typedef std::vector<double> doubleVec;
typedef std::vector<signed int> intVec;

/*============================================================================*/
mtRand::MTRand randGen;

//! choose a random double
inline double randDouble(){
    return randGen();
}
//! Pick a random direction on the unit sphere
inline void randDirection(std::vector<double>& v){
    double phi = tranSupport::constants::TWOPI*randDouble();
    Require(v.size() == 3);
    
    v[0] = 2*randDouble() - 1;
    double mu = std::sqrt(1 - v[0]*v[0]);
    v[1] = mu*std::cos(phi);
    v[2] = mu*std::sin(phi);
}
//! Pick a random position in our geometry
inline void randPosition( std::vector<double>& position ){
    // choose x in (-2,2)   y in (0,4)  z in (-2, 2)
    //
    std::vector<double>::iterator vecIter;
    for(vecIter = position.begin(); vecIter != position.end(); ++vecIter){
        *vecIter = 4*randDouble();
    }
    position[0] -= 2.0;
    position[2] -= 2.0;
}

/*============================================================================*/
//! throw around a bunch of particles to hopefully make sure there are no dead
//  spots, and to build the connectivity 
void testGeometry(MCGeometry& theGeom)
{
    doubleVec position(3, 0.0);
    doubleVec direction(3, 0.0);

    doubleVec    newPosition;
    unsigned int oldCellIndex;
    unsigned int newCellIndex;
    double       distance;
    MCGeometry::ReturnStatus returnStatus = MCGeometry::NORMAL;
    for (int i = 0; i < 1000; i++) {
        randDirection(direction);
        randPosition(position);
        oldCellIndex = theGeom.findCell(position);

        theGeom.findDistance(position, direction, oldCellIndex, distance);
        theGeom.findNewCell(position, direction,
                            newPosition, newCellIndex, returnStatus);
    }
}
/*============================================================================*/
void run(MCGeometry& theGeom)
{
    unsigned int oldCellIndex;
    doubleVec position(3, 0.0);
    doubleVec direction(3, 0.0);

    direction[1] = 1.0;
    oldCellIndex = theGeom.getCellIndexFromUserId(1);

    doubleVec    newPosition;
    unsigned int newCellIndex;
    double       distance;
    MCGeometry::ReturnStatus returnStatus = MCGeometry::NORMAL;

    while (returnStatus != MCGeometry::DEADCELL)
    {
        cout << "Particle at " << position << " with direction " << direction 
             << " in cell index " << oldCellIndex << endl;
        theGeom.findDistance(position, direction, oldCellIndex, distance);
        cout << "Distance to next surface: " << distance << endl;

        theGeom.findNewCell(position, direction,
                            newPosition, newCellIndex, returnStatus);
        cout << "New position: " << newPosition 
             << "; New cell index: " << newCellIndex << endl;

        if (oldCellIndex == newCellIndex) {
            cout << "Particle is stuck! :(" << endl;
            return;
        }

        oldCellIndex = newCellIndex;
        position = newPosition;

        if (returnStatus == MCGeometry::DEADCELL) {
            cout << "Particle is deeeeaaaad." << endl;
        } else if (returnStatus != MCGeometry::NORMAL) {
            cout << "Got unexpected return status " << returnStatus << endl;
            return;
        }
    }
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    MCGeometry geom;
    try {
        createTrickyGeometry(geom);
        testGeometry(geom);
//        geom.debugPrint();
        run(geom);
    }
    catch (tranSupport::tranError &theErr) {
        cout << "FAILURE: CAUGHT ERROR" << endl
             << theErr.what() << endl;
    }

    return 0;
}
