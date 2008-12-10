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
using std::cout;
using std::endl;

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
/*----------------------------------------------------------------------------*/
// Equation: (X-P)^2 - [(X-P).U]^2 - R^2 = 0
//      X = position 
//      P = point on axis of cylinder
//      U = direction vector of cylinder axis
inline bool Cylinder::hasPosSense(const std::vector<double>& position) const
{
    Require(position.size() == 3);
    double eval(0);
    double diff(0);
    double tmpValue(0);

    for( int i = 0; i < 3; ++i ){
        // Perform (X-P)^2
        diff = position[i] - _pointOnAxis[i];
        eval += diff*diff;

        // Perform [(X-P).U]^2
        tmpValue += diff*_axis[i];
    }

    eval -= tmpValue*tmpValue + _radius*_radius;

    return _hasPosSense(eval);
}

/*----------------------------------------------------------------------------*/
// TODO: optimization needed here
inline void Cylinder::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction,
        const bool posSense,
        bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    double A(0.0);   // 1-(direction._axis)^2
    for( int i = 0; i < 3; ++i ){
        A += direction[i]*_axis[i];
    }
    A = 1-A*A;

    double B(0.0);
    double b(0.0);      // Temporary variable
    std::vector<double> bvec(3,0.0);
    std::vector<double> diff(position);
    tranSupport::vectorMinusEq(diff, _pointOnAxis);
    b = tranSupport::vectorDot3(diff, _axis);
    for( int i = 0; i < 3; ++i ) bvec[i] = _axis[i]*b;
    tranSupport::vectorMinusEq(diff, bvec);
    B = tranSupport::vectorDot3(diff, direction);

    double C(0.0);
    double c(0.0);
    diff = position;
    tranSupport::vectorMinusEq(diff, _pointOnAxis);
    c = tranSupport::vectorDot3(diff, _axis);
    C = tranSupport::vectorDot3(diff, diff) - c*c - _radius*_radius;

    _calcQuadraticIntersect(A, B, C, posSense, hit, distance);
}
/*----------------------------------------------------------------------------*/
inline void Cylinder::normalAtPoint(
                const std::vector<double>& position,
                std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);

    // "unitNormal" is now particle location translated to cylinder
    unitNormal = position;
    tranSupport::vectorMinusEq(unitNormal, _pointOnAxis);

    double cosTheta = tranSupport::vectorDot3(unitNormal, _axis);

    // make "unitNormal" the un-normalized difference to the axis (position
    // minus projection)
    for (int i = 0; i < 3; ++i)
        unitNormal[i] -= _axis[i] * cosTheta;

    // the vector length should now be just the radius, if we have done it right
    Check(softEquiv(tranSupport::vectorNorm(unitNormal), _radius));

    for (int i = 0; i < 3; ++i)
        unitNormal[i] /= _radius;

    Ensure(softEquiv(tranSupport::vectorNorm(unitNormal), 1.0));
}
/*----------------------------------------------------------------------------*/
inline std::ostream& Cylinder::_output( std::ostream& os ) const {
    os  << "[ CYL   Point:  " << std::setw(10) << _pointOnAxis
        << " Axis: " << std::setw(10) << _axis 
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
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

/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline bool CylinderNormal<axis>::hasPosSense(
                const std::vector<double>& position) const
{
    Require(position.size() == 3);

    return _hasPosSense( _dotProduct(position, position) - _radius * _radius);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void CylinderNormal<axis>::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Require(direction.size() == 3);
    Require(position.size() == 3);

    //temporary for now
    Insist(softEquiv(_pointOnAxis, std::vector<double>(3,0.0)),
            "As of now, CylinderNormal must be on the axis.");

    double A = _dotProduct(direction, direction);

    double B = _dotProduct(direction, position);

    double C = _dotProduct(position, position) - _radius*_radius;

    return _calcQuadraticIntersect(A, B, C, posSense,
                                   hit, distance);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void CylinderNormal<axis>::normalAtPoint(
                        const std::vector<double>& position,
                        std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);

    Insist(0, "Not yet implemented.");

    Ensure(softEquiv(tranSupport::vectorNorm(unitNormal), 1.0));
}
/*----------------------------------------------------------------------------*/
template<>
inline std::ostream& CylinderNormal<0>::_output( std::ostream& os ) const
{
    os  << "[ CYLX: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*----------------------------------------------------------------------------*/
template<>
inline std::ostream& CylinderNormal<1>::_output( std::ostream& os ) const
{
    os  << "[ CYLY: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}/*----------------------------------------------------------------------------*/
template<>
inline std::ostream& CylinderNormal<2>::_output( std::ostream& os ) const
{
    os  << "[ CYLZ: "
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}/*============================================================================*/

//! provide typedefs for user interaction
typedef CylinderNormal<0> CylinderX;
//! provide typedefs for user interaction
typedef CylinderNormal<1> CylinderY;
//! provide typedefs for user interaction
typedef CylinderNormal<2> CylinderZ;

/*============================================================================*/
} // end namespace mcGeometry
#endif

