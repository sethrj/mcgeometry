/*!
 * \file Plane.hpp
 * \brief 
 * \author Jeremy L. Conlin
 * 
 */

#ifndef MCG_PLANE_HPP
#define MCG_PLANE_HPP

#include <vector>
#include <algorithm>
#include <iostream>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

#include "Quadric.hpp"

namespace mcGeometry {
/*============================================================================*/
class Plane : public Quadric {
public:
    Plane( const std::vector<double>& normal,
           const std::vector<double>& coord) 
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

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const std::vector<double> _normal;
    const std::vector<double> _coordinate;    // Coordinate of point in plane
};

/*----------------------------------------------------------------------------*/
//! calculate distance to intersection
Quadric::HitAndDist Plane::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction,
        bool posSense) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    // default to "does not hit" values
    bool hit = false;
    double distance = 0.0;

    double cosine = tranSupport::vectorDot(_normal, direction);

    if ( ((posSense == false) && (cosine > 0))
         || ((posSense == true) and (cosine < 0)) )
    {
        // Headed towards surface and hits it
        hit = true;
        for( int i = 0; i < 3; ++i ) {
            distance += _normal[i]*(_coordinate[i] - position[i]);
        }
        distance = std::max(0.0, distance/cosine);
    }

    return std::make_pair(hit, distance);
}

/*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/
inline std::ostream& Plane::_output( std::ostream& os ) const {
    os  << "[ PLANE  Point:  " << std::setw(10) << _coordinate 
        << " Normal vector: " << std::setw(10) << _normal << " ]";
    return os;
}
/*============================================================================*/
//! A plane normal to the X axis
class PlaneX : public Quadric {
public:
    PlaneX( const double coord) : _coordinate(coord)
    { /* * */ }

    PlaneX* clone() const { return new PlaneX(*this); }

    ~PlaneX() { /* * */} 

    bool hasPosSense(const std::vector<double>& position) const;

    HitAndDist intersect(
            const std::vector<double>& Position, 
            const std::vector<double>& Direction,
            bool PosSense) const;

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const double              _coordinate;    // Coordinate of point in plane
};
/*----------------------------------------------------------------------------*/
bool PlaneX::hasPosSense(const std::vector<double>& position) const
{
    // 0 is index for X coordinate
    return _hasPosSense(position[0]);
}
/*----------------------------------------------------------------------------*/
Quadric::HitAndDist PlaneX::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                bool posSense) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    // default to "does not hit" values
    bool hit = false;
    double distance = 0.0;

    // 0 is index for X coordinate
    if (    ((posSense == false) && (direction[0] > 0))
         || ((posSense == true)  && (direction[0] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[0]) / direction[0];
        distance = std::max(0.0, distance);
    }

    return std::make_pair(hit, distance);
}
/*----------------------------------------------------------------------------*/
inline std::ostream& PlaneX::_output( std::ostream& os ) const {
    os  << "[ PLANE X  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif 

