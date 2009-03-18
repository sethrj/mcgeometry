/*!
 * \file   Sphere.hpp
 * \brief  Sphere surface
 * \author Seth R. Johnson
 * 
 */

#ifndef MCG_SPHERE_HPP
#define MCG_SPHERE_HPP

#include <cmath>
#include <iostream>
#include <blitz/tinyvec.h>
#include "transupport/dbc.hpp"

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
    //! Blitz++ TinyVector of length D stores position/direction/etc.
    typedef blitz::TinyVector<double, 3> TVecDbl; 
    
public:
    //! User-called constructor.
    Sphere(TVecDbl& C, double R)
        : _center(C), _radius(R)
    { 
        Insist(R > 0, "Sphere must have positive radius.");
    }

    //! Copy the surface with a new user ID.
    Sphere(const Sphere& oldSphere, const UserSurfaceIdType& newId)
        : Surface(oldSphere, newId),
          _center(oldSphere._center),
          _radius(oldSphere._radius)
    { /* * */ }

    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new Sphere(*this, newId);
    }

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

    ~Sphere() { /* * */ };

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    //! Center point of the sphere.
    const TVecDbl _center;
    //! Radius of the sphere.
    const double _radius;
};
/*============================================================================*/
/*!
 * \class SphereO
 * \brief Sphere at the origin
 *
 * Positive sense is the outward normal.
 */
class SphereO : public Surface {
public:
    //! User-called constructor.
    SphereO(double radius)
        : _radius(radius)
    { 
        Insist(_radius > 0, "SphereO must have positive radius.");
    }

    //! Copy the surface with a new user ID.
    SphereO(const SphereO& oldSphere, const UserSurfaceIdType& newId)
        : Surface(oldSphere, newId),
          _radius(oldSphere._radius)
    { /* * */ }

    //! Create a "new" copy of the surface.
    Surface* clone(const UserSurfaceIdType& newId) const {
        return new SphereO(*this, newId);
    }

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

    ~SphereO() { /* * */ };

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    //! Radius of the sphere.
    const double _radius;
};
/*============================================================================*/
} // end namespace mcGeometry
#endif 

