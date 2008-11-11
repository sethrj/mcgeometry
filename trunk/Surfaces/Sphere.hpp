
#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <vector>
#include "Quadric.hpp"

class Sphere : public Quadric {
    public:
        Sphere(std::vector<double>& C, double R)
            : _Center(C), _Radius(R)   {   }

        //! Sphere at origin
        Sphere(double R)
            : _Center(3,0.0), _Radius(R)   {   }

        ~Sphere();

        HitAndDist intersect(std::vector<double>& Position, 
                std::vector<double& Direction, bool PosSense);

    private:
        const std::vector<double> _Center;
        const double _Radius;
        
};

#endif 

