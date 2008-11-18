/*!
 * \file Surface.hpp
 * \brief The parent abstract class of all the other surfaces
 * \author Jeremy L. Conlin and Seth R. Johnson
 * 
 */

#ifndef MCG_QUADRIC_HPP
#define MCG_QUADRIC_HPP

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
    typedef std::pair<bool, double> HitAndDist;

    // Intersect will determine if the particle at Postion and Direction
    // will intersect with the surface.  It returns a std::pair HitAndDist
    // It assumes that the PosSense defines whether the particles thinks it
    // has a positive sense with respect to the surface
    virtual HitAndDist intersect(const std::vector<double>& position, 
                                 const std::vector<double>& direction,
                                 bool PosSense) const = 0;

    //! create a copy of ourself
    virtual Surface* clone(const UserSurfaceIdType& newId) const = 0;

    //!Calculate whether a point has a positive 
    // sense to this surface without doing all the distance calcs
    virtual bool hasPosSense(const std::vector<double>& position) const = 0;

    // NOTE: this must be public if we ever have a generic Surface
    virtual ~Surface() {
//        cout << "Oh no, I (surface  << this << ") am dying!" << endl;
        /* * */
    }

    const UserSurfaceIdType& getUserId() const {
        return _userId;
    }

protected:
    Surface() : _userId()
    { /* * */ }

    Surface(const UserSurfaceIdType& userId) : _userId(userId)
    { /* * */ }

    bool _hasPosSense(const double eval) const;
    HitAndDist _calcQuadraticIntersect( double A, double B, double C,
            bool posSense ) const;

    //! output to a stream
    virtual std::ostream& _output( std::ostream& os ) const = 0;

private:
    //! hold the user's identification of this surface
    UserSurfaceIdType _userId;
};
/*----------------------------------------------------------------------------*/

// Defining this function here gives us the ability to make the same decision 
// for all Surface surfaces in one place.
inline bool Surface::_hasPosSense(const double eval) const{
    // Positive sense includes points "on" the surface (i.e. includes zero)
    // negative sense if the evaluated is strictly less than zero
    return ( eval >= 0 );
}

inline Surface::HitAndDist Surface::_calcQuadraticIntersect(
        double A, double B, double C, bool posSense) const
{
    bool particleHitsSurface = false;
    double distanceToIntercept = -1.0;

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

    return std::make_pair(particleHitsSurface, distanceToIntercept);
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

