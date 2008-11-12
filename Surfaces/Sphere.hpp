
#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <vector>
#include "Surfaces/Quadric.hpp"
#include "transupport/VectorMath.hpp"

class Sphere : public Quadric {
    public:
        Sphere(std::vector<double>& C, double R)
            : _Center(C), _Radius(R), Quadric()   {   }

        //! Sphere at origin
        Sphere(double R)
            : _Center(3,0.0), _Radius(R), Quadric()   {   }

        ~Sphere();

        HitAndDist intersect(const std::vector<double>& Position, 
                const std::vector<double>& Direction, bool PosSense) const;

    private:
        const std::vector<double> _Center;
        const double _Radius;
        
};

HitAndDist Sphere::intersect(const std::vector<double>& Position, 
        const std::vector<double>& Direction, bool PosSense){

    // Compute relative coordinates
    std::vector<double> xr(Position)
    transSupport::vectorMinusEq(xr, _Center);

    // Compute quadratic coefficients
    double A = 1.0;
    double B = transSupport::vectorDot(Direction, xr);
    double C = _Radius*_Radius - transSupport::vectorDot(xr, xr);

    // Call quadratic solver and return result
    return intersect(A, B, C, PosSense);
    
}
#endif 

