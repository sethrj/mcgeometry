

#ifndef PLANE_HPP
#define PLANE_HPP

#include <vector>
#include <algorithm>
#include "Surfaces/Quadric.hpp"
#include "transupport/VectoMath.hpp"

class Plane : public Quadric {
    public:
        Plane(std::vector<double> N, std::vector<double> C, unsigned int id) 
            : _normal(N), _coordinate(C) {   }

        ~Plane();

        bool hasPosSense(std::vector<double>& position) const;

        HitAndDist intersect(const std::vector<double>& Position, 
                const std::vector<double>& Direction, bool PosSense) const;

    private:
        std::vector<double> _normal;
        std::vector<double> _coordinate;    // Coordinate of point in plane
};

std::ostream& operator<<( std::ostream& os, Sphere& s){
    os  << "Normal vector: <" << _normal[0] << ", " << _normal[1] << ", " 
        << _normal[2] << ">, Point: (" << _coordinate[0] << ", " 
        << _coordinate[1] << ", " << _coordinate[2] << ")";
    return os;
}

Quadric::HitAndDist Plane::intersect(const std::vector<double>& position, 
        const std::vector<double>& direction, bool posSense) const{

    bool hit;
    double distance(0.0);

    double cosine = tranSupport.vectorDot(_normal, direction);
    if( (posSense and cosine < 0) or (posSense and cosine > 0 )){
        // Headed towards surface
        hit = true;
        for( int i = 0; i < 3; ++i ){
            distance += _normal[i]*(_coordinate - position)/cosine;
        }
        distance = max(0.0, distance);
    }
    else{   // Headed away from surface, no collision
        hit = false;
        distance = 0.0;
    }

    return std::make_pair(hit, distance);
}

// Information taken from http://mathworld.wolfram.com/Plane.html
// Equation: n.p - n.p0     n = normal vector to plane
//      p = position
//      p0 = point on plane
bool Plane::hasPosSense(std::vector<double>& position) const{
    double eval(0);
    for( int i = 0; i < 3; ++i ){
        eval += _normal[i]*position[i] - _Normal[i]*_coordinate[i];
    }

    return _hasPosSense(eval);
}

#endif 


