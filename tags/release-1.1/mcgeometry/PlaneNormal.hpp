/*!
 * \file   PlaneNormal.hpp
 * \brief  Plane surfaces normal to each axis
 * \author Seth R. Johnson
 */
#ifndef mcgeometry_PlaneNormal_hpp
#define mcgeometry_PlaneNormal_hpp
/*----------------------------------------------------------------------------*/
#include "Surface.hpp"

#include <blitz/tinyvec.h>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class PlaneNormal
 * \brief Plane normal to some surface, templated on the coordinate axis
 *
 * 0 = X axis
 * 1 = Y axis
 * 2 = Z axis
 */
template <unsigned int axis>
class PlaneNormal : public Surface {
public:
    //! User-called constructor.
    PlaneNormal( const double coord) : _coordinate(coord)
    { /* * */ }

    //! Copy the surface with a new user ID.
    PlaneNormal(
            const PlaneNormal& oldPlane,
            const UserSurfaceIdType& newId) :
        Surface(oldPlane, newId),
        _coordinate(oldPlane._coordinate)
    { /* * */ }

    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const
    {
        return new PlaneNormal<axis>(*this, newId);
    }

    ~PlaneNormal() { /* * */}

    //! Calculate whether a point has a positive sense to this surface
    bool hasPosSense(const TVecDbl& position) const;

    //! Determine distance to intersection with the surface.
    void intersect(
            const TVecDbl& position,
            const TVecDbl& direction,
            const bool posSense,
            bool& hit,
            double& distance) const;

    //! Calculate the surface normal at a point
    void normalAtPoint(
            const TVecDbl& position,
            TVecDbl& unitNormal) const;

    //! return the index along which we are oriented
    unsigned int getAxis() const {
        return axis;
    }
protected:
    //! output to a stream
    std::ostream& printStream( std::ostream& os ) const;

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

