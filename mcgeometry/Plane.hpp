/*!
 * \file Plane.hpp
 * \brief Plane surface
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
 * \class PlaneX
 * \brief Plane normal to the X axis
 */
class PlaneX : public Surface {
public:
    PlaneX( const double coord) : _coordinate(coord)
    { /* * */ }

    PlaneX(const PlaneX& oldPlane, const UserSurfaceIdType& newId)
        : Surface(newId),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    PlaneX* clone(const UserSurfaceIdType& newId) const {
        return new PlaneX(*this, newId);
    }

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
    return _hasPosSense(position[0] - _coordinate);
}
/*----------------------------------------------------------------------------*/
Surface::HitAndDist PlaneX::intersect(
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
/*!
 * \class PlaneY
 * \brief Plane normal to the Y axis
 */
class PlaneY : public Surface {
public:
    PlaneY( const double coord) : _coordinate(coord)
    { /* * */ }

    PlaneY(const PlaneY& oldPlane, const UserSurfaceIdType& newId)
        : Surface(newId),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    PlaneY* clone(const UserSurfaceIdType& newId) const {
        return new PlaneY(*this, newId);
    }

    ~PlaneY() { /* * */} 

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
bool PlaneY::hasPosSense(const std::vector<double>& position) const
{
    // 0 is index for Y coordinate
    return _hasPosSense(position[1] - _coordinate);
}
/*----------------------------------------------------------------------------*/
Surface::HitAndDist PlaneY::intersect(
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

    // 0 is index for Y coordinate
    if (    ((posSense == false) && (direction[1] > 0))
         || ((posSense == true)  && (direction[1] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[1]) / direction[1];
        distance = std::max(0.0, distance);
    }

    return std::make_pair(hit, distance);
}
/*----------------------------------------------------------------------------*/
inline std::ostream& PlaneY::_output( std::ostream& os ) const {
    os  << "[ PLANE Y  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/
/*!
 * \class PlaneZ
 * \brief Plane normal to the Z axis
 */
class PlaneZ : public Surface {
public:
    PlaneZ( const double coord) : _coordinate(coord)
    { /* * */ }

    PlaneZ(const PlaneZ& oldPlane, const UserSurfaceIdType& newId)
        : Surface(newId),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    PlaneZ* clone(const UserSurfaceIdType& newId) const {
        return new PlaneZ(*this, newId);
    }

    ~PlaneZ() { /* * */} 

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
bool PlaneZ::hasPosSense(const std::vector<double>& position) const
{
    // 0 is index for Z coordinate
    return _hasPosSense(position[2] - _coordinate);
}
/*----------------------------------------------------------------------------*/
Surface::HitAndDist PlaneZ::intersect(
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

    // 0 is index for Z coordinate
    if (    ((posSense == false) && (direction[2] > 0))
         || ((posSense == true)  && (direction[2] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[2]) / direction[2];
        distance = std::max(0.0, distance);
    }

    return std::make_pair(hit, distance);
}
/*----------------------------------------------------------------------------*/
inline std::ostream& PlaneZ::_output( std::ostream& os ) const {
    os  << "[ PLANE Z  Point:  " << std::setw(10) << _coordinate 
        << " ]";
    return os;
}
/*============================================================================*/
} // end namespace mcGeometry
#endif 

