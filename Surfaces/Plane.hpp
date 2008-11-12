

#ifndef PLANE_HPP
#define PLANE_HPP

#include <vector>
#include "Quadric.hpp"

class Plane : public Quadric {
    public:
        Plane(std::vector<double> N, std::vector<double> C, unsigned int id) 
            : _Normal(N), _Coordinate(C) {   }

        ~Plane();

        HitAndDist intersect(const std::vector<double>& Position, 
                const std::vector<double>& Direction, bool PosSense) const;

    private:
        std::vector<double> _Normal;
        std::vector<double> _Coordinate;    // Coordinate of point in plane
};

#endif 


