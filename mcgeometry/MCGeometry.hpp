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

class Quadric;
class Cell;

/*----------------------------------------------------------------------------*/
class MCGeometry {
public:
    typedef std::vector<signed int> IntVec;

    //! ReturnStatus indicates whether it interacted with a special geometry
    enum ReturnStatus {
        MCG_DEADCELL  = 0,
        MCG_REFLECTED = 1,
        MCG_LOST      = 2  // God help us all if 2 is ever returned!
    };

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

    //! Debug printing; will be incorporated into file IO etc.
    void debugPrint() const;

    //! empty constructor
    MCGeometry()
    { /* * */ }
    
    //! destructor must delete surfaces and cells
    ~MCGeometry();


private:
    typedef std::pair<Quadric*, bool>               QuadricAndSense;
    typedef std::vector<QuadricAndSense>            QASVec;
    typedef std::vector<Cell*>                      CellPVec;

    typedef std::map< unsigned int, Quadric* >              SurfaceMap;
    typedef std::map< unsigned int, Cell* >                 CellMap;
    typedef std::map< QuadricAndSense, CellPVec > SCConnectMap;


    SurfaceMap _surfaces;
    CellMap    _cells;

    //! a surface 
    // surface --> cell connectivity
    SCConnectMap _surfToCellConnectivity;

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
#endif

