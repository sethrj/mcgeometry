
#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>
#include <vector>
#include "Quadric.hpp"
#include "transupport/dbc.hpp"

/*----------------------------------------------------------------------------*/
class Cell {
public:
    typedef std::pair<Quadric*, bool> QuadricAndSense;

    Cell(const std::vector<QuadricAndSense>& boundingSurfaces) 
        : _boundingSurfaces(boundingSurfaces)
    {
        Require(_boundingSurfaces.size() > 0);
    }

    ~Cell() { /* * */ }


private:
    const std::vector<QuadricAndSense> _boundingSurfaces;
    std::map<Quadric*, std::vector<Cell*> > _hood;
};
/*----------------------------------------------------------------------------*/

#endif

