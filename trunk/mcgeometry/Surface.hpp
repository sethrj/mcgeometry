/*!
 * \file   Surface.hpp
 * \brief  The parent abstract class of all the other surfaces
 * \author Jeremy L. Conlin and Seth R. Johnson
 *
 */
#ifndef MCG_SURFACE_HPP
#define MCG_SURFACE_HPP
/*----------------------------------------------------------------------------*/
#include <utility>
#include <iosfwd>
#include <blitz/tinyvec.h>

#include "transupport/dbc.hpp"

//using std::cout;
//using std::endl;

namespace mcGeometry {
/*============================================================================*/

/*!
 * \class Surface
 * \brief The parent abstract class of all the other surfaces .
 *
 * This defines an interface so that all kinds of surfaces can appear as the
 * same general object.
 */

class Surface
{
public:
    //! Blitz++ TinyVector of length D stores position/direction/etc.
    typedef blitz::TinyVector<double, 3> TVecDbl;

    //! This might be replaced with a template later.
    typedef unsigned int UserSurfaceIdType;
public:
    //! Extra information about surfaces (i.e. is it reflecting)
    enum SurfaceFlags {
        NONE       = 0,
        REFLECTING = 1
    };

    /*!
     * \brief Determine distance to intersection with the surface.
     *
     * See if going from a position in a direction with a surface sense
     * intersects the surface; pass back whether it hits and what the distance
     * is.
     */
    virtual void intersect(
            const TVecDbl& position,
            const TVecDbl& direction,
            const bool PosSense,
            bool& hit,
            double& distance) const = 0;

    //! Calculate whether a point has a positive sense to this surface without
    //!  doing all the distance calculations
    virtual bool hasPosSense(const TVecDbl& position) const = 0;

    //! \brief Calculate the surface normal at a point, necessary for reflection
    //!
    //!  The value returned is the surface normal with a direction in the
    //!  "positive" sense of the surface.
    virtual void normalAtPoint(
            const TVecDbl& position,
            TVecDbl& unitNormal) const = 0;

    //! Create a "new" copy of ourself, so that a user-created surface can be
    //! retained by MCGeometry.
    virtual Surface* clone(const UserSurfaceIdType& newId) const = 0;

    //! Return the user ID associated with this surface.
    UserSurfaceIdType getUserId() const {
        return _userId;
    }

    //! Are we a reflecting surface?
    bool isReflecting() const {
        return (_flags & REFLECTING);
    }

    //! Generic virtual distructor.
    virtual ~Surface()
    { /* * */ }

    //! Set different options for the surfaces
    void setReflecting() {
        _flags = static_cast<SurfaceFlags>(_flags | REFLECTING);
    }

    //! Output surface information to a stream.
    virtual std::ostream& printStream( std::ostream& os ) const = 0;

protected:
    //! Create a surface without any extra information
    Surface() : _userId(), _flags(NONE)
    { /* * */ }

    //! Copy an old surface, but with a user ID
    Surface(const Surface& oldSurface, const UserSurfaceIdType& userId)
        : _userId(userId), _flags(oldSurface._flags)
    { /* * */ }

    //! Copy an old surface, and use th eold surface user ID.
    Surface(const Surface& oldSurface)
        : _userId(oldSurface._userId), _flags(oldSurface._flags)
    { /* * */ }

    /*! \brief Does the evaluated quadric eq result in a pos or neg sense?
     * Defining this function here gives us the ability to make the same
     * decision for all Surface surfaces in one place.
     */
    bool _hasPosSense(const double eval) const;

    //! Calculate the intersection of a surface with calculated quadratic
    //! values.
    void _calcQuadraticIntersect(
                            const double A, const double B, const double C,
                            const bool posSense,
                            bool& particleHitsSurface,
                            double& distanceToIntercept ) const;

private:
    //! Store the user's identification of this surface.
    UserSurfaceIdType _userId;
    //! Store extra information about the surface
    SurfaceFlags      _flags;
};
/*----------------------------------------------------------------------------*/

inline bool Surface::_hasPosSense(const double eval) const
{
    // Positive sense includes points "on" the surface (i.e. includes zero)
    // negative sense if the evaluated is strictly less than zero
    return ( eval >= 0 );
}

/*----------------------------------------------------------------------------*/
/*! \brief Output a general Surface-descended surface

 * For polymorphism, we have to call a inherited method
 */
std::ostream& operator<<( std::ostream& os, const Surface& q);

//! \brief Output a Surface and its corresponding sense
std::ostream& operator<<(
        std::ostream& os,
        const std::pair<Surface*, bool>& surfAndSense);
/*============================================================================*/
} // end namespace mcGeometry
#endif

