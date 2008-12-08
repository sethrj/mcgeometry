/*!
 * \file trickyGeometry.cpp
 * \brief Test some tricky geometries
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "createGeometry.hpp"
#include "visualizeSurfaces.hpp"
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
             << " in cell user ID "
             << theGeom.getUserIdFromCellIndex(oldCellIndex) << endl;

        theGeom.findDistance(position, direction, oldCellIndex, distance);
        cout << "Distance to next surface: " << distance << endl;

        theGeom.findNewCell(position, direction,
                            newPosition, newCellIndex, returnStatus);

        oldCellIndex = newCellIndex;
        position = newPosition;

        if (returnStatus == MCGeometry::DEADCELL) {
            cout << "Particle is deeeeaaaad at " << newPosition
                 << " in cell user ID " 
                 << theGeom.getUserIdFromCellIndex(newCellIndex) << endl;
        } else if (returnStatus != MCGeometry::NORMAL) {
            cout << "Got unexpected return status " << returnStatus << endl;
            return;
        }
    }
}
/*============================================================================*/
void testAmrGeometry()
{
    MCGeometry theGeom;
    createTrickyGeometry(theGeom);

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

    // throw around a bunch of particles to hopefully make sure there are
    // no dead spots, and to build the connectivity 
    cout << "********** printing surfaces"
         << endl;
    doubleVec bounds(3, 4.0);
    doubleVec subtract;
    subtract.push_back(2.0);
    subtract.push_back(0.0);
    subtract.push_back(2.0);

    visualizeSurfaces(theGeom, "amrOut.txt", bounds, subtract);

//    theGeom.debugPrint();
}
/*============================================================================*/
void testMeshGeometry() {
    MCGeometry geom2;
    unsigned int numSides = 4;

    CreateMesh(numSides, geom2);
//    geom2.debugPrint();
    unsigned int oldCellIndex;
    doubleVec position(3, 0);
    doubleVec direction(3, 0.0);

    cout << "**********Streaming into 2-corners from bottom left front"
         << endl;
    position.assign(3, 0.0);
    position[2] = 0.5;

    direction[0] = tranSupport::constants::SQRTHALF;
    direction[1] = tranSupport::constants::SQRTHALF;
    direction[2] = 0;

    oldCellIndex = 0;
    transport(geom2, oldCellIndex, position, direction);


    cout << "**********Streaming into 2-corners from upper right back"
         << endl;
    position.assign(3, numSides);
    position[2] = 0.5;

    direction[0] = -tranSupport::constants::SQRTHALF;
    direction[1] = -tranSupport::constants::SQRTHALF;
    direction[2] = 0;

    oldCellIndex = numSides * numSides - 1;
    transport(geom2, oldCellIndex, position, direction);

    cout << "**********JESSE TRANSPORT 5 to 2"
         << endl;
    position.assign(3, numSides);
    position[0] = 1.0;
    position[1] = 2.0;
    position[2] = 0.5;

    direction[0] = tranSupport::constants::SQRTHALF;
    direction[1] = -tranSupport::constants::SQRTHALF;
    direction[2] = 0;

    oldCellIndex = 5;
    transport(geom2, oldCellIndex, position, direction);

    cout << "**********Streaming into 3-corners from bottom left front"
         << endl;

    position.assign(3, 0.0);
    direction[0] = tranSupport::constants::SQRTTHIRD;
    direction[1] = tranSupport::constants::SQRTTHIRD;
    direction[2] = tranSupport::constants::SQRTTHIRD;

    oldCellIndex = geom2.findCell(position);
    transport(geom2, oldCellIndex, position, direction);
    cout << "**********Streaming into 3-corners from upper right back"
         << endl;
    position.assign(3, numSides);

    direction[0] = -tranSupport::constants::SQRTTHIRD;
    direction[1] = -tranSupport::constants::SQRTTHIRD;
    direction[2] = -tranSupport::constants::SQRTTHIRD;

    oldCellIndex = numSides * numSides * numSides - 1;
    transport(geom2, oldCellIndex, position, direction);

    cout << "**********Streaming into 3-corners from upper right back perturbed"
         << endl;

    position.assign(3, numSides);
    position[0] -= 1e-15;
    position[1] -= 2e-15;

    direction[0] = -tranSupport::constants::SQRTTHIRD;
    direction[1] = -tranSupport::constants::SQRTTHIRD;
    direction[2] = -tranSupport::constants::SQRTTHIRD;

    oldCellIndex = numSides * numSides * numSides - 1;
    transport(geom2, oldCellIndex, position, direction);

    // throw particles around
    cout << "********** printing surfaces"
         << endl;
    doubleVec bounds(3, numSides);
    doubleVec subtract(3, 0.0);
    visualizeSurfaces(geom2, "meshOut.txt", bounds, subtract);
}
/*============================================================================*/
void testSphereGeometry() {
    MCGeometry geom3;

    createAnotherTrickyGeometry(geom3);
//    geom3.debugPrint();

    unsigned int oldCellIndex;
    doubleVec position(3, 0);
    doubleVec direction(3, 0.0);

    cout << "**********Streaming into tangent spheres from left"
         << endl;
    direction[0] = 1.0;
    direction[1] = 0.0;
    direction[2] = 0.0;

    position[0]  = -1.0;
    oldCellIndex = geom3.findCell(position);
    transport(geom3, oldCellIndex, position, direction);

    cout << "**********Streaming into tangent spheres from right"
         << endl;
    direction[0] = -1.0;
    direction[1] = 0.0;
    direction[2] = 0.0;

    position[0]  = 1.0;
    oldCellIndex = geom3.findCell(position);
    transport(geom3, oldCellIndex, position, direction);

    cout << "**********Streaming into right tangent sphere/plane/cylinder"
         << endl;
    direction[0] = 0.0;
    direction[1] = 1.0;
    direction[2] = 0.0;

    position[0] = 1.0;
    position[1] = 0.0;
    position[2] = 0.0;
    oldCellIndex = geom3.findCell(position);
    transport(geom3, oldCellIndex, position, direction);

    cout << "**********Streaming into left tangent sphere/plane/cylinder"
         << endl;
    direction[0] = 0.0;
    direction[1] = 1.0;
    direction[2] = 0.0;

    position[0] = -1.0;
    position[1] = 0.0;
    position[2] = 0.0;
    oldCellIndex = geom3.findCell(position);
    transport(geom3, oldCellIndex, position, direction);

    cout << "**********Streaming in middle region"
         << endl;
    direction[0] = 0.0;
    direction[1] = 1.0;
    direction[2] = 0.0;

    position[0] = 0.0;
    position[1] = 0.01;
    position[2] = 0.0;
    oldCellIndex = geom3.findCell(position);
    transport(geom3, oldCellIndex, position, direction);

//    position[0] = -1.0;
//    position[1] = 1.5;
//    position[2] = 0.0;
//    cout << "Above left sphere: "
//         << geom3.getUserIdFromCellIndex(geom3.findCell(position)) << endl;
//
//    position[0] = -2.5;
//    position[1] = 0.0;
//    position[2] = 0.0;
//    cout << "Leftmost cell: "
//         << geom3.getUserIdFromCellIndex(geom3.findCell(position)) << endl;
//
//    position[0] = 2.5;
//    position[1] = 0.0;
//    position[2] = 0.0;
//    cout << "Rightmost cell: "
//         << geom3.getUserIdFromCellIndex(geom3.findCell(position)) << endl;

    // throw particles around
    cout << "********** printing surfaces"
         << endl;

    doubleVec bounds(3, 0.0);
    doubleVec subtract(3, 0.0);
    bounds[0] = 4.0; subtract[0] = 2.0;
    bounds[1] = 2.0; subtract[1] = 1.0;
    bounds[2] = 2.0; subtract[2] = 1.0;

    visualizeSurfaces(geom3, "sphereOut.txt", bounds, subtract);

//    geom3.debugPrint();
}

void printComplexGeometry()
{
    MCGeometry geo;
    createComplexGeometry(geo);

    doubleVec bounds(3, 6.0);
    doubleVec subtract(3, 3.0);

    visualizeSurfaces(geo, "complexOut.txt", bounds, subtract);
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    try {
        cout << "================== TRICKY GEOMETRY 1 (AMR) =================="
             << endl;
        testAmrGeometry();

        cout << "================== TRICKY CORNERS    =================="
             << endl;
        testMeshGeometry();

        cout << "================== TRICKY GEOMETRY (CURVES)=================="
             << endl;
        testSphereGeometry();

        printComplexGeometry();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "FAILURE: CAUGHT ERROR" << endl
             << theErr.what() << endl;
    }

    return 0;
}
