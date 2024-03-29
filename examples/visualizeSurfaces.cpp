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
#include <cmath>
#include <blitz/tinyvec-et.h>
#include "transupport/constants.hpp"
#include "extra/mtrand.h"

using namespace mcGeometry;

using std::cout;
using std::endl;

//! Blitz++ TinyVector of length D stores position/direction/etc.
typedef blitz::TinyVector<double, 3> TVecDbl;

/*============================================================================*/
mtRand::MTRand randGen;

//! choose a random double
inline double randDouble(){
    return randGen();
}

//! Pick a random direction on the unit sphere
inline void randDirection(TVecDbl& v){
    double phi = randDouble() * tranSupport::constants::TWOPI;
    double mu  = 2*randDouble() - 1;

    double sinPolar = std::sqrt(1 - mu * mu);

    v[0] = mu;
    v[1] = sinPolar*std::cos(phi);
    v[2] = sinPolar*std::sin(phi);
}

//! Pick a random position in our geometry
inline void randPosition(   const TVecDbl& bounds,
                            const TVecDbl& subtract,
                            TVecDbl& position )
{
    position = randDouble(), randDouble(), randDouble();
    position *= bounds;
    position -= subtract;
}
/*============================================================================*/
void visualizeSurfaces( mcGeometry::MCGeometry& geo,
                        const std::string& fileName,
                        const TVecDbl& bounds,
                        const TVecDbl& subtract)
{
    std::fstream outFile;

    outFile.open(fileName.c_str(), std::fstream::out);
    TVecDbl    direction(0.0);
    TVecDbl    position(0.0);
    TVecDbl    newPosition(0.0);

    unsigned int oldCellIndex;
    unsigned int newCellIndex;
    double       distance;
    MCGeometry::ReturnStatus returnStatus;
    unsigned int surfaceCrossId;
    double       dotProduct;
    unsigned int j = 0;

    for (int i = 0; (i < 10000) && (j < 20000); i++) {
        randDirection(direction);

        // sample in a box until we're somewhere inside the valid geometry
        do {
            randPosition(bounds, subtract, position);
            oldCellIndex = geo.findCell(position);
            j++;
        } while ( geo.isDeadCell(oldCellIndex) && (j < 20000));

        // cross surfaces until we leave the system
        returnStatus = MCGeometry::NORMAL;
        while (returnStatus != MCGeometry::DEADCELL)
        {
            geo.findDistance(position, direction, oldCellIndex, distance);
            geo.findNewCell( position, direction,
                               newPosition, newCellIndex, returnStatus);
            geo.getSurfaceCrossing( newPosition, direction, surfaceCrossId,
                    dotProduct);

            // write the surface crossing position to the file
            outFile << surfaceCrossId << "\t" << newPosition[0] << "\t"
                    << newPosition[1] << "\t" << newPosition[2] << endl;

            oldCellIndex = newCellIndex;
            position = newPosition;

            j++;
        }
    }
    outFile.close();
}
