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
    //! User-called constructor.
    Sphere(std::vector<double>& C, double R)
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
    //! Center point of the sphere.
    const std::vector<double> _center;
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
    //! Radius of the sphere.
    const double _radius;
};
/*============================================================================*/
} // end namespace mcGeometry
#endif 

