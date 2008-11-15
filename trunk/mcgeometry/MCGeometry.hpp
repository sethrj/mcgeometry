/*!
 * \file MCGeometry.hpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 */

#ifndef MCGEOMETRY_HPP
#define MCGEOMETRY_HPP

#include <map>
#include <vector>
#include <utility>

#include "transupport/VectorMath.hpp"
#include "transupport/SoftEquiv.hpp"

#include "Quadric.hpp"
#include "Cell.hpp"

/*----------------------------------------------------------------------------*/
class MCGeometry {
public:
    //! ReturnStatus indicates whether it interacted with a special geometry
    enum ReturnStatus {
        MCG_NORMAL    = 0,
        MCG_DEADCELL,
        MCG_REFLECTED,
        MCG_LOST  // God help us all if this is ever returned!
    };

    typedef std::vector<signed int>                 IntVec;

    //! user creates an arbitrary quadric surface and passes it in with ID
    // return success
    void addSurface(const unsigned int surfaceId,
                    const Quadric& newQuadric);

    //! user passes in a vector of surface IDs with +/- 
    // return success
    void addCell(const unsigned int cellId,
                 const IntVec surfaces);

    //! do optimization after input is finished
    // void completedGeometryInput();




    //! given a current position, location, and cell; find the new one
    // we may have to add further code to pass back a surface ID for a surface
    // tally, for example
    void intersect(         const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellId,
                            std::vector<double>& newPosition,
                            unsigned int& newCellId,
                            double& distanceTraveled,
                            ReturnStatus& returnStatus);

    //TODO: define IMP=0 cells, reflecting surfaces, etc.

    //! Debug printing; will be incorporated into file IO etc.
    void debugPrint() const;

    //! empty constructor
    MCGeometry() :
        _unMatchedSurfaces(0)
    { /* * */ }
    
    //! destructor must delete surfaces and cells
    ~MCGeometry();


private:
    typedef std::pair<Quadric*, bool>             QuadricAndSense;
    typedef std::vector<QuadricAndSense>          QASVec;
    typedef std::vector<Cell*>                    CellVec;

    typedef std::map< unsigned int, Quadric* >    SurfaceMap;
    typedef std::map< unsigned int, Cell* >       CellMap;
    typedef std::map< QuadricAndSense, CellVec >  SCConnectMap;


    //! map user's surface IDs to Quadric pointers
    SurfaceMap _surfaces;
    //! map user's cell IDs to Cell pointers
    CellMap    _cells;
    //! surface --> cell connectivity
    SCConnectMap _surfToCellConnectivity;

    //! keep track of surface neighborhood connectivity
    //  unmatched surfaces are from cell's point of view, i.e. surfaces may be
    //  and probably will be double-counted
    unsigned int _unMatchedSurfaces;

    /* NOTE: using a pair as a key value should be legit, because when a pair
     * searches by testing for "less than" or "greater than" it tests the first
     * member, then the second; so for all comparisons except for the surface
     * sense it tests only a memory reference (integer) and on the last
     * comparison it tests for the boolean sense. SO MAKE SURE QUADRIC AND
     * SENSE HAS QUADRIC DEFINED AS THE FIRST PART OF THE PAIR.
     */

    //typedef std::pair< std::vector<Cell*>, std::vector<Cell*> > PmCellVec;
    //std::map<Quadric*, PmCellVec> _surfToCellConnectivity;

    // different (probably inferior) ways to do this might have been:
    // std::map<Quadric*, std::vector< std::vector<Cell*> > > ???
    // std::map<signed int, std::vector<Cell*> > ??? Maybe not since that will
    // not be portable (if anything other than ints is used; also the
    // conversion will not be efficient)

    void _printQAS(const QuadricAndSense& qas) const;
};
/*----------------------------------------------------------------------------*/
inline void MCGeometry::intersect(
                            const std::vector<double>& position,
                            const std::vector<double>& direction,
                            const unsigned int oldCellId,
                            std::vector<double>& newPosition,
                            unsigned int& newCellId,
                            double& distanceTraveled,
                            ReturnStatus& returnStatus)
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    CellMap::iterator findResult = _cells.find(oldCellId);
    Require(findResult != _cells.end());

    // a reference to the pointer to the old cell
    Cell& oldCell = *(findResult->second);

    // call intersect on the old cell to find the surface and distance that it
    // moves to
    Quadric* hitQuadric;
    bool     oldQuadricSense;
    oldCell.intersect(position, direction, hitQuadric, oldQuadricSense,
                      distanceTraveled);

    Check(hitQuadric != NULL);

    returnStatus = MCG_NORMAL;

    // ===== calculate transported position on the boundary of our new cell
    newPosition.resize(3);
    for (int i = 0; i < 3; i++) {
        newPosition[i] = position[i] + distanceTraveled * direction[i];
    }


    // ===== Loop over neighborhood cells
    Cell::CellVec& neighborhood = oldCell.getNeighbors(hitQuadric);

    // loop through the old cell's hood to find if it's in one of those cells
    for (Cell::CellVec::const_iterator it  = neighborhood.begin();
                                       it != neighborhood.end(); ++it)
    {
        // check if the point is inside (and pass hitQuadric to exclude
        // checking it)
        if ( (*it)->isPointInside( newPosition, hitQuadric ) )
        {
            //we have found the new cell
            newCellId = (*it)->getUserId();
            return;
        }
    }

    // ===== Loop through all cells that have the same surface as the one we
    // intersected, and the opposite surface sense (i.e. if in our cell we hit
    // surface 2, and our cell is defined as being -2, then the cell on the
    // other side has to have orientation +2

    QuadricAndSense searchQas(hitQuadric, !oldQuadricSense);

    SCConnectMap::iterator cellList
                 = _surfToCellConnectivity.find(searchQas);
    Check(cellList != _surfToCellConnectivity.end());

    CellVec& cellsToCheck = cellList->second;

    for (Cell::CellVec::iterator pNewCell  = cellsToCheck.begin();
                                 pNewCell != cellsToCheck.end(); ++pNewCell)
    {
        // check if the point is inside (and pass hitQuadric to exclude
        // checking it)
        if ( (*pNewCell)->isPointInside( newPosition, hitQuadric ) )
        {
            // add new cell to old cell's hood connectivity
            neighborhood.push_back(*pNewCell);

            // add old cell to new cell's hood connectivity
            (*pNewCell)->getNeighbors(hitQuadric).push_back(&oldCell);

            // the surface was matched from two sides, so decrement the
            // unmatched count by two
            _unMatchedSurfaces -= 2;

            // we have found the new cell
            newCellId = (*pNewCell)->getUserId();
            return;
        }
    }

    // TODO: when looping through the other cells in the problem, exclude the
    // ones that we already checked in the cell's hood; means more loops but
    // fewer flops in the "is inside" method calls

    newCellId = 0;
    returnStatus = MCG_LOST;
    Insist(0, "Ruh-roh, new cell not found!");
}
/*----------------------------------------------------------------------------*/
#endif

