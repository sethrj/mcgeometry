
#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>
#include <utility>
#include "Quadric.hpp"

using std::cout;
using std::endl;

class Cell {
    public:
        typedef std::pair<Quadric*, bool> QuadricSense;

        Cell()  {   }

        ~Cell() {   }


    private:
        std::vector<QuadricSense> _BoundingSurfaces;
};

#endif

