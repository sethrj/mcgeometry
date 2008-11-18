/*!
 * \file Plane.hpp
 * \brief Plane surface
 * \author Seth R. Johnson
 * A regular Plane, and PlaneNormal to an axis, templated on that axis.
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

#include "Surface.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class Plane
 * \brief General plane
 */
class Plane : public Surface {
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

    Plane(const Plane& oldPlane, const UserSurfaceIdType& newId)
        : Surface(newId),
          _normal(oldPlane._normal),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    Plane* clone(const UserSurfaceIdType& newId) const {
        return new Plane(*this, newId);
    }

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
Surface::HitAndDist Plane::intersect(
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
/*!
 * \class PlaneNormal
 * \brief Plane normal to some surface, templated on the coordinate axis
 *
 * 0 = X axis
 * 1 = Y axis
 * 2 = Z axis
 */
template<unsigned int axis>
class PlaneNormal : public Surface {
public:
    PlaneNormal( const double coord) : _coordinate(coord)
    { /* * */ }

    PlaneNormal(const PlaneNormal& oldPlane, const UserSurfaceIdType& newId)
        : Surface(newId),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    PlaneNormal* clone(const UserSurfaceIdType& newId) const {
        return new PlaneNormal<axis>(*this, newId);
    }

    ~PlaneNormal() { /* * */} 

    bool hasPosSense(const std::vector<double>& position) const;

    HitAndDist intersect(
            const std::vector<double>& Position, 
            const std::vector<double>& Direction,
            bool PosSense) const;

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const double              _coordinate;    // Coordinate of plane along axis
};
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
bool PlaneNormal<axis>::hasPosSense(const std::vector<double>& position) const
{
    Require(position.size() == 3);
    return _hasPosSense(position[axis] - _coordinate);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
Surface::HitAndDist PlaneNormal<axis>::intersect(
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

    if (    ((posSense == false) && (direction[axis] > 0))
         || ((posSense == true)  && (direction[axis] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[axis]) / direction[axis];
        distance = std::max(0.0, distance);
    }

    return std::make_pair(hit, distance);
}

/*----------------------------------------------------------------------------*/
// template specialize 0, 1, and 2
// but don't provide generic so that the compiler will fail if axis > 2
template<>
inline std::ostream& PlaneNormal<0>::_output( std::ostream& os ) const {
    os  << "[ PLANE X  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
inline std::ostream& PlaneNormal<1>::_output( std::ostream& os ) const {
    os  << "[ PLANE Y  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}

template<>
inline std::ostream& PlaneNormal<2>::_output( std::ostream& os ) const {
    os  << "[ PLANE Z  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/

//! provide typedefs for user interaction
typedef PlaneNormal<0> PlaneX;
//! provide typedefs for user interaction
typedef PlaneNormal<1> PlaneY;
//! provide typedefs for user interaction
typedef PlaneNormal<2> PlaneZ;

/*============================================================================*/
} // end namespace mcGeometry
#endif 

