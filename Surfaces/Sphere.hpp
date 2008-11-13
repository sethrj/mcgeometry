
#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <vector>
#include <cmath>
#include "Surfaces/Quadric.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/dbc.hpp"

class Sphere : public Quadric {
    public:
        Sphere(std::vector<double>& C, double R)
            : _Center(C), _Radius(R), Quadric() 
        { 
            Insist(R > 0, "Sphere must have positive radius.");
        }

        //! create sphere at origin
        Sphere(double R)
            : _Center(3,0.0), _Radius(R), Quadric() 
        { 
            Insist(R > 0, "Sphere must have positive radius.");
        }

        bool hasPosSense(std::vector<double>& position) const;

        HitAndDist intersect(const std::vector<double>& position, 
                const std::vector<double>& direction, bool posSense) const;

        ~Sphere() { /* * */ };
    private:
        const std::vector<double> _Center;
        const double _Radius;
        
};

std::ostream& operator<<( std::ostream& os, Sphere& s){
    os  << "Center: (" << _center[0] << ", " << _center[1] << ", " << _center[1] 
        << "), Radius = " << _radius;
    return os;
}

// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
bool Sphere::hasPosSense(std::vector<double>& position) const{
    double eval = pow(position[0]-_Center[0],2) + pow(position[1]-_Center[1],2)
                 + pow(position[2]-_Center[2],2) - pow(_Radius,2);
    return _hasPosSense(eval);
}

Quadric::HitAndDist Sphere::intersect(const std::vector<double>& position, 
        const std::vector<double>& direction, bool posSense) const
{
    Require(direction.size() == 3);
//	Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));
	Require(position.size() == 3);

	double selfDot = 0.0;
	double B = 0.0;
	double movedLoc;
	
	// subtract out the center of the sphere
	for (int i = 0; i < 3; ++i) {
		movedLoc = (position[i] - _Center[i]);
		B +=  movedLoc * direction[i]; 
		selfDot += movedLoc * movedLoc;
	}

	// find distance and whether it intercepts
	Quadric::HitAndDist theResult =
		_calcQuadraticIntersect(
			1,  // A
			B,  // B
			selfDot - _Radius * _Radius, //C
			posSense);

	return theResult;

    // THIS CODE DOES NOT WORK SADLY:
//
//    // Compute relative coordinates
//    std::vector<double> xr(position);
//
//    tranSupport::vectorMinusEq(xr, _Center);
//
//    // Compute quadratic coefficients
//    double A = 1.0;
//    double B = tranSupport::vectorDot(direction, xr);
//    double C = _Radius*_Radius - tranSupport::vectorDot(xr, xr);
//
//    // Call quadratic solver and return result
//    return calcQuadraticIntersect(A, B, C, posSense);
//    
}

#endif 

