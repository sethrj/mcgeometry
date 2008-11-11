

#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include <vector>
#include "Quadric.hpp"

class Ellipsoid : public Quadric {
    public:
        Ellipsoid();

        ~Ellipsoid();

        HitAndDist intersect(std::vector<double>& Position, 
                std::vector<double& Direction, bool PosSense);

    private:
        
};

#endif 


