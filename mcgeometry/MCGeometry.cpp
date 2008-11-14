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
#include "Quadric.hpp"
#include "Cell.hpp"
#include "MCGeometry.hpp"

/*----------------------------------------------------------------------------*/

void MCGeometry::addSurface( const unsigned int surfaceId,
                                    const Quadric& inQuadric)
{
    // (the return value from "insert" has a weird type)
    typedef std::pair<SurfaceMap::iterator, bool> ReturnedPair;

    // "clone" calls a routine in the quadric which allocates new memory
    // WE ARE NOW RESPONSIBLE FOR THIS MEMORY (and must delete it when
    // appropriate)
    ReturnedPair result =
        _surfaces.insert( std::make_pair(surfaceId, inQuadric.clone()) );

    // check return value to make sure surfaceId was not already taken
    Insist(result.second == true,
            "Tried to add a surface with an ID that was already there.");

}

/*----------------------------------------------------------------------------*/
void MCGeometry::addCell(const unsigned int cellId, const IntVec surfaceIds)
{
    typedef std::pair<CellMap::iterator, bool> ReturnedPair;

//    //====== convert surface ID to pairs of unsigned ints (surfaceIds) and bools
//    // (surface sense)
//    std::vector<Cell::QuadricAndSense> boundingSurfaces;
//
//    IntVec::const_iterator it = surfaceIds.begin();
//
//    bool surfaceSense;
//    unsigned int surfaceId;
//    while(it != surfaceIds.end()) {
//        // user inputs a positive value
//        if (*it > 0)
//        {
//            surfaceSense = true;
//            surfaceId    = *it;
//        }
//        else
//        {
//            surfaceSense = false;
//            surfaceId    = -(*it);
//        }
//
//        ++it;
//    }
//
//    //====== add cell to the map
//    Cell* newCell = new Cell(boundingSurfaces);
//    ReturnedPair result =
//        _surfaces.insert( std::make_pair(cellId, newCell) );
//
//    // check return value to make sure surfaceId was not already taken
//    Insist(result.second == true,
//            "Tried to add a cell with an ID that was already there.");

    //====== add surface/positivity to the vector of surface to cell
    //       connectivity


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

