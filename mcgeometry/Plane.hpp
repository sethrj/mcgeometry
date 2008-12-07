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
    //! User-called constructor.
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

    //! Copy the surface with a new user ID.
    Plane(const Plane& oldPlane, const UserSurfaceIdType& newId)
        : Surface(oldPlane, newId),
          _normal(oldPlane._normal),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new Plane(*this, newId);
    }

    ~Plane() { /* * */} 

    bool hasPosSense(const std::vector<double>& position) const;

    void intersect( const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

    void normalAtPoint( const std::vector<double>& position,
                        std::vector<double>& unitNormal) const;
protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    //! Unit normal to the plane for a positive sense.
    const std::vector<double> _normal;
    //! Some coordinate through which the plane passes.
    const std::vector<double> _coordinate;
};

/*----------------------------------------------------------------------------*/
//! calculate distance to intersection
inline void Plane::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    // default to "does not hit" values
    hit = false;
    distance = 0.0;

    double cosine = tranSupport::vectorDot3(_normal, direction);

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
}

/*----------------------------------------------------------------------------*/
// Information taken from http://mathworld.wolfram.com/Plane.html
// Equation: n.p - n.p0     n = normal vector to plane
//      p = position
//      p0 = point on plane
inline bool Plane::hasPosSense(const std::vector<double>& position) const{
    Require(position.size() == 3);
    double eval(0);
    for( int i = 0; i < 3; ++i ){
        eval += _normal[i]*position[i] - _normal[i]*_coordinate[i];
    }

    return _hasPosSense(eval);
}
/*----------------------------------------------------------------------------*/
inline void Plane::normalAtPoint(  const std::vector<double>& position,
                            std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);
    // "position" should be on the surface
    IfDbc(  double eval(0);
            for( int i = 0; i < 3; ++i )
                eval += _normal[i]*position[i] - _normal[i]*_coordinate[i];
         );
    Require(softEquiv(eval, 0.0));
    
    unitNormal = _normal;
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
    //! User-called constructor.
    PlaneNormal( const double coord) : _coordinate(coord)
    { /* * */ }

    //! Copy the surface with a new user ID.
    PlaneNormal(const PlaneNormal& oldPlane, const UserSurfaceIdType& newId)
        : Surface(oldPlane, newId),
          _coordinate(oldPlane._coordinate)
    { /* * */ }

    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new PlaneNormal<axis>(*this, newId);
    }

    ~PlaneNormal() { /* * */} 

    bool hasPosSense(const std::vector<double>& position) const;

    void intersect( const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool posSense,
                    bool& hit,
                    double& distance) const;

    void normalAtPoint( const std::vector<double>& position,
                        std::vector<double>& unitNormal) const;

    //! return the index along which we are oriented
    unsigned int getAxis() const {
        return axis;
    }
protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    //! The coordinate along the axis through which the plane passes.
    const double              _coordinate;
};
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline bool PlaneNormal<axis>::hasPosSense(
                    const std::vector<double>& position) const
{
    Require(position.size() == 3);
    return _hasPosSense(position[axis] - _coordinate);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void PlaneNormal<axis>::intersect(
                    const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool posSense,
                    bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    // default to "does not hit" values
    hit = false;
    distance = 0.0;

    if (    ((posSense == false) && (direction[axis] > 0))
         || ((posSense == true)  && (direction[axis] < 0)) )
    {
        // Headed towards surface
        hit = true;
        distance = (_coordinate - position[axis]) / direction[axis];
        distance = std::max(0.0, distance);
    }
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void PlaneNormal<axis>::normalAtPoint( const std::vector<double>& position,
                                       std::vector<double>& unitNormal) const
{
    // "position" should be on the surface
    Require(position.size() == 3);
    Require(softEquiv(position[axis] - _coordinate, 0.0));

    unitNormal.assign(3,0.0);
    unitNormal[axis] = 1.0;
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

