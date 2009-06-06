/*!
 * \file   MCGeometry.i.hpp
 * \brief  Contains inline implementation for the \c MCGeometry class
 * \author Seth R. Johnson
 */

#ifndef MCG_MCGEOMETRY_I_HPP
#define MCG_MCGEOMETRY_I_HPP

#include <utility>
#include <limits>
#include <map>
#include <vector>

#include <blitz/tinyvec-et.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

#include "MCGeometry.hpp"
#include "Surface.hpp"
#include "Cell.hpp"

#include "Cell.i.hpp"

#include <sstream>

namespace mcGeometry {
/*============================================================================*\
 * subroutines that the user calls while running their monte carlo code
\*============================================================================*/
inline void MCGeometry::findDistance(
        const TVecDbl& position,
        const TVecDbl& direction,
        const unsigned int oldCellIndex,
        double& distanceTraveled)
{
    Require(tranSupport::checkDirectionVector(direction));
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
inline void MCGeometry::findNewCell(
        const TVecDbl& position,
        const TVecDbl& direction,
        TVecDbl& newPosition,
        unsigned int& newCellIndex,
        ReturnStatus& returnStatus)
{
//    cout << "unmatched surface count: " << _unMatchedSurfaces << endl;
    Require( blitz::all(position  == _findCache.position) );
    Require( blitz::all(direction == _findCache.direction) );
    Require(_findCache.oldCellIndex < getNumCells());

    // a reference to the pointer to the old cell
    CellT& oldCell = *(_cells[_findCache.oldCellIndex]);

    // ===== calculate transported position on the boundary of our new cell
    //  (necessary for finding which cell the point belongs to)

    // if the distance to the next surface is zero, we may be stuck at a corner!
    // move the particle so that instead of 
    // $$ \vec{x}_\text{new} = \vec{x}_\text{old} + \vec{\Omega} d $$
    // where $d=0$, use
    // $$ d = \| \vec{x} \| 2 \varepsilon_\text{mach} $$
    // so that the position is perturbed in the particle direction by 
    // machine epsilon times the particle's order of magnitude
    //
    // This is a very minor nudge and should happen only EXTREMELY infrequently
    // (i.e. pretty much JUST on fabricated problems)
    //  THIS IS A RARE CASE OF WHAT COULD HAPPEN
    if (_findCache.distanceToSurface == 0.0) {
        _findCache.distanceToSurface
            = tranSupport::vectorNorm(position)
                * 2 * std::numeric_limits<double>::epsilon();
        _findCache.distanceToSurface = std::max(_findCache.distanceToSurface,
                                    std::numeric_limits<double>::epsilon());

        std::ostringstream message;
        message << "crossing surface ID " << _findCache.hitSurface->getUserId()
                << " and adding |dx| = " << _findCache.distanceToSurface;
        _warnGeometry("Bumping the particle", position, direction, &oldCell,
                      message.str());
    }
    // transport the particle
    newPosition = position + _findCache.distanceToSurface * direction;

    // ===== if we're reflecting, just return the reflected status
    if ( _findCache.hitSurface->isReflecting() ) {
        returnStatus = REFLECTED;

        // particle stays in the same cell
        newCellIndex = _findCache.oldCellIndex;

        return;
    }

    returnStatus = NORMAL;
    // ===== Loop over neighborhood cells
    CellT::CellContainer& neighborhood =
            oldCell.getNeighbors(_findCache.hitSurface);


//    // if we've completed the geometry and there is only once cell on the other
//    // side, then we know that it's the right cell
//    if ((_unMatchedSurfaces == 0) && (neighborhood.size() == 1)) {
//        newCellIndex = neighborhood.front()->getIndex();
//
//        if ( neighborhood.front()->isDeadCell() )
//            returnStatus = DEADCELL;
//        return;
//    }

    // loop through the old cell's hood to find if it's in one of those cells
    for (CellT::CellContainer::const_iterator it  = neighborhood.begin();
                                       it != neighborhood.end(); ++it)
    {
//        cout << "Checking if cell UserID " << (*it)->getUserId()
//             << " contains point " << newPosition << endl;
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
//        cout << "Checking if cell UserID " << (*pNewCell)->getUserId()
//             << " contains point " << newPosition << endl;
        // check if the point is inside (and pass _hitSurface to exclude
        // checking it)
        if ( (*pNewCell)->isPointInside( newPosition, _findCache.hitSurface ) )
        {
            // we have found the new cell
            newCellIndex = (*pNewCell)->getIndex();

            _updateConnectivity(&oldCell, *pNewCell, neighborhood);

            if ( (*pNewCell)->isDeadCell() )
                returnStatus = DEADCELL;
            return;
        }
    }

    // after checking cells connected to the surface, check all cells in the
    // problem to make sure it doesn't show up there
    //  THIS IS A RARE CASE OF WHAT COULD HAPPEN
    for (unsigned int i = 0; i < _cells.size(); ++i) {
        if (i != _findCache.oldCellIndex) {
            if (_cells[i]->isPointInside(newPosition, _findCache.hitSurface)) {
                
                std::ostringstream message;
                message << "crossing surface ID "
                        << _findCache.hitSurface->getUserId()
                        << " into new cell index " << _cells[i]->getIndex()
                        << " (user ID " << _cells[i]->getUserId() << ")";

                _warnGeometry("Used global search", position, direction, 
                                &oldCell, message.str());

//                _unMatchedSurfaces++; // do this? or something more complicated?
         
                _updateConnectivity(&oldCell, _cells[i], neighborhood);

                newCellIndex = _cells[i]->getIndex();

                if ( _cells[i]->isDeadCell() )
                    returnStatus = DEADCELL;

                return;
            }
        }
    }


    // TODO: when looping through the other cells in the problem, exclude the
    // ones that we already checked in the cell's hood; means more loops but
    // fewer flops in the "is inside" method calls ??

    // at this point, particle is LOST. We pretend it's in the same cell as it
    // was?
    newCellIndex = _findCache.oldCellIndex;
    returnStatus = LOST;

    _failGeometry("Ruh-roh, new cell not found!",
                    _findCache.oldCellIndex, position, direction);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//     SUBROUTINES USED IN findNewCell MULTIPLE TIMES
inline void MCGeometry::_updateConnectivity(
        CellT* oldCell,
        CellT* newCell,
        CellT::CellContainer& oldNeighborhood)
{
    // if this is the first cell linked to this surface, we decrement 
    // the unmatched surfaces
    if (oldNeighborhood.size() == 0)
        _unMatchedSurfaces--;

    // add new cell to old cell's hood connectivity
    oldNeighborhood.push_back(newCell);

    CellT::CellContainer& newNeighborhood
        = newCell->getNeighbors(_findCache.hitSurface);

    // if this is the first cell linked to this surface, we decrement 
    // the unmatched surfaces
    if (newNeighborhood.size() == 0)
        _unMatchedSurfaces--;

    // add old cell to new cell's hood connectivity
    newNeighborhood.push_back(oldCell);

    //            cout << "Connected ending cell index " << newCellIndex
    //                 << " to starting cell index " << _findCache.oldCellIndex
    //                 << endl;

    if (_unMatchedSurfaces == 0)
        _completedConnectivity();

    Ensure(_unMatchedSurfaces >= 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------*/
inline void MCGeometry::reflectDirection(
        const TVecDbl& newPosition,
        const TVecDbl& oldDirection,
        TVecDbl& newDirection)
{
    Require( blitz::all(oldDirection == _findCache.direction));
    // law of reflection: omega = omega - 2 (n . omega) n
    TVecDbl surfaceNormal;

    // find the surface normal at the point of intersection
    _findCache.hitSurface->normalAtPoint(newPosition, surfaceNormal);

    // returned normal is with respect to the "positive" sense of the
    // surface, so reverse if necessary
    if (_findCache.oldSurfaceSense == false)
    {
        surfaceNormal = -surfaceNormal;
    }

    double doubleProjection 
        = 2 * blitz::dot(oldDirection, surfaceNormal);

    // calculate the new direction
    newDirection = oldDirection - doubleProjection * surfaceNormal;

    Ensure(tranSupport::checkDirectionVector(newDirection));
}
/*----------------------------------------------------------------------------*/
inline void MCGeometry::getSurfaceCrossing(
        const TVecDbl& newPosition,
        const TVecDbl& oldDirection,
        MCGeometry::UserSurfaceIDType& surfaceCrossingUserId,
        double&       dotProduct)
{
    Require(blitz::all(oldDirection == _findCache.direction));
    Require(tranSupport::checkDirectionVector(oldDirection));

    TVecDbl surfaceNormal;

    // find the surface normal at the point of intersection
    _findCache.hitSurface->normalAtPoint(newPosition, surfaceNormal);

    // returned normal is with respect to the "positive" sense of the
    // surface, so reverse if necessary
    if (_findCache.oldSurfaceSense == false)
    {
        surfaceNormal = -surfaceNormal;
    }

    surfaceCrossingUserId = _findCache.hitSurface->getUserId();
    dotProduct = blitz::dot(oldDirection, surfaceNormal);

    Ensure(tranSupport::checkDirectionVector(surfaceNormal));
}
/*----------------------------------------------------------------------------*/
inline unsigned int MCGeometry::findCell(
        const TVecDbl& position) const
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
    _failGeometry("Could not find cell!", 0, position, TVecDbl());
    return 0;
}
/*----------------------------------------------------------------------------*/
inline bool MCGeometry::isDeadCell(const unsigned int cellIndex) const
{
    Require(cellIndex < getNumCells() );
    return ( _cells[cellIndex]->isDeadCell() );
}
/*============================================================================*\
 * other internal-use code
\*============================================================================*/
inline void MCGeometry::_completedConnectivity()
{
    // THIS CODE IS ALMOST ALWAYS VALID
    //  difficulties: transporting across negative-sense surfaces into corners
    //                doing global geometry search

//    cout << "<<CONNECTIVITY IS COMPLETE>>" << endl;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif
