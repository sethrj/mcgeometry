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

    // allocate new const quadric using the input value
    /* * * * * * * * * * * \
     * WRONG WRONG WRONG
     * WE NEED TO CREATE OUR OWN COPY OF INQUADRIC, OR RATHER HAVE IT
     * COPY ITSELF AND SEND THAT TO US
     * SEE "VIRTUAL COPY CONSTRUCTOR"
     * http://en.wikibooks.org/wiki/C%2B%2B_Programming/Classes/Polymorphism
     * * * * * * * * * * */
    Quadric* newQuadric = &inQuadric; //new Quadric(inQuadric);

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

