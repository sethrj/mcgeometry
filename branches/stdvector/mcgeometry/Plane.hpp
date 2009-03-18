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

