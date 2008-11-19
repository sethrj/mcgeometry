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

#include "transupport/VectorMath.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Surface.hpp"
#include "Cell.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
/*!
 * \class MCGeometry
 * \brief Umbrella class that coordinates the cells in a problem
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
        MCG_NORMAL    = 0,
        MCG_DEADCELL,
        MCG_REFLECTED,
        MCG_LOST  // God help us all if this is ever returned!
    };

    typedef std::vector<signed int>                 IntVec;

    //! user creates an arbitrary quadric surface and passes it in with ID
    // return INTERNAL index of the surface (0 to N_sur - 1)
    unsigned int addSurface(const UserSurfaceIDType userSurfaceId,
                            const Surface& newSurface);

    //! user passes in a vector of surface IDs with +/- 
    // return INTERNAL index of the surface (0 to N_cell - 1) 
    unsigned int addCell(const UserCellIDType userCellId,
                         const IntVec surfaces);

    //! do optimization after input is finished, check geometry for duplicate
    //surfaces, etc.
    void completedGeometryInput();


    //!\brief  given a current position, location, and cell; find the new one
    // we may have to add further code to pass back a surface ID for a surface
    // tally, for example
    void intersect(         const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            double& distanceTraveled,
                            ReturnStatus& returnStatus);

    //! find a cell given a point
    unsigned int findCell(const std::vector<double>& position) const;

    //TODO: define IMP=0 cells, reflecting surfaces, etc.

    //! Debug printing; will be incorporated into file IO etc. later
    void debugPrint() const;

    //! empty constructor
    MCGeometry() :
        _unMatchedSurfaces(0)
    { /* * */ }
    
    //! destructor must delete surfaces and cells
    ~MCGeometry();

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

    //! get an internal index for a surface from a user ID
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
                    const unsigned int index) const
    {
        Require( index < getNumCells() );

        return _cells[index]->getUserId();
    }

    //! get a user ID internal index  from a surface index
    UserSurfaceIDType getUserIdFromSurfaceIndex(
                    const unsigned int index) const
    {
        Require( index < getNumSurfaces() );

        return _surfaces[index]->getUserId();
    }

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

    //====== INTERNAL ASSOCIATIVE VECTORS AND MAPS ======//
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

    //! print a surface and its corresponding sense
    void _printSAS(const SurfaceAndSense& qas) const;

    //! do optimization whenever the last surface is linked
    void _completedConnectivity();

    //! print geometry failure message
    void _failGeometry(            const std::string failureMessage,
                                   const unsigned int currentCellIndex,
                                   const std::vector<double>& position,
                                   const std::vector<double>& direction);
};
/*----------------------------------------------------------------------------*/
inline void MCGeometry::intersect(
                            const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellIndex,
                            std::vector<double>& newPosition,
                            unsigned int& newCellIndex,
                            double& distanceTraveled,
                            ReturnStatus& returnStatus)
{
//    cout << "unmatched surface count: " << _unMatchedSurfaces << endl;

    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    Require(oldCellIndex < getNumCells());

    // a reference to the pointer to the old cell
    CellT& oldCell = *(_cells[oldCellIndex]);

    // call intersect on the old cell to find the surface and distance that it
    // moves to
    Surface* hitSurface;
    bool     oldSurfaceSense;
    oldCell.intersect(position, direction, hitSurface, oldSurfaceSense,
                      distanceTraveled);

    Check(hitSurface != NULL);
    Check(distanceTraveled >= 0.0);

    returnStatus = MCG_NORMAL;

    // ===== calculate transported position on the boundary of our new cell
    newPosition.resize(3);
    for (int i = 0; i < 3; i++) {
        newPosition[i] = position[i] + distanceTraveled * direction[i];
    }


    // ===== Loop over neighborhood cells
    CellT::CellContainer& neighborhood = oldCell.getNeighbors(hitSurface);

    // loop through the old cell's hood to find if it's in one of those cells
    for (CellT::CellContainer::const_iterator it  = neighborhood.begin();
                                       it != neighborhood.end(); ++it)
    {
        // check if the point is inside (and pass hitSurface to exclude
        // checking it)
        if ( (*it)->isPointInside( newPosition, hitSurface ) )
        {
            //we have found the new cell
            newCellIndex = (*it)->getIndex();

//            cout << "Found ending cell index " << newCellIndex
//                 << " already connected to starting cell index "
//                 << oldCellIndex << " through hood" << endl;

            return;
        }
    }

    // ===== Loop through all cells that have the same surface as the one we
    // intersected, and the opposite surface sense (i.e. if in our cell we hit
    // surface 2, and our cell is defined as being -2, then the cell on the
    // other side has to have orientation +2

    SurfaceAndSense searchQas(hitSurface, !oldSurfaceSense);

    SCConnectMap::iterator cellList
                 = _surfToCellConnectivity.find(searchQas);

    if (cellList == _surfToCellConnectivity.end()) {
        _failGeometry("Surface connectivity not found for surface",
                        newCellIndex, position, direction);
    }

    CellVec& cellsToCheck = cellList->second;

    for (CellVec::iterator pNewCell  = cellsToCheck.begin();
                                  pNewCell != cellsToCheck.end(); ++pNewCell)
    {
        // check if the point is inside (and pass hitSurface to exclude
        // checking it)
        if ( (*pNewCell)->isPointInside( newPosition, hitSurface ) )
        {
            // if this is the first cell linked to this surface, we decrement 
            // the unmatched surfaces
            if (neighborhood.size() == 0)
                _unMatchedSurfaces--;

            // add new cell to old cell's hood connectivity
            neighborhood.push_back(*pNewCell);

            CellT::CellContainer& newNeighborhood
                = (*pNewCell)->getNeighbors(hitSurface);

            // if this is the first cell linked to this surface, we decrement 
            // the unmatched surfaces
            if (newNeighborhood.size() == 0)
                _unMatchedSurfaces--;

            // add old cell to new cell's hood connectivity
            newNeighborhood.push_back(&oldCell);

            // we have found the new cell
            newCellIndex = (*pNewCell)->getIndex();

//            cout << "Connected ending cell index " << newCellIndex
//                 << " to starting cell index " << oldCellIndex << endl;

            if (_unMatchedSurfaces == 0)
                _completedConnectivity();
            return;
        }
    }

    // TODO: when looping through the other cells in the problem, exclude the
    // ones that we already checked in the cell's hood; means more loops but
    // fewer flops in the "is inside" method calls

    newCellIndex = -1;
    returnStatus = MCG_LOST;

    _failGeometry("Ruh-roh, new cell not found!",
                    newCellIndex, position, direction);
}
/*----------------------------------------------------------------------------*/
inline unsigned int MCGeometry::findCell(
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
    Insist(0, "Could not find cell!");
    return 0;
}
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

