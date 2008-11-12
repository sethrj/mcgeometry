

#ifndef PLANE_HPP
#define PLANE_HPP

#include <vector>
#include "Quadric.hpp"

class Plane : public Quadric {
    public:
        Plane(std::vector<double> N, std::vector<double> C, unsigned int id) 
            : _Normal(N), _Coordinate(C), Quadric(id) {   }

        ~Plane();

        HitAndDist intersect(std::vector<double>& Position, 
                std::vector<double& Direction, bool PosSense);

    private:
        std::vector<double> _Normal;
        std::vector<double> _Coordinate;    // Coordinate of point in plane
};

#endif 


