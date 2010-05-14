/*!
 * \file   Cell.cpp
 * \brief  Contains implementation for \c Cell
 * \author Seth R. Johnson
 */
/*----------------------------------------------------------------------------*/
#include "Cell.hpp"
#include "Surface.hpp"

#include <utility>
#include <limits>

#include <blitz/tinyvec.h>
#include "transupport/dbc.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
//! Static function to make flag generation easier.
Cell::CellFlags Cell::generateFlags(
        bool isDeadCell,
        bool isNegated )
{
    CellFlags flags = NONE;

    if (isDeadCell) flags = static_cast<CellFlags>(flags | DEADCELL);
    if (isNegated)  flags = static_cast<CellFlags>(flags | NEGATED);

    return flags;
}

/*============================================================================*/
//! constructor requires an immutable bounding surface
Cell::Cell(
        const SASVec& boundingSurfaces,
        const UserCellIdType userId,
        const unsigned int internalIndex,
        const CellFlags flags) :
    _boundingSurfaces(boundingSurfaces),
    _userId(userId),
    _internalIndex(internalIndex),
    _flags(flags)
{
    typedef std::pair<HoodMap::iterator, bool> ReturnedPair;
    Require(_boundingSurfaces.size() > 0);

    // initialize hood map
    SASVec::const_iterator bsIt = _boundingSurfaces.begin();

    // create empty neighborhood map, one entry for each surface
    while (bsIt != _boundingSurfaces.end()) {
        ReturnedPair result =
            _hood.insert(std::make_pair(bsIt->first, CellContainer()));

        Insist(result.second == true, "Duplicate surface in this cell.");
        ++bsIt;
    }
}
/*----------------------------------------------------------------------------*/
bool Cell::isPointInside(
        const TVecDbl& position,
        const Surface* surfaceToSkip) const
{
    if (_flags & NEGATED) {
        // the negated flag makes the whole cell greedy:
        //  ANYWHERE that disagrees with one of the specified faces is
        //  considered part of the negated cell.
        for (SASVec::const_iterator it  = _boundingSurfaces.begin();
                                    it != _boundingSurfaces.end(); ++it)
        {
            if (it->first != surfaceToSkip) {
                if ( it->first->hasPosSense(position) != it->second ) {
                    return true;
                }
            }
            else {
                // we already know we're on the negated side!
                return true;
            }
        }
        // we're inside all the surfaces
        return false;
    }
    else
    {
//        cout << "Checking for whether " << position << " is inside cell UID"
//             << _userId << endl;
        // loop over all surfaces
        for (SASVec::const_iterator it  = _boundingSurfaces.begin();
                                    it != _boundingSurfaces.end(); ++it)
        {
            // it->first is the pointer to the Surface object
            // it->second is the surface sense

            // if we need to check it
            if (it->first != surfaceToSkip)
            {
               if ( it->first->hasPosSense(position) != it->second ) {
//                   cout << "Looks like cell UID " << _userId
//                        << " has the WRONG sense wrt "
//                        << "surface UID " << it->first->getUserId()
//                        << endl;
                   // if the surface reports that the sense of the point is
                   // NOT the same sense as we know this cell is defined, then
                   // it is not inside.
                   return false;
               }
//               else {
//                   cout << "Looks like cell UID " << _userId
//                        << " has the correct sense wrt "
//                        << "surface UID " << it->first->getUserId()
//                        << endl;
//               }
            }
//            else {
//                cout << "Skipping surface " << surfaceToSkip->getUserId()
//                     << endl;
//            }
        }
    }
    // we only get to this point if the point has the correct sense wrt every
    // surface that we checked
    return true;
}
/*----------------------------------------------------------------------------*/
void Cell::intersect(
        const TVecDbl& position,
        const TVecDbl& direction,
        Surface*& hitSurface,
        bool&     quadricSense,
        double&   distance) const
{
    hitSurface   = NULL;
    quadricSense = false;
    distance     = std::numeric_limits<double>::infinity();

//    cout << "Doing intersect in cell " << getUserId() << endl;
    // loop over all surfaces
    for (SASVec::const_iterator it =  _boundingSurfaces.begin();
                                it != _boundingSurfaces.end(); ++it)
    {
        bool thisHit;
        double thisDistance;

        // call "intersect" on the quadric, also passing our sense
        // of the quadric (it->second)
        it->first->intersect(position, direction, it->second,
                             thisHit, thisDistance); //(return results)

//        if (thisHit) {
//            cout << "Calculated an intersection (distance " << thisDistance
//                 << ") with " << *(it->first) << endl;
//        } else {
//            cout << "No intersection with " << *(it->first) << endl;
//        }


        if (thisHit) // if it hits, and if it's a smaller distance
        {
            if (thisDistance < distance)
            {
                distance     = thisDistance;
                hitSurface   = it->first;
                quadricSense = it->second;
            }
//            else if (thisDistance == distance) {
//                cout << "@@@@@@@ Two surfaces at the same distance!" << endl;
//                return;
//                hitSurface   = it->first;
//                quadricSense = it->second;
//            }
        }
    }

    // really do this? yes, if intersect is only called if isInsideCell is a
    // certainty
    Ensure(hitSurface != NULL);
    Ensure(distance   != std::numeric_limits<double>::infinity());
}

/*============================================================================*/
} // end namespace mcGeometry

