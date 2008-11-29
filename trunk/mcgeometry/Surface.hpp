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

//#include <iostream>
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
    typedef unsigned int UserSurfaceIdType;
private:
    //! Output a Surface to a stream 
    friend std::ostream& operator<<( std::ostream&, const Surface& );
public:

    //! extra information about surfaces
    // TODO this is not really yet implemented
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

    //! Determine 
    // See if going from a position in a direction with a surface sense 
    // intersects the surface; pass back whether it hits and what the distance
    // is
    virtual void intersect(const std::vector<double>& position, 
                           const std::vector<double>& direction,
                           const bool PosSense,
                           bool& hit,
                           double& distance) const = 0;

    //! create a copy of ourself
    virtual Surface* clone(const UserSurfaceIdType& newId) const = 0;

    // //! return some identity about ourselves for volume calculation etc.
    // virtual SurfaceKind getKind() const = 0;

    //!Calculate whether a point has a positive 
    // sense to this surface without doing all the distance calcs
    virtual bool hasPosSense(const std::vector<double>& position) const = 0;

    // NOTE: this must be public if we ever have a generic Surface
    virtual ~Surface() {
        /* * */
    }

    const UserSurfaceIdType& getUserId() const {
        return _userId;
    }

protected:
    Surface() : _userId(), _flags(NONE)
    { /* * */ }

    Surface(const UserSurfaceIdType& userId) : _userId(userId), _flags(NONE)
    { /* * */ }

    bool _hasPosSense(const double eval) const;

    void _calcQuadraticIntersect(
                            const double A, const double B, const double C,
                            const bool posSense,
                            bool& particleHitsSurface,
                            double& distanceToIntercept ) const;

    //! output to a stream
    virtual std::ostream& _output( std::ostream& os ) const = 0;

private:
    //! hold the user's identification of this surface
    UserSurfaceIdType _userId;
    SurfaceFlags      _flags;
};
/*----------------------------------------------------------------------------*/

// Defining this function here gives us the ability to make the same decision 
// for all Surface surfaces in one place.
inline bool Surface::_hasPosSense(const double eval) const{
    // Positive sense includes points "on" the surface (i.e. includes zero)
    // negative sense if the evaluated is strictly less than zero
    return ( eval >= 0 );
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
    os << *(surfAndSense.first);

    return os;
}
/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
/*----------------------------------------------------------------------------*/
#endif

