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

namespace mcGeometry {

class Surface;

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
    //! User's surface IDs are unsigned ints for now. (Maybe templated later.)
    typedef unsigned int UserSurfaceIDType;

    //! User's cell IDs are unsigned ints for now. (Maybe templated later.)
    typedef unsigned int UserCellIDType;

    //! We only use one kind of templated cell.
    typedef Cell<UserCellIDType> CellT;

    //! ReturnStatus indicates whether it interacted with a special geometry.
    enum ReturnStatus {
        NORMAL    = 0,
        DEADCELL,
        REFLECTED,
        LOST  // God help us all if this is ever returned!
    };

    //! Vector of signed integers that are passed in by the user and parsed
    //! into surfaces and sense.
    typedef std::vector<signed int>                 IntVec;

    /*!
     * \brief Add a new \c Surface to our geometry, with an associated user ID.
     *
     * Return INTERNAL index of the surface (0 to N_sur - 1).
     */
    unsigned int addSurface(const UserSurfaceIDType userSurfaceId,
                            const Surface& newSurface);

    /*!
     * \brief Parse a list of unsigned ints with +/- into surfaces and senses,
     * and add the new cell.
     *
     * Return INTERNAL index of the surface (0 to N_cell - 1).
     */
    unsigned int addCell(const UserCellIDType& userCellId,
                         const IntVec& surfaces,
                         const CellT::CellFlags flags = CellT::NONE);

    //! Do optimization after input is finished, check geometry for duplicate
    //! surfaces, etc.
    void completedGeometryInput();

    /*!
     * \brief Find the distance to the closest geometry interaction.
     *
     *  Given a current position, location, and cell this also caches the hit
     *  surface so we don't have to re-calculate the intersected surface, etc.
     *  \param[in]  position     Current particle position.
     *  \param[in]  direction    Current particle direction.
     *  \param[in]  oldCellIndex Current particle internal cell index.
     *  \param[out] distance     Distance to intersecting a surface.
     */
    void findDistance(      const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            double& distance);

    /*!
     * \brief Go ahead and find the next cell after finding the distance.
     *
     *  \param[in]  position     Current particle position.
     *  \param[in]  direction    Current particle direction.
     *  \param[out] newPosition  Particle's new position at the surface.
     *  \param[out] newCellIndex Internal cell index of the new cell.
     *  \param[out] returnStatus Extra information about the transport.
     */
    void findNewCell(       const std::vector<double>& position,
                            const std::vector<double>& direction,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            ReturnStatus& returnStatus);

    // we may have to add further code to pass back a surface ID for a surface
    // tally, for example

    //!\brief Calculate distance to next cell *and* do the next-cell calculation
    //! in one go.
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
    
     /*!
     * \brief If we found that the surface was reflecting, change the direction.
     *
     *
     *  \param[in]  newPosition  Particle position after transporting
     *  \param[in]  oldDirection Particle direction before reflecting
     *  \param[out] newDirection Direction after reflecting
     *
     */
    void reflectDirection(  const std::vector<double>& newPosition,
                            const std::vector<double>& oldDirection,
                            std::vector<double>& newDirection);

    /*!
     * \brief Return information about a crossed surface.
     *
     *
     *  \param[in]  newPosition  Particle position after transporting
     *  \param[in]  oldDirection Particle direction
     *  \param[out] surfaceCrossingUserId User ID of the surface that was
     *                      crossed
     *  \param[out] dotProduct   Omega dot n (use for surface current tally)
     *
     */
    void getSurfaceCrossing(    const std::vector<double>& newPosition,
                                const std::vector<double>& oldDirection,
                                UserSurfaceIDType& surfaceCrossingUserId,
                                double&       dotProduct);

    //! Find a cell given an arbitrary point in the problem.
    unsigned int findCell(const std::vector<double>& position) const;

    //! See whether a given cell is a dead cell.
    bool isDeadCell(const unsigned int cellIndex) const;

    //! Print a user-readable copy of all our geometry information.
    void debugPrint() const;

    //! Return the number of cells we have stored.
    unsigned int getNumCells() const {
        return _cells.size();
    }

    //! Returns the number of surfaces we have stored.
    //!
    //! This will not necessarily be the number of surfaces that are used.
    unsigned int getNumSurfaces() const {
        return _surfaces.size();
    }

    // ======= user ID to internal index translation ======= //

    //! Get an internal index for a cell from a user ID.
    unsigned int getCellIndexFromUserId(
                    const UserCellIDType userCellId) const;

    //! Get an internal index for a surface from a user ID.
    unsigned int getSurfaceIndexFromUserId(
                 const UserSurfaceIDType userSurfaceId) const;

    //! Get a user ID for a cell from a cell internal index.
    UserCellIDType getUserIdFromCellIndex(
                    const unsigned int index) const;

    //! Get a user ID for a surface from a surface internal index.
    UserSurfaceIDType getUserIdFromSurfaceIndex(
                    const unsigned int index) const;

    //! Constructor.
    MCGeometry();
    
    //! Destructor must delete actual surfaces and cell memory.
    ~MCGeometry();

private:
    //! A \c Surface and sense (true = positive, false = negative)
    typedef std::pair<Surface*, bool>             SurfaceAndSense;
    //! Vector that defines cell bounding surfaces.
    typedef std::vector<SurfaceAndSense>          SASVec;

    //! Vector of pointers to surfaces
    typedef std::vector< Surface* >               SurfaceVec;
    //! Vector of pointers to cells
    typedef std::vector< CellT* >                 CellVec;

    //! Connect surface-and-senses to a vector of cells on the other side
    typedef std::map< SurfaceAndSense, CellVec >  SCConnectMap;

    //! Map user surface IDs to surface internal index
    typedef std::map< UserSurfaceIDType, unsigned int >   SurfaceRevIDMap;
    //! Map user cell IDs to cell internal index
    typedef std::map< UserCellIDType, unsigned int >      CellRevIDMap;

    //====== INTERNAL ASSOCIATIVE VECTORS AND MAPS ======//
    // These use pointers and unsigned index values that handle
    // all the internal associativity and cell/surface definition
    // for the entire problem.
    // They therefore need to be as efficient as possible.

    //! All of the Surface s in the problem; indexed by our internal
    //! representation of them
    SurfaceVec _surfaces;

    //! All of the Cell s in the problem; indexed by our internal
    //! representation of them
    CellVec    _cells;

    //! What cells connect to a surface with a particular sense.
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

    /*! \brief Keep track of surface neighborhood connectivity.
     *
     *  When this reaches zero, connectivity is complete.
     *  Unmatched surfaces are from the cell's point of view, i.e. surfaces may
     *  be and probably will be double-counted.
     */
    int _unMatchedSurfaces;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    //! Cache for storing intersection information from findDistance
    struct {
        unsigned int    oldCellIndex;
        Surface*        hitSurface;
        bool            oldSurfaceSense;
        double          distanceToSurface;

        IfDbc(std::vector<double> position; std::vector<double> direction;)

    } _findCache;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    //! After finding two newly connecting cells, update their connections.
    void _updateConnectivity(
                CellT* oldCell,
                CellT* newCell,
                CellT::CellContainer& oldNeighborhood);

    //! Internal mechanism to add a cell based on a list of surface/senses.
    unsigned int _addCell(          const UserCellIDType&  userCellId,
                                    const CellT::SASVec&   boundingSurfaces,
                                    const CellT::CellFlags flags);

    //! Do optimization whenever the last surface is linked.
    void _completedConnectivity();

    //! Print some kind of warning during findNewCell.
    void _warnGeometry(             const std::string& shortMessage,
                                    const std::vector<double>& position,
                                    const std::vector<double>& direction,
                                    const CellT* oldCell,
                                    const std::string& longMessage) const;

    //! Print geometry failure message.
    void _failGeometry(            const std::string failureMessage,
                                   const unsigned int currentCellIndex,
                                   const std::vector<double>& position,
                                   const std::vector<double>& direction) const;
};
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

