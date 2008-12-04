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
void transport(MCGeometry& theGeom, unsigned int oldCellIndex,
               doubleVec& position, doubleVec& direction)
{
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

        if (oldCellIndex == newCellIndex) {
            cout << "Particle is stuck! :(" << endl;
            cout << "New position: " << newPosition 
                 << "; New cell index: " << newCellIndex << endl;
            return;
        }

        oldCellIndex = newCellIndex;
        position = newPosition;

        if (returnStatus == MCGeometry::DEADCELL) {
            cout << "Particle is deeeeaaaad at " << newPosition
                 << " in cell index " << newCellIndex << endl;
        } else if (returnStatus != MCGeometry::NORMAL) {
            cout << "Got unexpected return status " << returnStatus << endl;
            return;
        }
    }
}
/*============================================================================*/
void run(MCGeometry& theGeom)
{
    unsigned int oldCellIndex;
    doubleVec position(3, 0.0);
    doubleVec direction(3, 0.0);

    cout << "**********Streaming along a plane**********" << endl;
    direction[1] = 1.0;
    oldCellIndex = theGeom.getCellIndexFromUserId(1);

    transport(theGeom, oldCellIndex, position, direction);

    cout << "**********Streaming into corners**********" << endl;
    position[0] = 2.0;
    position[1] = 4.0;
    position[2] = 0.0;

    direction[0] = -tranSupport::constants::SQRTHALF;
    direction[1] = -tranSupport::constants::SQRTHALF;
    direction[2] = 0;

    oldCellIndex = theGeom.getCellIndexFromUserId(7);

    transport(theGeom, oldCellIndex, position, direction);

    cout << "**********Streaming into corners (perturbed) **********" << endl;
    position[0] = 1.999999999999999;
    position[1] = 4.0;
    position[2] = 0.0;

    direction[0] = -tranSupport::constants::SQRTHALF;
    direction[1] = -tranSupport::constants::SQRTHALF;
    direction[2] = 0;

    oldCellIndex = theGeom.getCellIndexFromUserId(7);

    transport(theGeom, oldCellIndex, position, direction);
}
/*============================================================================*/
void runAnother() {
    MCGeometry geom2;
    unsigned int numSides = 3;

    CreateMesh(numSides, geom2);

    cout << "**********Streaming into many corners from bottom left front"
         << endl;
    unsigned int oldCellIndex;
    doubleVec position(3, 0);
    doubleVec direction(3, 0.0);

    direction[0] = tranSupport::constants::SQRTTHIRD;
    direction[1] = tranSupport::constants::SQRTTHIRD;
    direction[2] = tranSupport::constants::SQRTTHIRD;

    oldCellIndex = geom2.findCell(position);
    transport(geom2, oldCellIndex, position, direction);

    cout << "**********Streaming into many cornders from upper right back"
         << endl;
    position.assign(3, numSides);

    direction[0] = -tranSupport::constants::SQRTTHIRD;
    direction[1] = -tranSupport::constants::SQRTTHIRD;
    direction[2] = -tranSupport::constants::SQRTTHIRD;

    oldCellIndex = 26;
    cout << "Quitting because the next one will fail :(" << endl;
return;
    transport(geom2, oldCellIndex, position, direction);

    // throw particles around
    doubleVec    newPosition;
    unsigned int newCellIndex;
    double       distance;
    MCGeometry::ReturnStatus returnStatus = MCGeometry::NORMAL;
    for (int i = 0; i < 10000; i++) {
        randDirection(direction);

        position[0] = randDouble() * numSides;
        position[1] = randDouble() * numSides;
        position[2] = randDouble() * numSides;

        oldCellIndex = geom2.findCell(position);

        geom2.findDistance(position, direction, oldCellIndex, distance);
        geom2.findNewCell( position, direction,
                           newPosition, newCellIndex, returnStatus);
    }
    geom2.debugPrint();
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    MCGeometry geom;
    try {
        createTrickyGeometry(geom);
        testGeometry(geom);
//        geom.debugPrint();
        run(geom);
        runAnother();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "FAILURE: CAUGHT ERROR" << endl
             << theErr.what() << endl;
    }

    return 0;
}
