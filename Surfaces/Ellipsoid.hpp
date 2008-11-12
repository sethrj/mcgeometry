

#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include <vector>
#include "Quadric.hpp"

class Ellipsoid : public Quadric {
    public:
        Ellipsoid() {   }

        ~Ellipsoid();

        HitAndDist intersect(const std::vector<double>& Position, 
                const std::vector<double>& Direction, bool PosSense) const;

    private:
        
};

#endif 

