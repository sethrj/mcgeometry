/*!
 * \file MCGeometry.hpp
 * \brief Geometry class
 * \author Seth R. Johnson
 * 
 */

#ifndef MCGEOMETRY_HPP
#define MCGEOMETRY_HPP

#include <map>
#include "Surfaces/Quadric.hpp"
#include "Cell.hpp"

/*----------------------------------------------------------------------------*/
class MCGeometry {
public:
    typedef std::vector<signed int> IntVec;

    //! user creates an arbitrary quadric surface and passes it in with ID
    // return success
    bool addSurface(const unsigned int surfaceId,
                    const Quadric& newQuadric);

    //! user passes in a vector of surface IDs with +/- 
    // return success
    bool addCell(const unsigned int cellId,
                 const IntVec surfaces);


    //! empty constructor
    MCGeometry()
    { /* * */ }
    
    //! destructor must delete surfaces and cells
    ~MCGeometry();


private:
    typedef const Quadric CQuadric;
    typedef const Cell    CCell;

    typedef std::map<unsigned int, CQuadric*> SurfaceMap;
    typedef std::map<unsigned int, CCell*>    CellMap;

    SurfaceMap _Surfaces;
    CellMap    _Cells;
};
/*----------------------------------------------------------------------------*/
#endif

