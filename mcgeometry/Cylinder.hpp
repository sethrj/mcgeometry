/*!
 * \file   Cylinder.hpp
 * \brief  Declaration for \c Cylinder class.
 * \author Jeremy L. Conlin and Seth R. Johnson
 */
#ifndef MCG_CYLINDER_HPP
#define MCG_CYLINDER_HPP
/*----------------------------------------------------------------------------*/

#include <blitz/tinyvec.h>
#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

#include "Surface.hpp"

#include <iostream>

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class Cylinder
 * \brief A general cylindrical surface
 */
class Cylinder : public Surface {
public:
    //! Blitz++ TinyVector of length D stores position/direction/etc.
    typedef blitz::TinyVector<double, 3> TVecDbl;

public:
    //! User-called constructor.
    Cylinder(const TVecDbl& point,
             const TVecDbl& axis,
             double radius)
        : _pointOnAxis(point), _axis(axis), _radius(radius)
    {
        // we only live in 3-space
        // require unit normal
        Require(tranSupport::checkDirectionVector(_axis));
        Require( _radius > 0.0 );
    }

    //! Copy the surface with a new user ID.
    Cylinder(const Cylinder& oldCylinder, const UserSurfaceIdType& newId)
        : Surface(oldCylinder, newId),
          _pointOnAxis(oldCylinder._pointOnAxis),
          _axis(oldCylinder._axis),
          _radius(oldCylinder._radius)
    { /* * */ }


    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new Cylinder(*this, newId);
    }

    ~Cylinder() { /* * */ }

    //! Calculate whether a point has a positive sense to this surface
    bool hasPosSense(const TVecDbl& position) const;

    //! Determine distance to intersection with the surface.
    void intersect( const TVecDbl& position,
                    const TVecDbl& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

    //! Calculate the surface normal at a point
    void normalAtPoint( const TVecDbl& position,
                        TVecDbl& unitNormal) const;
protected:
    //! Output to a stream
    std::ostream& printStream( std::ostream& os ) const;

private:
    //! some point through which the cylinder's axis passes
    const TVecDbl _pointOnAxis;
    //! axis about which the cylinder is centered
    const TVecDbl _axis;
    //! cylinder radius
    const double _radius;
};
/*============================================================================*/
} // end namespace mcGeometry
#endif

