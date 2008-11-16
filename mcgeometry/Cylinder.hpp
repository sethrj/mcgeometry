/*!
 * \file Cylinder.hpp
 * \brief 
 * \author Jeremy L. Conlin
 * 
 */

#ifndef MCG_CYLINDER_HPP
#define MCG_CYLINDER_HPP

#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/VectorPrint.hpp"

#include "Quadric.hpp"

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
class Cylinder : public Quadric {
public:
    Cylinder(const std::vector<double>& point, const std::vector<double>& axis,
             double radius)
        : _point(point), _axis(axis), _radius(radius) 
    {
        // we only live in 3-space
        Require( _point.size() == 3 );
        Require( _axis.size() == 3 );
        // require unit normal 
        Require(softEquiv(tranSupport::vectorNorm(_axis), 1.0));
    }

    Cylinder* clone() const { return new Cylinder(*this); }

    ~Cylinder() {   }

    bool hasPosSense(const std::vector<double>& position) const;

    HitAndDist intersect(const std::vector<double>& Position, 
            const std::vector<double>& Direction, bool PosSense) const;

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const std::vector<double> _point;
    const std::vector<double> _axis;
    const double _radius;

};

// TODO: eliminate use of pow with integer coefficients
// also having it work would be nice!

// Equation: (X-P)^2 - [(X-P).U]^2 - R^2 = 0
//      X = position 
//      P = point on axis of cylinder
//      U = direction vector of cylinder axis
inline bool Cylinder::hasPosSense(const std::vector<double>& position) const
{
    double eval(0);

    // Perform (X-P)^2
    double posMinusPoint(0);
    for( int i = 0; i < 3; ++i ){
        posMinusPoint += pow(position[i] - _point[0], 2);
    }

    // Perform [(X-P).U]^2
    for( int i = 0; i < 3; ++i ){
        eval += posMinusPoint - posMinusPoint*_axis[i];
    }

    eval -= _radius*_radius;
    
    return _hasPosSense(eval);
}

// This could be done more efficiently but this way is (arguably) closer to 
// the original math.
inline Quadric::HitAndDist Cylinder::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction, bool posSense) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    double A(0.0);   // 1-(direction._axis)^2
    for( int i = 0; i < 3; ++i ) {
        A += direction[i]*_axis[i];
    }
    A = 1-A*A;

    double B(0.0);                  // direction.{(position-_point)-
    double diff(0.0);               //_axis.[(position-_point)._axis]}
    for( int i = 0; i < 3; ++i ) {   
        diff = position[i] - _point[i];
        B += direction[i]*(diff - pow(_axis[i],2)*diff);
    }

    double C(0.0);                  // (position - _point)^2 - 
    diff = 0.0;
    for( int i = 0; i < 3; ++i ){   // [(position-_point)._axis]^2-_radius^2
        C += position[i]*_point[i];
        diff += pow( (position[i]*_point[i]*_axis[i]) , 2);
    }

    C -= (diff + _radius * _radius );

    return _calcQuadraticIntersect(A, B, C, posSense);
}

inline std::ostream& Cylinder::_output( std::ostream& os ) const {
    os  << "[ CYL   Point:  " << std::setw(10) << _point
        << " Axis: " << std::setw(10) << _axis 
        << " Radius: " << std::setw(5) << _radius << " ]";
    return os;
}
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif

