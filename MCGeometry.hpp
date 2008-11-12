/*!
 * \file MCGeometry.hpp
 * \brief Geometry class: self-sufficient
 * \author Seth R. Johnson
 * 
 */

#ifndef MCGEOMETRY_HPP
#define MCGEOMETRY_HPP

#include <iostream>
#include <utility>
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


    private:
        std::map<unsigned int, Quadric*> _Surfaces;
        std::map<unsigned int, Cell*>    _Cells;
};
/*----------------------------------------------------------------------------*/

inline bool MCGeometry::addSurface( const unsigned int surfaceId,
                                    const Quadric& newQuadric)
{
    // make sure surfaceId is not already taken
    // allocate new quadric memory and 
    Quadric* newQuadric = new Quadric;
}
#endif

