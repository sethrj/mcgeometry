/*!
 * \file   Cell.t.hpp
 * \brief  Contains templated methods for Cell
 * \author Seth R. Johnson
 */

#ifndef MCG_CELL_T_HPP
#define MCG_CELL_T_HPP

#include <vector>
#include <list>
#include <map>
#include <utility>
#include <limits>
#include "Surface.hpp"
#include "transupport/dbc.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
//! constructor requires an immutable bounding surface
template <typename UserIdType>
Cell<UserIdType>::Cell(   const SASVec& boundingSurfaces,
        const UserIdType userId,
        const unsigned int internalIndex ) 
    : _boundingSurfaces(boundingSurfaces),
      _userId(userId),
      _internalIndex(internalIndex)
{
    typedef std::pair<typename HoodMap::iterator, bool> ReturnedPair;
    Require(_boundingSurfaces.size() > 0);

    // initialize hood map
    SASVec::const_iterator bsIt = _boundingSurfaces.begin();

    while (bsIt != _boundingSurfaces.end()) {
        ReturnedPair result = 
            _hood.insert(std::make_pair(bsIt->first, CellContainer()));

        Insist(result.second == true, "Duplicate surface in this cell.");
        ++bsIt;
    }
}
/*----------------------------------------------------------------------------*/
template <typename UserIdType>
bool Cell<UserIdType>::isPointInside(const std::vector<double>& position,
                                            const Surface* surfaceToSkip) const
{
    // loop over all surfaces
    for (SASVec::const_iterator it = _boundingSurfaces.begin();
                                      it != _boundingSurfaces.end(); ++it)
    {
        // it->first is the pointer to the Surface object
        // it->second is the surface sense
        
        // if we need to check it
        if (it->first != surfaceToSkip)
        {
           if ( it->first->hasPosSense(position) != it->second ) {
               // if the surface reports that the sense of the point is 
               // NOT the same sense as we know this cell is defined, then
               // it is not inside.
               return false;
           }
        }
    }
    // we only get to this point if the point has the correct sense wrt every
    // surface that we checked
    return true;
}
/*----------------------------------------------------------------------------*/
template <typename UserIdType>
void Cell<UserIdType>::intersect(
                    const std::vector<double>& position,
                    const std::vector<double>& direction,
                    Surface*& hitSurface,
                    bool&     quadricSense,
                    double&   distance) const
{
    //since we only *pass* these variables to the lower level and don't actually
    //base anything on their properties in this function; let the lower level
    //handle the input checking instead of being redundant by including it here
    //and at MCGeometry
//    Require(position.size() == 3);
//    Require(direction.size() == 3);

    hitSurface   = NULL;
    quadricSense = false;
    distance     = std::numeric_limits<double>::infinity();

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

        // if this check fails, then two surfaces apparently have exactly the
        // same distance from the particle; i.e. the particle ran into a corner?
        // what happens then?
//        Check(theResult.second != distance);

//        cout << "Test for " << typeid(*(it->first)).name()
//             << ": hit=" << thisHit << " dist=" << thisDistance
//             << endl;

        if (thisHit // if it hits, and if it's a smaller distance
                && (thisDistance < distance))
        {
            distance     = thisDistance;
            hitSurface   = it->first;
            quadricSense = it->second;
        }
    }

    // really do this? yes, if intersect is only called if isInsideCell is a
    // certainty
    Ensure(hitSurface != NULL);
    Ensure(distance   != std::numeric_limits<double>::infinity());
}

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif
