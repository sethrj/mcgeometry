/*!
 * \file MCGeometry.hpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 */

#ifndef MCG_MCGEOMETRY_HPP
#define MCG_MCGEOMETRY_HPP

#include <map>
#include <vector>
#include <utility>
#include <string>

#include "Cell.hpp"
#include "Surface.hpp"

namespace mcGeometry {

/*----------------------------------------------------------------------------*/
/*!
 * \class MCGeometry
 * \brief MCGeometry is an umbrella class that coordinates the cells in a 
 *        problem.
 *
 * The MCGeometry parent class handles almost all the external associations
 * between Surfaces and Cells. It maps the user's IDs to internal pointers
 * for both surfaces and cells, and maps surface to Cell connectivity.
 *
 * MCGeometry has two effective points of interface. The first is to the user's
 * ID values, which may be arbitrary integers or strings or anything. These id
 * values would be evaluated *only* when inputing and outputing data to and
 * from the user.
 *
 * The second is its internal indexing system, from 0 to N-1 (where N is the
 * number of cells or surfaces, depending). This will be used throughout
 * the program that links to MCGeometry, e.g. linking cross sections to cells
 * by using this index and a vector. This allows the geometry to be completely
 * isolated from the rest of the program, although it will translate the user
 * IDs to an internal index.
 *
 * The Cell and Surface objects each store their own copy of the user ID, and
 * to translate from a user ID to an internal index, MCGeometry stores a map
 * for the surfaces and cells.
 *
 */
class MCGeometry {
public:
    //! user's surface IDs are unsigned ints for now
    typedef unsigned int UserSurfaceIDType;
    //! user's cell IDs are unsigned ints for now
    typedef unsigned int UserCellIDType;

    typedef Cell<UserCellIDType> CellT;

    //! ReturnStatus indicates whether it interacted with a special geometry
    enum ReturnStatus {
        NORMAL    = 0,
        DEADCELL,
        REFLECTED,
        LOST  // God help us all if this is ever returned!
    };

    typedef std::vector<signed int>                 IntVec;

    //! user creates an arbitrary quadric surface and passes it in with ID
    //  return INTERNAL index of the surface (0 to N_sur - 1)
    unsigned int addSurface(const UserSurfaceIDType userSurfaceId,
                            const Surface& newSurface);

    //! Parse a list of unsigned ints with +/- into surfaces and senses,
    //  and add the new cell.
    //  Return INTERNAL index of the surface (0 to N_cell - 1) 
    unsigned int addCell(const UserCellIDType& userCellId,
                         const IntVec& surfaces,
                         const CellT::CellFlags flags = CellT::NONE);

    //! do optimization after input is finished, check geometry for duplicate
    //  surfaces, etc.
    void completedGeometryInput();

    //!\brief Find the distance to the next geometry interaction.
    // Given a current position, location, and cell
    // this also caches the hit surface so we don't have to re-calculate
    // intersect
    void findDistance(      const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            double& distanceTraveled);

    //!\brief Go ahead and find the next cell after finding the distance.
    // we may have to add further code to pass back a surface ID for a surface
    // tally, for example
    void findNewCell(       const std::vector<double>& position,
                            const std::vector<double>& direction,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            ReturnStatus& returnStatus);


    //!\brief Calculate distance to next cell *and* do the next-cell calculation
    // in one go.
    void findNewCell(       const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            double& distanceTraveled,
                            ReturnStatus& returnStatus)
    {
        findDistance(position, direction, oldCellIndex, distanceTraveled);
        findNewCell(position, direction, newPosition,
                    newCellIndex, returnStatus);
    }
    
    //! If we found that the surface was reflecting, change the direction
    void reflectDirection(  const std::vector<double>& newPosition,
                            const std::vector<double>& oldDirection,
                            std::vector<double>& newDirection);

    //! Find a cell given an arbitrary point in the problem.
    unsigned int findCell(const std::vector<double>& position) const;

    //! Debug printing; will be incorporated into file IO etc. later
    void debugPrint() const;

    //! return the number of cells we have stored
    unsigned int getNumCells() const {
        return _cells.size();
    }

    //! returns the number of surfaces we have stored
    // (will not necessarily be the number of surfaces that are used)
    unsigned int getNumSurfaces() const {
        return _surfaces.size();
    }

    // ======= user ID to internal index translation ======= //

    //! get an internal index for a cell from a user ID
    unsigned int getCellIndexFromUserId(
                    const UserCellIDType userCellId) const
    {
        // translate the user's given surface ID to a Surface pointer
        CellRevIDMap::const_iterator findCMResult = 
            _cellRevUserIds.find(userCellId);

        if (findCMResult == _cellRevUserIds.end()) {
            Insist(0,
            "FATAL ERROR: cell user ID does not exist.");
        }
        return findCMResult->second;
    }

    //! Get an internal index for a surface from a user ID.
    unsigned int getSurfaceIndexFromUserId(
                 const UserSurfaceIDType userSurfaceId) const
    {
        // translate the user's given surface ID to a Surface pointer
        SurfaceRevIDMap::const_iterator findSMResult = 
            _surfaceRevUserIds.find(userSurfaceId);

        if (findSMResult == _surfaceRevUserIds.end()) {
            Insist(0,
            "FATAL ERROR: surface user ID does not exist.");
        }
        return findSMResult->second;
    }

    //! get a user ID internal index  from a cell index
    UserCellIDType getUserIdFromCellIndex(
                    const unsigned int index) const;

    //! get a user ID internal index  from a surface index
    UserSurfaceIDType getUserIdFromSurfaceIndex(
                    const unsigned int index) const;

    //! constructor
    MCGeometry();
    
    //! destructor must delete surfaces and cells
    ~MCGeometry();

private:
    typedef std::pair<Surface*, bool>             SurfaceAndSense;
    typedef std::vector<SurfaceAndSense>          SASVec;

    typedef std::vector< Surface* >               SurfaceVec;
    typedef std::vector< CellT* >                 CellVec;

    typedef std::map< SurfaceAndSense, CellVec >  SCConnectMap;

    typedef std::map< UserSurfaceIDType, unsigned int >   SurfaceRevIDMap;
    typedef std::map< UserCellIDType, unsigned int >      CellRevIDMap;

    //====== INTERNAL ASSOCIATIVE VECTORS AND MAPS ======//
    // These use pointers and unsigned index values that handle
    // all the internal associativity and cell/surface definition
    // for the entire problem.
    // They therefore need to be as efficient as possible.

    //! all of the Surface surfaces in the problem
    //  indexed by our internal representation of them
    SurfaceVec _surfaces;

    //! all of the Cell s in the problem
    //  indexed by our internal representation of them
    CellVec    _cells;

    //! +-surface --> cell connectivity
    SCConnectMap _surfToCellConnectivity;

    //======     USER ASSOCIATIVE MAPS     ======//
    // These associate the user input (i.e. cell IDs and surface IDs)
    // to our internal index values. This is used ONLY when the user inputs
    // geometry information and wants a reprint of said information.
    // They therefore do not need to have huge efficiency in mind.

    //! translate user IDs to internal cell indices
    CellRevIDMap _cellRevUserIds;

    //! translate user IDs to internal surface indices
    SurfaceRevIDMap _surfaceRevUserIds;

    /* NOTE re: SCConnectMap:
     * using a pair as a key value should be legit, because when a pair
     * searches by testing for "less than" or "greater than" it tests the first
     * member, then the second; so for all comparisons except for the surface
     * sense it tests only a memory reference (integer) and on the last
     * comparison it tests for the boolean sense. SO MAKE SURE QUADRIC AND
     * SENSE HAS QUADRIC DEFINED AS THE FIRST PART OF THE PAIR.
     */

    //! keep track of surface neighborhood connectivity
    //  when this reaches zero, connectivity is complete
    //  unmatched surfaces are from cell's point of view, i.e. surfaces may be
    //  and probably will be double-counted
    unsigned int _unMatchedSurfaces;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    // cache for storing intersect information from findDistance
    struct {
        unsigned int    oldCellIndex;
        Surface*        hitSurface;
        bool            oldSurfaceSense;
        double          distanceToSurface;

        IfDbc(std::vector<double> position; std::vector<double> direction;)

    } _findCache;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    //! internal mechanism to add a cell based on a list of surface/senses
    unsigned int _addCell(          const UserCellIDType&  userCellId,
                                    const CellT::SASVec&   boundingSurfaces,
                                    const CellT::CellFlags flags);

    //! do optimization whenever the last surface is linked
    void _completedConnectivity();

    //! print geometry failure message
    void _failGeometry(            const std::string failureMessage,
                                   const unsigned int currentCellIndex,
                                   const std::vector<double>& position,
                                   const std::vector<double>& direction) const;
};
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

