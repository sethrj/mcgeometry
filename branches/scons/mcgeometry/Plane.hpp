/*!
 * \file   Plane.hpp
 * \brief  Plane surfaces
 * \author Seth R. Johnson
 * A regular Plane, and PlaneNormal to an axis, templated on that axis.
 */

#ifndef MCG_PLANE_HPP
#define MCG_PLANE_HPP

#include <iostream>
#include <blitz/tinyvec.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

#include "Surface.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class Plane
 * \brief General plane
 */
class Plane : public Surface {
public:
    //! Blitz++ TinyVector of length D stores position/direction/etc.
    typedef blitz::TinyVector<double, 3> TVecDbl; 
    
public:
    //! User-called constructor.
    Plane( const TVecDbl& normal,
           const TVecDbl& coord) 
                                    : _normal(normal), _coordinate(coord)
    {
        // require unit normal 
        Require(tranSupport::checkDirectionVector(_normal));
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

    bool hasPosSense(const TVecDbl& position) const;

    void intersect( const TVecDbl& position, 
                    const TVecDbl& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

    void normalAtPoint( const TVecDbl& position,
                        TVecDbl& unitNormal) const;
protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    //! Unit normal to the plane for a positive sense.
    const TVecDbl _normal;
    //! Some coordinate through which the plane passes.
    const TVecDbl _coordinate;
};
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

    //! Calculate whether a point has a positive sense to this surface
    bool hasPosSense(const TVecDbl& position) const;

    //! Determine distance to intersection with the surface.  
    void intersect( const TVecDbl& position, 
                    const TVecDbl& direction,
                    const bool posSense,
                    bool& hit,
                    double& distance) const;

    //! Calculate the surface normal at a point
    void normalAtPoint( const TVecDbl& position,
                        TVecDbl& unitNormal) const;

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

