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
#include "Surface.hpp"

namespace mcGeometry {
/*============================================================================*/
/*!
 * \class Sphere
 * \brief General sphere.
 *
 * Positive sense is the outward normal.
 */
class Sphere : public Surface {
public:
    Sphere(std::vector<double>& C, double R)
        : _center(C), _radius(R)
    { 
        Insist(R > 0, "Sphere must have positive radius.");
    }

    Sphere(const Sphere& oldSphere, const UserSurfaceIdType& newId)
        : Surface(oldSphere, newId),
          _center(oldSphere._center),
          _radius(oldSphere._radius)
    { /* * */ }

    Surface* clone(const UserSurfaceIdType& newId) const {
        return new Sphere(*this, newId);
    }

    bool hasPosSense(const std::vector<double>& position) const;

    void intersect( const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

    void normalAtPoint( const std::vector<double>& position,
                        std::vector<double>& unitNormal) const;

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
inline void Sphere::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction, const bool posSense,
        bool& hit, double& distance) const
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
    _calcQuadraticIntersect(
            1,  // A
            B,  // B
            (selfDot - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
inline void Sphere::normalAtPoint( const std::vector<double>& position,
                                   std::vector<double>& unitNormal) const
{
    Require(position.size() == 3);
    // (position is on the outer edge of the sphere, we hope)
    Require(softEquiv(tranSupport::distance(position,_center),_radius));

    unitNormal.resize(3);

    // for a sphere, normal is a line from "position" to the origin
    double normValue = 0.0;
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] = position[i] - _center[i];
        normValue += unitNormal[i] * unitNormal[i];
    }
    normValue = std::sqrt(normValue);

    // normalize it
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] /= normValue;
    }

    // make sure it actually is a normal vector
    Ensure(softEquiv(tranSupport::vectorNorm(unitNormal), 1.0));
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
/*!
 * \class SphereO
 * \brief Sphere at the origin
 *
 * Positive sense is the outward normal.
 */
class SphereO : public Surface {
public:
    SphereO(double R)
        : _radius(R)
    { 
        Insist(R > 0, "SphereO must have positive radius.");
    }

    SphereO(const SphereO& oldSphere, const UserSurfaceIdType& newId)
        : Surface(oldSphere, newId),
          _radius(oldSphere._radius)
    { /* * */ }

    Surface* clone(const UserSurfaceIdType& newId) const {
        return new SphereO(*this, newId);
    }

    bool hasPosSense(const std::vector<double>& position) const;

    void intersect( const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

    void normalAtPoint( const std::vector<double>& position,
                        std::vector<double>& unitNormal) const;

    ~SphereO() { /* * */ };

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    const double _radius;
};


/*----------------------------------------------------------------------------*/
// Equation: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 - R^2 = 0
//      (x,y,z) = center of sphere
//      (x0,y0,z0) = position
inline bool SphereO::hasPosSense(const std::vector<double>& position) const
{
    return _hasPosSense(
              position[0]*position[0]
            + position[1]*position[1]
            + position[2]*position[2] 
                - _radius*_radius);
}

/*----------------------------------------------------------------------------*/
inline void SphereO::intersect(
        const std::vector<double>& position, 
        const std::vector<double>& direction, const bool posSense,
        bool& hit, double& distance) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);
    Require(softEquiv(tranSupport::vectorNorm(direction), 1.0));

    double selfDot = 0.0;
    double B = 0.0;
    
    // subtract out the center of the sphere
    for (int i = 0; i < 3; ++i) {
        B +=  position[i] * direction[i]; 
        selfDot += position[i] * position[i];
    }

    // find distance and whether it intercepts
    _calcQuadraticIntersect(
            1,  // A
            B,  // B
            (selfDot - _radius * _radius), //C
            posSense,
            hit, distance
            );
}
/*----------------------------------------------------------------------------*/
inline void SphereO::normalAtPoint( const std::vector<double>& position,
                            std::vector<double>& unitNormal) const
{
    // (position is on the outer edge of the sphere, we hope)
    Require(softEquiv(tranSupport::vectorNorm(position),_radius));

    unitNormal.resize(3);

    // for a sphere, normal is a line from "position" to the origin
    double normValue = 0.0;
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] = position[i];
        normValue += unitNormal[i] * unitNormal[i];
    }

    // normalize it
    for (unsigned int i = 0; i < 3; ++i) {
        unitNormal[i] /= normValue;
    }

    // make sure it actually is a normal vector
    Ensure(softEquiv(tranSupport::vectorNorm(unitNormal), 1.0));
}
/*----------------------------------------------------------------------------*/
//! output a stream which prints the SphereO's characteristics
inline std::ostream& SphereO::_output( std::ostream& os ) const {
    os  << "[ SPHEREO Radius: " << std::setw(5) << _radius
        << " ]";
    return os;
}


/*============================================================================*/
} // end namespace mcGeometry
#endif 

