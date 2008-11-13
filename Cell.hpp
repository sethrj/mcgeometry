
#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>
#include <vector>
#include <list>
#include "Surfaces/Quadric.hpp"

/*----------------------------------------------------------------------------*/
class Cell {
public:
    typedef std::pair<Quadric*, bool> QuadricAndSense;

    Cell()  { /* * */ }

    ~Cell() { /* * */ }


private:
    std::vector<QuadricAndSense> _boundingSurfaces;
    std::map<Quadric*, std::list<Cell*> > _hood;
};
/*----------------------------------------------------------------------------*/

#endif

