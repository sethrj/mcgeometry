/*!
 * \file MCGeometry.cpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 */

//#include <iostream>
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

    // this calls a routine in the quadric which allocates new memory
    // WE ARE NOW RESPONSIBLE FOR THIS MEMORY (and must delete it when
    // appropriate)
    Quadric* newQuadric = inQuadric.clone();

    ReturnedPair result =
        _surfaces.insert( std::make_pair(surfaceId, newQuadric) );

    // check return value to make sure surfaceId was not already taken
    Insist(result.second == true,
            "Tried to add a surface with an ID that was already there.");
}

/*----------------------------------------------------------------------------*/
bool MCGeometry::addCell(const unsigned int cellId, const IntVec surfaceIds)
{
//    using Cell::QuadricAndSense;
}

/*----------------------------------------------------------------------------*/
MCGeometry::~MCGeometry()
{
    // clean up after ourselves
    

    SurfaceMap::iterator itSur = _surfaces.begin();
    
    while (itSur != _surfaces.end()) {
        // delete the surface to which the iterator points
        delete itSur->second;

        ++itSur;
    }
    
    CellMap::iterator itCel = _cells.begin();
    
    while (itCel != _cells.end()) {
        // delete the cell to which the iterator points
        delete itCel->second;

        ++itCel;
    }
}
/*----------------------------------------------------------------------------*/

