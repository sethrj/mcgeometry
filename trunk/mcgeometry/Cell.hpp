/*!
 * \file   Cell.hpp
 * \brief  Contains Cell class
 * \author Seth R. Johnson
 */

#ifndef MCG_CELL_HPP
#define MCG_CELL_HPP

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
/*!
 * \class Cell
 * \brief Cell object defined by bounding surface+senses
 *
 * It finds the intersected surface, and it also stores cell-->cell
 * connectivity. Currently it also stores a user ID number, which is only to
 * interface with the user.
 */
template <typename UserIdType>
class Cell {
public:
    //! Stores a pointer to a Surface as well as the "sense" of that quadric
    //  as viewed from this cell
    typedef std::pair<Surface*, bool>          SurfaceAndSense;

    //! A vector of Surface/sense pairs (e.g. for bounding surfaces)
    typedef std::vector<SurfaceAndSense>       SASVec;

    //! A container of pointers to Cells for neighborhood connectivity
//    typedef std::vector< Cell<UserIdType> * >  CellContainer;
    typedef std::list< Cell<UserIdType> * >  CellContainer;

    //! constructor requires an immutable bounding surface
    Cell(   const SASVec& boundingSurfaces,
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

    //! destructor doesn't have to do anything
    ~Cell() {
//        cout << "Oh no, I (cell " << this << ") am dying!" << endl;
        /* * */
    }

    //! get a list of bounding surfaces; maybe should make MCGeometry a friend
    //  instead?
    const SASVec& getBoundingSurfaces() const {
        return _boundingSurfaces;
    }

    //! get a list of known cell neighbors for a quadric
    //  non-const so that whatever we pass can add to it
    //  TODO possibly poor design here?
    //const CellContainer& getNeighbors(const Surface* surface) const {
    CellContainer& getNeighbors(Surface* surface) {
        // I think surface can't be const because findResult returns a pair
        // that has a non-const Surface* in it
        typename HoodMap::iterator findResult = _hood.find(surface);
        Require(findResult != _hood.end());

        return findResult->second;
    }

    //! Return the internal index (needed for turning "neighbor" to index) 
    const unsigned int& getIndex() const {
        return _internalIndex;
    }
    //! Return the user ID 
    const UserIdType& getUserId() const {
        return _userId;
    }

    ////! get a list of known cell neighbors for a quadric
    //void addNeighbor(const Surface*, ) {
    //    return _boundingSurfaces;
    //}

    //! \brief see if our cell contains the point; possibly skip one surface if
    //         testing "next region"
    // do this by comparing the "sense" of each of our surface to what the
    // surface says about the position's sense relative to it
    bool isPointInside(const std::vector<double>& position,
                       const Surface* surfaceToSkip = NULL) const;

    //! find the nearest surface from a point in a given direction
    void intersect( const std::vector<double>& position,
                    const std::vector<double>& direction,
                    Surface*& hitSurface,
                    bool&     quadricSense,
                    double&   distance) const;

private:
    //! map our surfaces to vectors of other cells attached to that surface
    typedef std::map< Surface*, CellContainer > HoodMap;

    //! a vector of surfaces and senses that define the cell
    const SASVec _boundingSurfaces;

    //! the variable that reports to the user
    const UserIdType _userId;

    //! its index in the primary Cell array
    const unsigned int _internalIndex;

    //! connectivity to other cell vectors through surfaces
    HoodMap _hood;
};

/*----------------------------------------------------------------------------*/
template <typename UserIdType>
inline bool Cell<UserIdType>::isPointInside(const std::vector<double>& position,
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
inline void Cell<UserIdType>::intersect(
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
        // call "intersect" on the quadric, also passing our sense 
        // of the quadric (it->second)
        Surface::HitAndDist theResult = 
            it->first->intersect(position, direction, it->second);

        // if this check fails, then two surfaces apparently have exactly the
        // same distance from the particle; i.e. the particle ran into a corner?
        // what happens then?
//        Check(theResult.second != distance);

//        cout << "Test for " << typeid(*(it->first)).name()
//             << ": hit=" << theResult.first << " dist=" << theResult.second
//             << endl;

        if (theResult.first // if it hits, and if it's a smaller distance
                && (theResult.second < distance))
        {
            distance     = theResult.second;
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

