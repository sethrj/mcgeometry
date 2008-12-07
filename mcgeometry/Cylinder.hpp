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
    diff = std::vector<double>(position);
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
    // "position" should be on the surface
    Require(position.size() == 3);

    Insist(0, "Not yet implemented.");
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
    CylinderNormal( double radius )
        : _radius(radius) 
    {
        Require( _radius > 0.0 );
    }

    //! Copy the surface with a new user ID.
    CylinderNormal(const CylinderNormal& oldCylinder,
                   const UserSurfaceIdType& newId)
        : Surface(newId),
          _radius(oldCylinder._radius)
    { /* * */ }


    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new CylinderNormal(*this, newId);
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

};
/*----------------------------------------------------------------------------*/
template<>
inline bool CylinderNormal<0>::hasPosSense(
                const std::vector<double>& position) const
{
    Require(position.size() == 3);

    double eval = 
              position[1] * position[1]
            + position[2] * position[2] 
            - _radius * _radius;

    return _hasPosSense( eval );
}

template<>
inline bool CylinderNormal<1>::hasPosSense(
                const std::vector<double>& position) const
{
    Require(position.size() == 3);

    return _hasPosSense(
              position[0] * position[0]
            + position[2] * position[2] 
            - _radius * _radius
            );
}

template<>
inline bool CylinderNormal<2>::hasPosSense(
                const std::vector<double>& position) const
{
    Require(position.size() == 3);

    return _hasPosSense(
            position[0] * position[0]
            + position[1] * position[1] 
            - _radius * _radius
            );
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline void CylinderNormal<axis>::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Insist(0, "Not yet implemented.");
    return _calcQuadraticIntersect(0.0, 0.0, 0.0, posSense, hit, distance);
}

template<>
inline void CylinderNormal<2>::intersect(
                const std::vector<double>& position, 
                const std::vector<double>& direction,
                const bool posSense,
                bool& hit, double& distance) const
{
    Require(direction.size() == 3);
    Require(position.size() == 3);

    double A = direction[0] * direction[0]
             + direction[1] * direction[1];

    double B = direction[0] * position[0]
             + direction[1] * position[1];

    double C = direction[0] * direction[0]
             + direction[1] * direction[1]
             - _radius * _radius;

    return _calcQuadraticIntersect(A, B, C, posSense,
                                   hit, distance);
}
/*----------------------------------------------------------------------------*/
template<unsigned int axis>
inline std::ostream& CylinderNormal<axis>::_output( std::ostream& os ) const
{
    os  << "[ CYL  Axis: " << std::setw(10) << axis 
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
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

