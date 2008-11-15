

#ifndef PLANE_HPP
#define PLANE_HPP

#include <vector>
#include <algorithm>
#include <iostream>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

#include "Quadric.hpp"

class Plane : public Quadric {
    friend std::ostream& operator<<( std::ostream& , const Plane& );
public:
    Plane( std::vector<double>& normal, std::vector<double>& coord) 
                        : _normal(normal), _coordinate(coord)
    {
        // we only live in 3-space
        Require(_normal.size() == 3);
        Require(_coordinate.size() == 3);
        // require unit normal 
        Require(softEquiv(tranSupport::vectorNorm(_normal), 1.0));
    }

    Plane* clone() const { return new Plane(*this); }

    ~Plane() { /* * */} 

    bool hasPosSense(const std::vector<double>& position) const;

    HitAndDist intersect(const std::vector<double>& Position, 
            const std::vector<double>& Direction, bool PosSense) const;

private:
    const std::vector<double> _normal;
    const std::vector<double> _coordinate;    // Coordinate of point in plane
};

/*----------------------------------------------------------------------------*/
//! calculate distance to intersection
Quadric::HitAndDist Plane::intersect(const std::vector<double>& position, 
        const std::vector<double>& direction, bool posSense) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    bool hit;
    double distance(0.0);

    double cosine = tranSupport::vectorDot(_normal, direction);
    if( ((posSense == false) && (cosine > 0))
         || ((posSense == true) and (cosine < 0)) )
    {
        // Headed towards surface
        hit = true;
        for( int i = 0; i < 3; ++i ) {
            distance += _normal[i]*(_coordinate[i] - position[i]);
        }
        distance = std::max(0.0, distance/cosine);
    }
    else
    {   // Headed away from surface, no collision
        hit = false;
        distance = 0.0;
    }

    return std::make_pair(hit, distance);
}

// Information taken from http://mathworld.wolfram.com/Plane.html
// Equation: n.p - n.p0     n = normal vector to plane
//      p = position
//      p0 = point on plane
bool Plane::hasPosSense(const std::vector<double>& position) const{
    double eval(0);
    for( int i = 0; i < 3; ++i ){
        eval += _normal[i]*position[i] - _normal[i]*_coordinate[i];
    }

    return _hasPosSense(eval);
}


inline std::ostream& operator<<( std::ostream& os, const Plane& p) {
    os  << "[ PLANE  Point:  " << std::setw(10) << p._coordinate 
        << " Normal vector: " << std::setw(10) << p._normal << " ]";
    return os;
}
#endif 


