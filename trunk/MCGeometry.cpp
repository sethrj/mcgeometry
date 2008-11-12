/*!
 * \file MCGeometry.cpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 */

#include <iostream>
#include <utility>
#include <map>
#include "transupport/dbc.hpp"
#include "Surfaces/Quadric.hpp"
#include "Cell.hpp"
#include "MCGeometry.hpp"

/*----------------------------------------------------------------------------*/

bool MCGeometry::addSurface( const unsigned int surfaceId,
                                    const Quadric& inQuadric)
{
    // (the return value from "insert" has a weird type)
    typedef std::pair<SurfaceMap::iterator, bool> ReturnedPair;

    // allocate new const quadric using the input value
    CQuadric* newQuadric = new Quadric(inQuadric);

    ReturnedPair result =
        _Surfaces.insert( std::make_pair<surfaceId, inQuadric> );

    // check return value to make sure surfaceId was not already taken
    Insist(result.first == true,
            "Tried to add a surface with an ID that was already there.");

}

/*----------------------------------------------------------------------------*/
bool MCGeometry::addCell(const unsigned int cellId, const IntVec surfaces)
{
    using Cell::QuadricAndSense;
}

/*----------------------------------------------------------------------------*/
MCGeometry::~MCGeometry() {
    // clean up after ourselves
    

    SurfaceMap::iterator it = _Surfaces.begin();
    
    while (it != _Surfaces.end()) {
        // delete the surface to which the iterator points
        delete *it;

        ++it;
    }
    
    CellMap::iterator it = _Cells.begin();
    
    while (it != _Cells.end()) {
        // delete the cell to which the iterator points
        delete *it;

        ++it;
    }
}
/*----------------------------------------------------------------------------*/
#endif

