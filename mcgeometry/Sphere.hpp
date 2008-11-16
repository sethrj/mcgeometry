/*!
 * \file Sphere.hpp
 * \brief Sphere surface
 * \author Jeremy L. Conlin
 * 
 */

#ifndef MCG_SPHERE_HPP
#define MCG_SPHERE_HPP

#include <vector>
#include <cmath>
#include <iostream>
#include "transupport/VectorPrint.hpp"
#include "transupport/VectorMath.hpp"
#include "transupport/dbc.hpp"
#include "transupport/SoftEquiv.hpp"
#include "Quadric.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class Sphere
 * \brief General sphere.
 *
 * Positive sense is the outward normal.
 */
class Sphere : public Quadric {
public:
    Sphere(std::vector<double>& C, double R)
        : _center(C), _radius(R)
    { 
        Insist(R > 0, "Sphere must have positive radius.");
    }

    Sphere* clone() const { return new Sphere(*this); }

    bool hasPosSense(const std::vector<double>& position) const;

    HitAndDist intersect(const std::vector<double>& position, 
            const std::vector<double>& direction, bool posSense) const;

    ~Sphere() { /* * */ };

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const std::vector<double> _center;
    const double _radius;
};


/*----------------------------------------------------------------------------*/
// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
inline bool Sphere::hasPosSense(const std::vector<double>& position) const
{
    double eval = 0.0;
    double temp;

    temp = position[0]-_center[0];
    eval += temp*temp;

    temp = position[1]-_center[1];
    eval += temp*temp;

    temp = position[2]-_center[2];
    eval += temp*temp;

    eval -= _radius*_radius;

    return _hasPosSense(eval);
}

/*----------------------------------------------------------------------------*/
inline Quadric::HitAndDist Sphere::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction, bool posSense) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    double selfDot = 0.0;
    double B = 0.0;
    double movedLoc;
    
    // subtract out the center of the sphere
    for (int i = 0; i < 3; ++i) {
        movedLoc = (position[i] - _center[i]);
        B +=  movedLoc * direction[i]; 
        selfDot += movedLoc * movedLoc;
    }

    // find distance and whether it intercepts
    Quadric::HitAndDist theResult =
        _calcQuadraticIntersect(
            1,  // A
            B,  // B
            selfDot - _radius * _radius, //C
            posSense);

    return theResult;

    // THIS CODE, SADLY, DOES NOT WORK :
//
//    // Compute relative coordinates
//    std::vector<double> xr(position);
//
//    tranSupport::vectorMinusEq(xr, _center);
//
//    // Compute quadratic coefficients
//    double A = 1.0;
//    double B = tranSupport::vectorDot(direction, xr);
//    double C = _radius*_radius - tranSupport::vectorDot(xr, xr);
//
//    // Call quadratic solver and return result
//    return calcQuadraticIntersect(A, B, C, posSense);
//    
}

/*----------------------------------------------------------------------------*/
//! output a stream which prints the Sphere's characteristics
inline std::ostream& Sphere::_output( std::ostream& os ) const {
    os  << "[ SPHERE Center: " << std::setw(10) << _center
        << " Radius: " << std::setw(5) << _radius
        << " ]";
    return os;
}

/*============================================================================*/
} // end namespace mcGeometry
#endif 

