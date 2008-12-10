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
 * \brief Cell object defined by bounding Surface/sense pairs.
 *
 * It finds the intersected surface, and it also stores cell-->cell
 * connectivity. Currently it also stores a user ID number, which is only to
 * interface with the user.
 *
 * Using the NEGATED flag is useful for creating a dead cell on the outside of
 * a complex surface. (For example, for a cube with complex geometry inside
 * and nothing outside, it is easier to make the "outside" cell the inverse of
 * everything inside a cube rather than six separate dead regions defined by
 * planes.)
 */
template <typename UserIdType>
class Cell {
public:
    //! bit flags with details about the cells
    //  use bitwise 'and' & to check for their being set
    //  use bitwise 'or' | to set several of them at once 
    enum CellFlags {
        NONE     = 0u, //!< No special cell attributes
        DEADCELL = 1u, //!< Particles should be killed when entering us
        NEGATED  = 2u  //!< We are "everything but" what is inside our bounds
            //further flags should be powers of 2
    };

    //! Stores a pointer to a Surface as well as the "sense" of that quadric
    //! as viewed from this cell
    typedef std::pair<Surface*, bool>          SurfaceAndSense;

    //! A vector of Surface/sense pairs (e.g. for bounding surfaces)
    typedef std::vector<SurfaceAndSense>       SASVec;

    //! A container of pointers to Cells for neighborhood connectivity.
    //! A std::list is used because this will be continually modified, and the
    //! cost of reallocation is high.
    typedef std::list< Cell<UserIdType> * >    CellContainer;
    /*******************************/
    //! Static function to make flag generation easier.
    static CellFlags generateFlags(
            bool isDeadCell = false, bool isNegated = false );

    /*******************************/
    /*! Constructor takes a list of bounding surface/senses, and also requires
     * a user ID to store along with MCGeometry's internal index. Finally, it
     * uses CellFlags to determine whether it is dead, negated, etc.
     */
    Cell(   const SASVec& boundingSurfaces,
            const UserIdType userId,
            const unsigned int internalIndex,
            const CellFlags flags = NONE);

    //! The destructor doesn't have to do anything.
    ~Cell() {
        /* * */
    }

    //! Get our list of bounding surfaces.
    const SASVec& getBoundingSurfaces() const {
        return _boundingSurfaces;
    }

    //! Get a writeable list of known cell neighbors for a quadric.
    //  non-const so that whatever we pass can add to it
    //  TODO possibly poor design here?
    CellContainer& getNeighbors(Surface* surface) {
//        typename HoodMap::iterator findResult = _hood.find(surface);
//        Require(findResult != _hood.end());
//
//        return findResult->second;
        // this may create new surface connections
        return _hood[surface];
    }

    //! Return the internal index (needed for turning "neighbor" to index).
    const unsigned int& getIndex() const {
        return _internalIndex;
    }
    //! Return the user ID.
    const UserIdType& getUserId() const {
        return _userId;
    }

    /*! \brief See if our cell contains the point.
     *
     * Optionally skip checking Surface::isPointInside() on one surface if we
     * know we've crossed it.
     * We check by comparing the "sense" of each of our surface to what the
     * surface says about the position's sense relative to it.
     */
    bool isPointInside(const std::vector<double>& position,
                       const Surface* surfaceToSkip = NULL) const;

    /*! \brief Find the nearest surface to a point in a given direction.
     *
     * \param[in] position    The position of the particle
     * \param[in] direction   The direction of the particle
     * \param[out] hitSurface A pointer to the nearest surface
     * \param[out] hitSense   The surface sense of the intersected surface
     * \param[out] distance   The distance to the nearest surface
     */
    void intersect( const std::vector<double>& position,
                    const std::vector<double>& direction,
                    Surface*& hitSurface,
                    bool&     hitSense,
                    double&   distance) const;

    //! Return whether we are a dead cell.
    bool isDeadCell() const {
        return (_flags & DEADCELL);
    }

    //! Return whether we are negated (i.e. inside out).
    bool isNegated() const {
        return (_flags & NEGATED);
    }
private:
    //! Map our surfaces to vectors of other cells attached to that surface.
    typedef std::map< Surface*, CellContainer > HoodMap;

    //! A vector of surfaces and senses that define this Cell.
    const SASVec _boundingSurfaces;

    //! The object that we report to the user.
    const UserIdType _userId;

    //! This Cell's index in the primary MCGeometry CellT* array.
    const unsigned int _internalIndex;

    //! Other information about this cell.
    const CellFlags _flags;

    //! Connectivity to other cells through each surface.
    HoodMap _hood;
};
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

