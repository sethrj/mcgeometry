/*!
 * \file MCGeometry.cpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 * The MCGeometry parent class handles all the external associations between
 * Surfaces and Cells. It maps the user's IDs to internal pointers for both
 * surfaces and cells.
 */

#include <utility>
#include <map>
#include "transupport/dbc.hpp"
#include "MCGeometry.hpp"
#include "Quadric.hpp"
#include "Cell.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/

void MCGeometry::addSurface( const unsigned int surfaceId,
                                    const Quadric& inQuadric)
{
    Insist(surfaceId > 0, "Things will break if surfaceId = 0 is allowed.");

    // (the return value from "insert" has a weird type)
    typedef std::pair<SurfaceMap::iterator, bool> ReturnedPair;

    // "clone" calls a routine in the quadric which allocates new memory
    // WE ARE NOW RESPONSIBLE FOR THIS MEMORY (and must delete it when
    // appropriate)
    Quadric* newQuadric = inQuadric.clone();

    ReturnedPair result =
        _surfaces.insert( std::make_pair(surfaceId, newQuadric) );

    // check return value to make sure surfaceId was not already taken
    Insist(result.second == true,
            "Tried to add a surface with an ID that was already there.");


    // **** temporary code? ****
    _surfaceIds.insert( std::make_pair(newQuadric, surfaceId) );

}

/*----------------------------------------------------------------------------*/
void MCGeometry::addCell(const unsigned int cellId, const IntVec surfaceIds)
{
    typedef std::pair<CellMap::iterator, bool>      CMReturnedPair;
    typedef std::pair<SCConnectMap::iterator, bool> SCCMReturnedPair;

    //---- convert surface ID to pairs of unsigned ints (surfaceIds) and bools
    //                                                  (surface sense) -----//
    QASVec boundingSurfaces;

    // bounding surfaces should have same length as input list of surface IDs
    // so from the start just reserve that many spaces in memory
    boundingSurfaces.reserve(surfaceIds.size());


    // -------- ITERATE OVER INPUT SURFACE LIST -------- //
    
    for (IntVec::const_iterator it = surfaceIds.begin();
                                it != surfaceIds.end(); ++it) {
        unsigned int userSurfaceId; //an integer that only the user deals with
        QuadricAndSense newSurface;

        if (*it > 0) // user inputs a positive value (positive sense surface)
        {
            userSurfaceId     = *it;
            newSurface.second = true;
        }
        else // user input a negative value (negative surface sense)
        {
            userSurfaceId     = -(*it);
            newSurface.second = false;
        }

        // translate the user's given surface ID to a Quadric pointer
        SurfaceMap::iterator findSMResult = 
            _surfaces.find(userSurfaceId);

        if (findSMResult == _surfaces.end()) {
            Insist(0,
            "FATAL ERROR: this cell references a surface that does not exist.");
        }
        // the value from the find result is the pointer to the Quadric
        newSurface.first = findSMResult->second;

        // add the surface to the vector of bounding surfaces
        boundingSurfaces.push_back(newSurface);

        // this counts as another unmatched surface
        _unMatchedSurfaces++;
//        cout << "Added surface " << userSurfaceId << " to cell " << cellId
//             << " (unmatched surface count: " << _unMatchedSurfaces << ")"
//             << endl;
    }

    Check(surfaceIds.size() == boundingSurfaces.size());

    // TODO: make sure surfaces inside each cell are unique (no duplications or
    // having both plus and minus)

    //====== add cell to the map
    Cell* newCell = new Cell(boundingSurfaces, cellId);
    CMReturnedPair result =
        _cells.insert( std::make_pair(cellId, newCell) );

    // check return value to make sure surfaceId was not already taken
    Insist(result.second == true,
            "Tried to add a cell with an ID that was already there.");

    //====== loop back through the surfaces and add the connectivity
    for (QASVec::iterator bsIt = boundingSurfaces.begin();
                          bsIt != boundingSurfaces.end(); ++bsIt)
    {
        QuadricAndSense &newQandS = *bsIt;

        // using the "associative array" capability of maps lets us access a
        // key, and it will either automatically initialize an empty vector or
        // return the vector that is already  
        // see C++ Standard Library pp. 182-183
        _surfToCellConnectivity[newQandS].push_back(newCell);
    }
}
/*----------------------------------------------------------------------------*/
void MCGeometry::debugPrint() const
{
    //-------------- PRINT SURFACES ----------------//
    cout << "SURFACES: " << endl;
    SurfaceMap::const_iterator itSur = _surfaces.begin();
    
    while (itSur != _surfaces.end()) {
        cout << " SURFACE " << itSur->first << ": "
             << *(itSur->second) << endl;
        ++itSur;
    }

    //-------------- PRINT CELLS TO SURFACES ----------------//
    cout << "CELLS: " << endl;
    CellMap::const_iterator celIt = _cells.begin();
    
    while (celIt != _cells.end()) {
        cout << " CELL " << celIt->first << ": ";

        // query cell for surface pointers
        const QASVec& boundingSurfaces = celIt->second->getBoundingSurfaces();

        QASVec::const_iterator bsIt = boundingSurfaces.begin();

        while (bsIt != boundingSurfaces.end()) {

            _printQAS(*bsIt);
            cout << " ";

            ++bsIt;
        }

        cout << endl;
        ++celIt;
    }

    cout << "SURFACES TO CELLS: " << endl;
    //-------------- PRINT SURFACES TO CELLS ----------------//
    SCConnectMap::const_iterator surfToCellIt = _surfToCellConnectivity.begin();
    
    while (surfToCellIt != _surfToCellConnectivity.end()) {
        // print the surface and orientation
        cout << " ";

        _printQAS(surfToCellIt->first);

        cout << ": ";
        
        // print the cells
        const CellVec& theCells = surfToCellIt->second;

        CellVec::const_iterator cIt = theCells.begin();
        while (cIt != theCells.end()) {
            cout << (*cIt)->getUserId() << " ";

            ++cIt;
        }

        cout << endl;
        ++surfToCellIt;
    }

    cout << "NEIGHBORHOOD CONNECTIVITY: " << endl;
    //-------------- PRINT CELLS TO CELLS ----------------//
    celIt = _cells.begin();
    
    while (celIt != _cells.end()) {
        cout << " CELL " << celIt->first << ": ";

        // query cell for surface pointers
        const QASVec& boundingSurfaces = celIt->second->getBoundingSurfaces();

        QASVec::const_iterator bsIt = boundingSurfaces.begin();

        while (bsIt != boundingSurfaces.end()) {

            _printQAS(*bsIt);
            cout << ":{";

            const CellVec& otherCells =
                celIt->second->getNeighbors(bsIt->first);

            CellVec::const_iterator outCelIt = otherCells.begin();

            while (outCelIt != otherCells.end()) {
                cout << (*outCelIt)->getUserId() << " ";

                ++outCelIt;
            }
            cout << "} ";
            ++bsIt;
        }

        cout << endl;
        ++celIt;
    }  
}

/*----------------------------------------------------------------------------*/
void MCGeometry::_printQAS(const QuadricAndSense& qas) const
{
    if (qas.second == true)
        cout << "+";
    else
        cout << "-";

    SurfaceUserIdMap::const_iterator findResult = _surfaceIds.find(qas.first);
    Require(findResult != _surfaceIds.end());
    // print the "value" corresponding to the key (i.e. unsigned int)
    cout << findResult->second;
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
    
    CellMap::iterator celIt = _cells.begin();
    
    while (celIt != _cells.end()) {
        // delete the cell to which the iterator points
        delete celIt->second;

        ++celIt;
    }
}

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
