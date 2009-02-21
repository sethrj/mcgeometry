/*!
 * \file Cylinder.hpp
 * \brief Cylinder surface object.
 * \author Jeremy L. Conlin and Seth R. Johnson
 */

#ifndef MCG_CYLINDER_HPP
#define MCG_CYLINDER_HPP

#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

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
    //! User-called constructor.
    Cylinder(const std::vector<double>& point,
             const std::vector<double>& axis,
             double radius)
        : _pointOnAxis(point), _axis(axis), _radius(radius) 
    {
        // we only live in 3-space
        Require( _pointOnAxis.size() == 3 );
        Require( _axis.size() == 3 );
        // require unit normal 
        Require(softEquiv(tranSupport::vectorNorm(_axis), 1.0));
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
    //! some point through which the cylinder's axis passes
    const std::vector<double> _pointOnAxis;
    //! axis about which the cylinder is centered
    const std::vector<double> _axis;
    //! cylinder radius
    const double _radius;
};
/*============================================================================*/

/*!
 * \class CylinderNormal
 * \brief A cylinder along one of the axes
 */
template<unsigned int axis>
class CylinderNormal : public Surface {
public:
    //! User-called constructor.
    CylinderNormal( const std::vector<double>& point,
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
    //! some point through which the cylinder's axis passes
    const std::vector<double> _pointOnAxis;
    //! axis about which the cylinder is centered
    const std::vector<double> _axis;
    //! cylinder radius
    const double _radius;

    //! Return unrolled (x .* x) for vector x tailored to this axis
    double _dotProduct( const std::vector<double>& x,
                        const std::vector<double>& y) const;
};
/*----------------------------------------------------------------------------*/
template<>
inline double CylinderNormal<0>::_dotProduct(
                        const std::vector<double>& x,
                        const std::vector<double>& y) const
{
    return x[1] * y[1] + x[2] * y[2];
}

template<>
inline double CylinderNormal<1>::_dotProduct(
                        const std::vector<double>& x,
                        const std::vector<double>& y) const
{
    return x[0] * y[0] + x[2] * y[2];
}

template<>
inline double CylinderNormal<2>::_dotProduct(
                        const std::vector<double>& x,
                        const std::vector<double>& y) const
{
    return x[0] * y[0] + x[1] * y[1];
}

/*============================================================================*/

//! provide typedefs for user interaction
typedef CylinderNormal<0> CylinderX;
//! provide typedefs for user interaction
typedef CylinderNormal<1> CylinderY;
//! provide typedefs for user interaction
typedef CylinderNormal<2> CylinderZ;

/*============================================================================*/
} // end namespace mcGeometry
#endif

