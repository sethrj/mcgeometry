/*!
 * \file   MCGeometry.hpp
 * \brief  Geometry class
 * \author Seth R. Johnson et al.
 *
 * Copyright (c) 2008, Seth R. Johnson, Jeremy L. Conlin
 * All rights reserved.
 */

/* Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of
 *     the MCGeometry project nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific
 *     prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef MCG_MCGEOMETRY_HPP
#define MCG_MCGEOMETRY_HPP
/*----------------------------------------------------------------------------*/

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <blitz/tinyvec.h>

#include "Cell.hpp"
#include "transupport/dbc.hpp"

namespace mcGeometry {
/*============================================================================*/

class Surface;

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
    typedef unsigned int UserSurfaceIdType;

    //! User's cell IDs are unsigned ints for now. (Maybe templated later.)
    typedef unsigned int UserCellIdType;

    //! Blitz++ TinyVector of length D stores position/direction/etc.
    typedef blitz::TinyVector<double, 3> TVecDbl;

    //! Vector of signed integers that are passed in by the user and parsed
    //! into surfaces and sense.
    typedef std::vector<signed int>                 IntVec;

    //! ReturnStatus indicates whether it interacted with a special geometry.
    enum ReturnStatus {
        NORMAL    = 0,  //!< Business as usual in the particle world
        DEADCELL,       //!< New cell is a dead cell
        REFLECTED,      //!< Particle hit a reflecting surface
        LOST            //!< God help us all if this is ever returned!
    };

    /*------------------------------------------------------------*/
    //! \name Geometry setup
    //\{

    /*!
     * \brief Add a new \c Surface to our geometry, with an associated user ID.
     *
     * Return INTERNAL index of the surface (0 to N_sur - 1).
     */
    unsigned int addSurface(const UserSurfaceIdType userSurfaceId,
                            const Surface& newSurface);

    /*!
     * \brief Parse a list of unsigned ints with +/- into surfaces and senses,
     * and add the new cell.
     *
     * Return INTERNAL index of the surface (0 to N_cell - 1).
     */
    unsigned int addCell(const UserCellIdType& userCellId,
                         const IntVec& surfaces,
                         const Cell::CellFlags flags = Cell::NONE);

    //! Do optimization after input is finished, check geometry for duplicate
    //! surfaces, etc.
    void completedGeometryInput();

    //\}
    /*------------------------------------------------------------*/
    //! \name Particle transport
    //\{

    /*!
     * \brief Find the distance to the closest geometry interaction.
     *
     *  Given a current position, location, and cell this also caches the hit
     *  surface so we don't have to re-calculate the intersected surface, etc.
     *  \param[in]  position     Current particle position.
     *  \param[in]  direction    Current particle direction.
     *  \param[in]  oldCellIndex Current particle internal cell index.
     *  \param[out] distance     Distance to intersecting a surface.
     *
     *  This function loops over every surface inside the old cell, and finds
     *  the surface that is the smallest distance away. It caches this closest
     *  surface and distance for later, and returns to the user the closest
     *  distance.
     *
     *  If the geometry is completely defined (i.e. there are no missing gaps
     *  and the outside is properly defined with "dead cells"), it will always
     *  return a finite distance.
     */
    void findDistance(      const TVecDbl& position,
                            const TVecDbl& direction,
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
     *
     *  Using the cached \c oldCellIndex and \c hitSurface from \c
     *  findDistance(),  we find the cell on the other side of the hit surface.
     *
     *  First, we
     *  check to see if the surface we hit is reflecting: if so, no further
     *  work is necessary, and we set the returnStatus to
     *  \c MCGeometry::REFLECTED .
     *
     *  Next, we ask the old cell for the "neighborhood" of cells that it knows
     *  are on the other side of that surface. We loop for each of those,
     *  calling \c Cell::isPointInside(), passing the surface that the
     *  particle just crossed.
     *  \note
     *  We \e never check the surface that a particle
     *  crossed, partly because it requires extra computational effort, and
     *  partly because being exactly on that surface may cause a cell to
     *  wrongly believe the particle is outside it, depending on how the
     *  surface normal is defined.
     *
     *  If the particle is inside one of these cells, we return.
     *
     *  If not, we loop over every cell in \c _surfToCellConnectivity that
     *  corresponds to the hit surface in the opposite surface sense that the
     *  old cell has. (To restate, any cell on the other side of the current
     *  cell \e must have the opposite surface sense of the crossed surface.)
     *  We call \c Cell::isPointInside() on each one of these, and eventually
     *  find the new cell. When this occurs, we \c push_back a pointer to the
     *  new cell onto the old cell's neighborhood list, and we \c push_back a
     *  pointer to the old cell on the new cell's neighborhood list.
     *
     *  Depending on how complex the geometry is, there is an O(epsilon) chance
     *  of hitting a point that is inside a cell which is "indirectly" connected
     *  to the current cell. (See \c examples/trickyGeometry.cpp in the case of
     *  streaming upward into a tangent sphere/plane/cylinder.) In this case,
     *  before declaring a particle lost, we do a global search on the
     *  particle's new position.
     *
     *  If that turns up nothing, then the user's geometry is most certainly
     *  flawed.
     *
     *  This function is ONLY valid after calling findDistance to calculate the
     *  surface crossing in a given transport iteration. The position and
     *  direction are expected to be unchanged between calling findDistance and
     *  findNewCell. Although this is checked when \c DBC is 7, it is not
     *  checked with debug code off.
     */
    void findNewCell(       const TVecDbl& position,
                            const TVecDbl& direction,
                            TVecDbl& newPosition,
                            unsigned int& newCellIndex,
                            ReturnStatus& returnStatus);

    //!\brief Calculate distance to next cell *and* do the next-cell calculation
    //! in one go.
    void findNewCell(       const TVecDbl& position,
                            const TVecDbl& direction,
                            const unsigned int oldCellIndex,
                            TVecDbl& newPosition,
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
     *  This function is ONLY valid after calling findDistance.
     */
    void reflectDirection(  const TVecDbl& newPosition,
                            const TVecDbl& oldDirection,
                            TVecDbl& newDirection);

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
    void getSurfaceCrossing(    const TVecDbl& newPosition,
                                const TVecDbl& oldDirection,
                                UserSurfaceIdType& surfaceCrossingUserId,
                                double&       dotProduct);

    //\}
    /*------------------------------------------------------------*/
    //! \name Problem information
    //\{

    //! Find a cell given an arbitrary point in the problem.
    unsigned int findCell(const TVecDbl& position) const;

    //! See whether a given cell is a dead cell.
    bool isDeadCell(const unsigned int cellIndex) const;

    //! Return the number of cells we have stored.
    unsigned int getNumCells() const {
        return _cells.size();
    }

    //! \brief Return the number of surfaces we have stored.
    //!
    //! This will not necessarily be the number of surfaces that are used.
    unsigned int getNumSurfaces() const {
        return _surfaces.size();
    }

    //! Print a user-readable copy of all our geometry information.
    void debugPrint() const;

    //\}
    /*------------------------------------------------------------*/
    //! \name User ID to internal index translation
    //\{

    //! Get an internal index for a cell from a user ID.
    unsigned int getCellIndexFromUserId(
                    const UserCellIdType userCellId) const;

    //! Get an internal index for a surface from a user ID.
    unsigned int getSurfaceIndexFromUserId(
                 const UserSurfaceIdType userSurfaceId) const;

    //! Get a user ID for a cell from a cell internal index.
    UserCellIdType getUserIdFromCellIndex(
                    const unsigned int index) const;

    //! Get a user ID for a surface from a surface internal index.
    UserSurfaceIdType getUserIdFromSurfaceIndex(
                    const unsigned int index) const;
    //\}
    /*------------------------------------------------------------*/

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
    typedef std::vector< Cell* >                 CellVec;

    //! Connect surface-and-senses to a vector of cells on the other side
    typedef std::map< SurfaceAndSense, CellVec >  SCConnectMap;

    //! Map user surface IDs to surface internal index
    typedef std::map< UserSurfaceIdType, unsigned int >   SurfaceRevIDMap;
    //! Map user cell IDs to cell internal index
    typedef std::map< UserCellIdType, unsigned int >      CellRevIDMap;

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

        IfDbc(TVecDbl position; TVecDbl direction;)

    } _findCache;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    //! After finding two newly connecting cells, update their connections.
    void _updateConnectivity(
                Cell* oldCell,
                Cell* newCell,
                Cell::CellContainer& oldNeighborhood);

    //! Internal mechanism to add a cell based on a list of surface/senses.
    unsigned int _addCell(          const UserCellIdType&  userCellId,
                                    const Cell::SASVec&   boundingSurfaces,
                                    const Cell::CellFlags flags);

    //! Do optimization whenever the last surface is linked.
    void _completedConnectivity();

    //! Print some kind of warning during findNewCell.
    void _warnGeometry(             const std::string& shortMessage,
                                    const TVecDbl& position,
                                    const TVecDbl& direction,
                                    const Cell* oldCell,
                                    const std::string& longMessage) const;

    //! Print geometry failure message.
    void _failGeometry(            const std::string& failureMessage,
                                   const unsigned int currentCellIndex,
                                   const TVecDbl& position,
                                   const TVecDbl& direction) const;
};
/*============================================================================*/
} // end namespace mcGeometry
#endif

