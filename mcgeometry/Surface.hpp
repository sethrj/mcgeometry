/*!
 * \file Surface.hpp
 * \brief The parent abstract class of all the other surfaces
 * \author Jeremy L. Conlin and Seth R. Johnson
 * 
 */

#ifndef MCG_SURFACE_HPP
#define MCG_SURFACE_HPP

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include "transupport/dbc.hpp"

//using std::cout;
//using std::endl;

namespace mcGeometry {

/*----------------------------------------------------------------------------*/
/*!
 * \class Surface
 * \brief The parent abstract class of all the other surfaces .
 *
 * This defines an interface so that all kinds of surfaces can appear as the
 * same general object.
 */

// template <typename UserSurfaceIdType>
class Surface {
protected:
    //! This might be replaced with a template later.
    typedef unsigned int UserSurfaceIdType;

private:
    //! Output a Surface to a stream.
    friend std::ostream& operator<<( std::ostream&, const Surface& );
public:

    //! Extra information about surfaces (i.e. is it reflecting)
    enum SurfaceFlags {
        NONE       = 0,
        REFLECTING = 1
    };

//    //! surface types for volume calculation
//    enum SurfaceKind {
//        SK_OTHER       = 0,
//        SK_PLANENORMAL = 1,
//        SK_SPHERE      = 2,
//        SK_CYLINDER    = 3
//    }

    /*!
     * \brief Determine distance to intersection with the surface.  
     *
     * See if going from a position in a direction with a surface sense
     * intersects the surface; pass back whether it hits and what the distance
     * is.
     */
    virtual void intersect(const std::vector<double>& position, 
                           const std::vector<double>& direction,
                           const bool PosSense,
                           bool& hit,
                           double& distance) const = 0;

    //! Calculate whether a point has a positive sense to this surface without
    //!  doing all the distance calculations
    virtual bool hasPosSense(const std::vector<double>& position) const = 0;

    //! \brief Calculate the surface normal at a point, necessary for reflection
    //!
    //!  The value returned is the surface normal with a direction in the
    //!  "positive" sense of the surface.
    virtual void normalAtPoint( const std::vector<double>& position,
                                std::vector<double>& unitNormal) const = 0;
                
    //! Create a "new" copy of ourself, so that a user-created surface can be
    //! retained by MCGeometry.
    virtual Surface* clone(const UserSurfaceIdType& newId) const = 0;

    // //! return some identity about ourselves for volume calculation etc.
    // virtual SurfaceKind getKind() const = 0;


    //! Return the user ID associated with this surface.
    const UserSurfaceIdType& getUserId() const {
        return _userId;
    }

    //! Are we a reflecting surface?
    const bool isReflecting() {
        return (_flags & REFLECTING);
    }

    //! Generic virtual distructor.
    // NOTE: this must be public if we ever have a generic Surface
    virtual ~Surface()
    { /* * */ }

    //! Set different options for the surfaces
    void setReflecting() {
        _flags = static_cast<SurfaceFlags>(_flags | REFLECTING);
    }

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

    //! Determine if an evaluated point is considered to have a positive sense
    //! or not
    bool _hasPosSense(const double eval) const;

    //! Calculate the intersection of a surface with calculated quadratic
    //! values.
    void _calcQuadraticIntersect(
                            const double A, const double B, const double C,
                            const bool posSense,
                            bool& particleHitsSurface,
                            double& distanceToIntercept ) const;

    //! Output surface information to a stream.
    virtual std::ostream& _output( std::ostream& os ) const = 0;

private:
    //! Store the user's identification of this surface.
    UserSurfaceIdType _userId;
    //! Store extra information about the surface (e.g. whether it is
    //! reflecting)
    SurfaceFlags      _flags;
};
/*----------------------------------------------------------------------------*/

//! Does the evaluated quadric equation result in a positive or negative sense?
// Defining this function here gives us the ability to make the same decision 
// for all Surface surfaces in one place.
inline bool Surface::_hasPosSense(const double eval) const{
    // Positive sense includes points "on" the surface (i.e. includes zero)
    // negative sense if the evaluated is strictly less than zero
    return ( eval >= 0 );
}
/*----------------------------------------------------------------------------*/
inline void Surface::_calcQuadraticIntersect(
        const double A, const double B, const double C, const bool posSense,
        bool& particleHitsSurface, double& distanceToIntercept) const
{
    double Q = B*B - A*C;

    if (Q < 0) {
        particleHitsSurface = false;
        distanceToIntercept = 0.0;
    } 
    else {
        if (not posSense) { //inside the surface (negative orientation)
            if (B <= 0) {   // headed away from the surface
                if (A > 0) {    // surface is curving upward
                    particleHitsSurface = true;
                    distanceToIntercept = (std::sqrt(Q) - B)/A;
                } 
                else {  // surface curving away and headed in, never hits it
                    particleHitsSurface = false;
                    distanceToIntercept = 0.0;
                }
            } 
            else {  // particle is heading toward the surface
                particleHitsSurface = true;
                distanceToIntercept = std::max(0.0, -C/(std::sqrt(Q) + B));
            }
        } 
        else {  // particle is outside
            if (B >= 0) {   // particle headed away
                if (A >= 0) {
                    particleHitsSurface = false;
                    distanceToIntercept = 0.0;
                } 
                else {
                    particleHitsSurface = true;
                    distanceToIntercept = -(std::sqrt(Q) + B)/A;
                }
            } 
            else {
                particleHitsSurface = true;
                distanceToIntercept = std::max(0.0, C/(std::sqrt(Q) - B));
            }
        }
    }

    Ensure( distanceToIntercept >= 0.0 );
}

/*----------------------------------------------------------------------------*/
//! \brief Output a general Surface-descended surface
// for polymorphism, we have to call a private inherited method
inline std::ostream& operator<<( std::ostream& os, const Surface& q)
{
    return q._output(os);
}

//! \brief Output a Surface and its corresponding sense
inline std::ostream& operator<<( std::ostream& os,
                const std::pair<Surface*, bool>& surfAndSense)
{
    if (surfAndSense.second == true)
        os << "+";
    else
        os << "-";
    os << surfAndSense.first->getUserId();
//    os << *(surfAndSense.first);

    return os;
}
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
/*----------------------------------------------------------------------------*/
#endif

