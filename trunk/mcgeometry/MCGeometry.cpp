/*!
 * \file MCGeometry.cpp
 * \brief MCGeometry class
 * \author Seth R. Johnson
 * 
 * MCGeometry holds the bulk of the .
 */

#include <utility>
#include <map>

#include "transupport/dbc.hpp"
#include "transupport/VectorPrint.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/SoftEquiv.hpp"

#include "MCGeometry.hpp"
#include "Surface.hpp"
#include "Cell.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace mcGeometry {
/*============================================================================*\
 * subroutines that the user calls while running their monte carlo code
\*============================================================================*/
void MCGeometry::findDistance(
                            const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            double& distanceTraveled)
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));
    Require(oldCellIndex < getNumCells());

    // call intersect on the old cell to find the surface and distance that it
    // moves to
    _cells[oldCellIndex]->intersect(
                                position, direction,
                                _findCache.hitSurface,
                                _findCache.oldSurfaceSense,
                                distanceTraveled);

    // cache variables for later
    _findCache.oldCellIndex = oldCellIndex;
    _findCache.distanceToSurface = distanceTraveled;
    IfDbc(_findCache.position = position; _findCache.direction = direction;)

    Check(_findCache.hitSurface != NULL);
    Ensure(distanceTraveled >= 0.0);
}
/*----------------------------------------------------------------------------*/
void MCGeometry::findNewCell(
                            const std::vector<double>& position,
                            const std::vector<double>& direction,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            ReturnStatus& returnStatus)
{
//    cout << "unmatched surface count: " << _unMatchedSurfaces << endl;
    Require(position  == _findCache.position);
    Require(direction == _findCache.direction);
    Require(_findCache.oldCellIndex < getNumCells());

    // a reference to the pointer to the old cell
    CellT& oldCell = *(_cells[_findCache.oldCellIndex]);

    // ===== calculate transported position on the boundary of our new cell
    newPosition.resize(3);
    for (int i = 0; i < 3; i++) {
        newPosition[i] = position[i] +
            _findCache.distanceToSurface * direction[i];
    }

    // ===== Loop over neighborhood cells
    CellT::CellContainer& neighborhood =
            oldCell.getNeighbors(_findCache.hitSurface);

    // loop through the old cell's hood to find if it's in one of those cells
    for (CellT::CellContainer::const_iterator it  = neighborhood.begin();
                                       it != neighborhood.end(); ++it)
    {
        // check if the point is inside (and pass _hitSurface to exclude
        // checking it)
        if ( (*it)->isPointInside( newPosition, _findCache.hitSurface ) )
        {
            //we have found the new cell
            newCellIndex = (*it)->getIndex();

//            cout << "Found ending cell index " << newCellIndex
//                 << " already connected to starting cell index "
//                 << _findCache.oldCellIndex << " through hood" << endl;

            if ( (*it)->isDeadCell() )
                returnStatus = DEADCELL;
            return;
        }
    }

    // ===== Loop through all cells that have the same surface as the one we
    // intersected, and the opposite surface sense (i.e. if in our cell we hit
    // surface 2, and our cell is defined as being -2, then the cell on the
    // other side has to have orientation +2

    SurfaceAndSense searchQas(_findCache.hitSurface,
                                !(_findCache.oldSurfaceSense));

    SCConnectMap::iterator cellList
                 = _surfToCellConnectivity.find(searchQas);

    if (cellList == _surfToCellConnectivity.end()) {
        // NOTE: if position and newPosition reference the same vector,
        // this will print the new position, not the starting position
        _failGeometry("Surface connectivity not found for surface",
                        _findCache.oldCellIndex, position, direction);
    }

    CellVec& cellsToCheck = cellList->second;

    for (CellVec::iterator pNewCell  = cellsToCheck.begin();
                           pNewCell != cellsToCheck.end(); ++pNewCell)
    {
        // check if the point is inside (and pass _hitSurface to exclude
        // checking it)
        if ( (*pNewCell)->isPointInside( newPosition, _findCache.hitSurface ) )
        {
            // if this is the first cell linked to this surface, we decrement 
            // the unmatched surfaces
            if (neighborhood.size() == 0)
                _unMatchedSurfaces--;

            // add new cell to old cell's hood connectivity
            neighborhood.push_back(*pNewCell);

            CellT::CellContainer& newNeighborhood
                = (*pNewCell)->getNeighbors(_findCache.hitSurface);

            // if this is the first cell linked to this surface, we decrement 
            // the unmatched surfaces
            if (newNeighborhood.size() == 0)
                _unMatchedSurfaces--;

            // add old cell to new cell's hood connectivity
            newNeighborhood.push_back(&oldCell);

            // we have found the new cell
            newCellIndex = (*pNewCell)->getIndex();

//            cout << "Connected ending cell index " << newCellIndex
//                 << " to starting cell index " << _findCache.oldCellIndex
//                 << endl;

            if (_unMatchedSurfaces == 0)
                _completedConnectivity();

            if ( (*pNewCell)->isDeadCell() )
                returnStatus = DEADCELL;
            return;
        }
    }

    // TODO: when looping through the other cells in the problem, exclude the
    // ones that we already checked in the cell's hood; means more loops but
    // fewer flops in the "is inside" method calls

    newCellIndex = -1;
    returnStatus = LOST;

    _failGeometry("Ruh-roh, new cell not found!",
                    _findCache.oldCellIndex, position, direction);
}
/*----------------------------------------------------------------------------*/
unsigned int MCGeometry::findCell(
                                const std::vector<double>& position) const
{
  //  // loop through all cells in problem
  //  for (CellVec::const_iterator itCel = _cells.begin();
  //                               itCel != _cells.end();  ++itCel)
  //  {
  //      if ((*itCel)->isPointInside(position))  {
  //          return itCel - _cells.begin(); // maybe valid?
  //      }
  //  }
    // that might be faster than this:
    for (unsigned int i = 0; i < _cells.size(); ++i) {
        if (_cells[i]->isPointInside(position)) {
            return i;
        }
    }

    // return a status value or something instead of failing miserably?
    _failGeometry("Could not find cell!", 0, position, std::vector<double>());
    return 0;
}

/*============================================================================*\
 * subroutines that the user calls to create the geometry
 * and/or otherwise interface with the user ID
\*============================================================================*/

unsigned int MCGeometry::addSurface( const MCGeometry::UserSurfaceIDType
                                                             userSurfaceId,
                                     const Surface& inSurface)
{
    // this is ONLY if we are using MCNP-type input definitions
    Insist(userSurfaceId > 0, "Things will break if surfaceId = 0 is allowed.");

    // "clone" calls a routine in the quadric which allocates new memory
    // WE ARE NOW RESPONSIBLE FOR THIS MEMORY (and must delete it when
    // appropriate)
    Surface* newSurface = inSurface.clone(userSurfaceId);

    _surfaces.push_back(newSurface);
    unsigned int newSurfaceIndex = _surfaces.size() - 1;

    // add the reverse mapping and 
    // verify that this surface has not been added already (i.e. the
    // UserSurfaceIDType is unique)
    std::pair<SurfaceRevIDMap::iterator, bool> result
        = _surfaceRevUserIds.insert(
                std::make_pair(userSurfaceId, newSurfaceIndex));

//    cout << "Added surface with ID " << userSurfaceId
//         << "that has index " << newSurfaceIndex << endl;

    Insist(result.second == true,
             "Tried to add a surface with an ID that was already there.");

    Check(_surfaceRevUserIds.size() == _surfaces.size());

    return newSurfaceIndex;
}

/*----------------------------------------------------------------------------*/
unsigned int MCGeometry::addCell(const MCGeometry::UserCellIDType userCellId,
                                 const IntVec surfaceIds,
                                 const CellT::CellFlags flags)
{
    //---- convert surface ID to pairs of unsigned ints (surfaceIds) and bools
    //                                                  (surface sense) -----//
    CellT::SASVec boundingSurfaces;

    // bounding surfaces should have same length as input list of surface IDs
    // so from the start just reserve that many spaces in memory
    boundingSurfaces.reserve(surfaceIds.size());


    // -------- ITERATE OVER INPUT SURFACE LIST -------- //
    
    for (IntVec::const_iterator it = surfaceIds.begin();
                                it != surfaceIds.end(); ++it) {
        unsigned int userSurfaceId; //an integer that only the user deals with
        SurfaceAndSense newSurface;

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

        // the value from the find result is the internal index
        newSurface.first = _surfaces[getSurfaceIndexFromUserId(userSurfaceId)];

        // add the surface to the vector of bounding surfaces
        boundingSurfaces.push_back(newSurface);

        // this counts as another unmatched surface
        _unMatchedSurfaces++;
//        cout << "Added surface " << userSurfaceId << " to cell " << userCellId
//             << " (unmatched surface count: " << _unMatchedSurfaces << ")"
//             << endl;
    }

    Check(surfaceIds.size() == boundingSurfaces.size());

    //====== add cell to the internal cell vector
    unsigned int newCellIndex = _cells.size();

    CellT* newCell = new CellT(boundingSurfaces, userCellId, newCellIndex,
                               flags);
    _cells.push_back(newCell);

//    cout << "Added cell with ID " << userCellId
//         << "that has index " << newCellIndex << endl;

    // add the reverse mapping to go from User ID to index and 
    // verify that this surface has not been added already (i.e. the
    // UserSurfaceIDType is unique)
    std::pair<CellRevIDMap::iterator, bool> result
        = _cellRevUserIds.insert(
                std::make_pair(userCellId, newCellIndex));

    Insist(result.second == true,
            "Tried to add a cell with an ID that was already there.");

    //====== loop back through the surfaces and add the connectivity
    for (CellT::SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                       bsIt != boundingSurfaces.end(); ++bsIt)
    {
        SurfaceAndSense newQandS = *bsIt;

        if (flags & CellT::NEGATED) {
            // cell is inside out, so reverse the sense of the surface
            // with respect to how it connects to other cells
            newQandS.second = !(newQandS.second);
        }

        // using the "associative array" capability of maps lets us access a
        // key, and it will either automatically initialize an empty vector or
        // return the vector that is already  
        // see C++ Standard Library pp. 182-183
        _surfToCellConnectivity[newQandS].push_back(newCell);
    }

    return newCellIndex;
}
/*----------------------------------------------------------------------------*/
void MCGeometry::completedGeometryInput()
{
    // resize all vectors so that no space is wasted

    // prevent new cells/surfaces from being added?

    // make sure SCConnectMap is not empty for anything?

    cout << "This doesn't do anything yet." << endl;
}
/*----------------------------------------------------------------------------*/
//! get a user ID internal index  from a cell index
MCGeometry::UserCellIDType MCGeometry::getUserIdFromCellIndex(
                const unsigned int index) const
{
    Require( index < getNumCells() );

    return _cells[index]->getUserId();
}

/*----------------------------------------------------------------------------*/
//! get a user ID internal index  from a surface index
MCGeometry::UserSurfaceIDType MCGeometry::getUserIdFromSurfaceIndex(
                const unsigned int index) const
{
    Require( index < getNumSurfaces() );

    return _surfaces[index]->getUserId();
}

/*============================================================================*\
 * other internal-use code
\*============================================================================*/
void MCGeometry::_completedConnectivity()
{
    cout << "<<CONNECTIVITY IS COMPLETE>>" << endl;
}
/*----------------------------------------------------------------------------*/
void MCGeometry::_failGeometry(const std::string failureMessage,
                               const unsigned int currentCellIndex,
                               const std::vector<double>& position,
                               const std::vector<double>& direction) const
{
    cout << "ERROR IN GEOMETRY: " << failureMessage << endl;
    
    cout << "Current cell index [" << currentCellIndex << "] ";

    Require(currentCellIndex < getNumCells());

    cout << "user ID [" << _cells[currentCellIndex]->getUserId() << "]"
         << endl;
    cout << "Known cell connectivity: ";

    const SASVec& boundingSurfaces
            = _cells[currentCellIndex]->getBoundingSurfaces();

    for (SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                bsIt != boundingSurfaces.end(); bsIt++)
    {
        _printSAS(*bsIt);
        cout << ":[";

        const CellT::CellContainer& otherCells =
            _cells[currentCellIndex]->getNeighbors(bsIt->first);

        for (CellT::CellContainer::const_iterator
                                     outCelIt = otherCells.begin(); 
                                     outCelIt != otherCells.end();
                                     ++outCelIt)
        {
            cout << (*outCelIt)->getUserId() << " ";
        }
        cout << "] ";
    }


    cout << "Was checking position " << position << " and direction "
         << direction << endl;


    Insist(0, "Geometry failure.");
}

/*----------------------------------------------------------------------------*/
void MCGeometry::debugPrint() const
{
    //-------------- PRINT USER SURFACE IDs ----------------//
    cout << "SURFACES: " << endl;
    for (SurfaceVec::const_iterator surfIt  = _surfaces.begin();
                                    surfIt != _surfaces.end(); ++surfIt)
    {
        cout << " SURFACE " << (*surfIt)->getUserId() << ": "
             << *(*surfIt) << endl;
    }

    //-------------- PRINT CELLS TO SURFACES ----------------//
    cout << "CELLS: " << endl;
    
    for (CellVec::const_iterator cellIt = _cells.begin();
                                 cellIt != _cells.end(); ++cellIt)
    {
        cout << " CELL " << (*cellIt)->getUserId() << ": ";

        // query cell for surface pointers
        const CellT::SASVec& boundingSurfaces =
                        (*cellIt)->getBoundingSurfaces();

        for (CellT::SASVec::const_iterator bsIt  = boundingSurfaces.begin(); 
                                        bsIt != boundingSurfaces.end(); ++bsIt)
        {
            _printSAS(*bsIt);
            cout << " ";
        }

        if ( (*cellIt)->isNegated() )
            cout << " <NEGATED>";

        if ( (*cellIt)->isDeadCell() )
            cout << " <DEADCELL>";

        cout << endl;
    }

    //-------------- PRINT SURFACES TO CELLS ----------------//
    cout << "SURFACES TO CELLS: " << endl;

    for (SCConnectMap::const_iterator
            surfToCellIt = _surfToCellConnectivity.begin();
            surfToCellIt != _surfToCellConnectivity.end();
            ++surfToCellIt)
    {
        // print the surface and orientation
        cout << " ";

        _printSAS(surfToCellIt->first);

        cout << ": ";
        
        // print the cells
        const CellVec& theCells = surfToCellIt->second;

        for (CellVec::const_iterator cIt = theCells.begin();
                                     cIt != theCells.end(); ++cIt)
        {
            cout << (*cIt)->getUserId() << " ";
        }

        cout << endl;
    }

    cout << "NEIGHBORHOOD CONNECTIVITY: " << endl;
    //-------------- PRINT CELLS TO CELLS ----------------//
    for (CellVec::const_iterator cellIt = _cells.begin();
                                 cellIt != _cells.end(); ++cellIt)
    {
        cout << " CELL " << (*cellIt)->getUserId() << ": ";

        // query cell for surface pointers
        const CellT::SASVec& boundingSurfaces
                    = (*cellIt)->getBoundingSurfaces();

        for (CellT::SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                    bsIt != boundingSurfaces.end(); bsIt++)
        {
            _printSAS(*bsIt);
            cout << ":{";

            const CellT::CellContainer& otherCells =
                (*cellIt)->getNeighbors(bsIt->first);

            for (CellT::CellContainer::const_iterator
                                        outCelIt = otherCells.begin(); 
                                        outCelIt != otherCells.end();
                                        ++outCelIt)
            {
                cout << (*outCelIt)->getUserId() << " ";
            }
            cout << "} ";
        }

        cout << endl;
    }  
}
/*----------------------------------------------------------------------------*/
void MCGeometry::_printSAS(const SurfaceAndSense& qas) const
{
    if (qas.second == true)
        cout << "+";
    else
        cout << "-";

    cout << qas.first->getUserId();
}
/*----------------------------------------------------------------------------*/
// creation
MCGeometry::MCGeometry() :
    _unMatchedSurfaces(0)
{
    /* * */
}
/*----------------------------------------------------------------------------*/
// clean up after ourselves
MCGeometry::~MCGeometry()
{
    // map iterators act as a pointer to (key, value) pair
    for (SurfaceVec::iterator surfIt = _surfaces.begin();
                              surfIt != _surfaces.end(); ++surfIt)
    {
        // dereferenced iterator is a pointer to the surface we need to delete
        delete (*surfIt);
    }
    
    for (CellVec::iterator cellIt = _cells.begin();
                           cellIt != _cells.end(); ++cellIt)
    {
        // dereferenced iterator is a pointer to the cell we need to delete
        delete (*cellIt);
    }
}

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
