/*!
 * \file   CylinderNormal.hpp
 * \brief  Declaration for \c CylinderNormal class.
 * \author Jeremy L. Conlin and Seth R. Johnson
 */
#ifndef mcgeometry_CylinderNormal_hpp
#define mcgeometry_CylinderNormal_hpp
/*----------------------------------------------------------------------------*/

#include <blitz/tinyvec.h>
#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

#include "Surface.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class CylinderNormal
 * \brief A cylinder along one of the axes
 */
template<unsigned int axis>
class CylinderNormal : public Surface
{
public:
    //! User-called constructor.
    CylinderNormal( const TVecDbl& point,
                    double radius )
        : _pointOnAxis(point), _radius(radius)
    {
        Require( _radius > 0.0 );
    }

    //! Copy the surface with a new user ID.
    CylinderNormal(const CylinderNormal<axis>& oldCylinder,
                   const UserSurfaceIdType& newId)
        : Surface(oldCylinder, newId),
          _radius(oldCylinder._radius)
    { /* * */ }


    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new CylinderNormal<axis>(*this, newId);
    }

    ~CylinderNormal() { /* * */ }

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
    //! output to a stream
    std::ostream& printStream( std::ostream& os ) const;
protected:
    //! Return unrolled (x .* x) for vector x tailored to this axis
    double _dotProduct( const TVecDbl& x,
                        const TVecDbl& y) const;
private:
    //! Some point through which the cylinder's axis passes
    const TVecDbl _pointOnAxis;
    //! Axis about which the cylinder is centered
    const TVecDbl _axis;
    //! Cylinder radius
    const double _radius;
};

/*----------------------------------------------------------------------------*/

//! \cond
template<>
inline double CylinderNormal<0>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[1] * y[1] + x[2] * y[2];
}

template<>
inline double CylinderNormal<1>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[0] * y[0] + x[2] * y[2];
}

template<>
inline double CylinderNormal<2>::_dotProduct(
                        const TVecDbl& x,
                        const TVecDbl& y) const
{
    return x[0] * y[0] + x[1] * y[1];
}
//! \endcond
/*============================================================================*/

//! A cylinder along the X axis
typedef CylinderNormal<0> CylinderX;
//! A cylinder along the Y axis
typedef CylinderNormal<1> CylinderY;
//! A cylinder along the Z axis
typedef CylinderNormal<2> CylinderZ;

/*============================================================================*/
} // end namespace mcGeometry
#endif

