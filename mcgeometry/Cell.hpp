/*!
 * \file   Cell.hpp
 * \brief  Contains interface for Cell class
 * \author Seth R. Johnson
 */

#ifndef MCG_CELL_HPP
#define MCG_CELL_HPP

#include <vector>
#include <list>
#include <map>
#include <utility>
#include "transupport/dbc.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/

class Surface;
    
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
    //! bit flags with details about the cells
    //  use bitwise 'and' & to check for their being set
    //  use bitwise 'or' | to set several of them at once 
    enum CellFlags {
        NONE     = 0u,
        DEADCELL = 1u,
        NEGATED  = 2u
            //further flags should be powers of 2
    };

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
            const unsigned int internalIndex,
            const CellFlags flags = NONE);

    //! destructor doesn't have to do anything
    ~Cell() {
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

    //! return if we are a dead cell
    bool isDeadCell() const {
        return (_flags & DEADCELL);
    }

    //! return if we are negated
    bool isNegated() const {
        return (_flags & NEGATED);
    }
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

    //! other information about this cell
    CellFlags _flags;
};
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

