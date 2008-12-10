/*!
 * \file trickyGeometry.cpp
 * \brief Test some tricky geometries
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "createGeometry.hpp"
#include "mcgeometry/MCGeometry.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "transupport/constants.hpp"
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
inline void randPosition(   const std::vector<double> bounds,
                            const std::vector<double> subtract,
                            std::vector<double>& position )
{
    Require(bounds.size() == 3);
    Require(subtract.size() == 3);
    Require(position.size() == 3);
    // choose x in (-2,2)   y in (0,4)  z in (-2, 2)
    //
    for(int i = 0; i < 3; i++){
        position[i] = bounds[i] * randDouble() - subtract[i];
    }
}
/*============================================================================*/
void visualizeSurfaces( mcGeometry::MCGeometry& geo,
                        const std::string& fileName,
                        const std::vector<double> bounds,
                        const std::vector<double> subtract)
{
    std::fstream outFile;

    outFile.open(fileName.c_str(), std::fstream::out);
    doubleVec    direction(3, 0.0);
    doubleVec    position(3, 0.0);
    doubleVec    newPosition(3, 0.0);

    unsigned int oldCellIndex;
    unsigned int newCellIndex;
    double       distance;
    MCGeometry::ReturnStatus returnStatus;
    unsigned int surfaceCrossId;
    double       dotProduct;
    unsigned int j = 0;

    for (int i = 0; (i < 10000) && (j < 20000); i++) {
        randDirection(direction);

        do {
            randPosition(bounds, subtract, position);
            oldCellIndex = geo.findCell(position);
            j++;
        } while ( geo.isDeadCell(oldCellIndex) && (j < 20000));

        returnStatus = MCGeometry::NORMAL;
        while (returnStatus != MCGeometry::DEADCELL)
        {
            geo.findDistance(position, direction, oldCellIndex, distance);
            geo.findNewCell( position, direction,
                               newPosition, newCellIndex, returnStatus);
            geo.getSurfaceCrossing( newPosition, direction, surfaceCrossId,
                    dotProduct);

            // print the surface crossing position
            outFile << surfaceCrossId << "\t" << newPosition[0] << "\t"
                    << newPosition[1] << "\t" << newPosition[2] << endl;

            oldCellIndex = newCellIndex;
            position = newPosition;

            j++;
        }
    }
    outFile.close();
}
