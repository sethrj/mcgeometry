/*!
 * \file Quadric.hh
 * \brief Quadric surface geomerty
 * \author Seth R. Johnson
 * 
 */

#ifndef mc_Quadric_hh
#define mc_Quadric_hh
/*----------------------------------------------------------------------------*/

#include <vector>
#include <cmath>
#include "transupport/dbc.hh"
#include "transupport/SoftEquiv.hh"

namespace monteCarlo {

/*----------------------------------------------------------------------------*/
class Quadric {
public:
	typedef std::pair<bool, double> HitAndDist;

	//! see if a point is "positive" with respect to quadric surface normal
	// e.g. outside a sphere is positive, also to the right of a plane with unit
	// normal to the right
	virtual bool isLocPositive(
									const std::vector<double> &location) const = 0;

	//! being inside is the opposite of being positive (in the case of a closed surface)
	bool isLocInside(const std::vector<double> &location) {
		return !( this->isLocPositive(location));
	}

	//! get the distance to interception and final point
	virtual HitAndDist getIntercept(
										const std::vector<double> &location,
										const std::vector<double> &direction,
										const bool isLocPos) const = 0;
public:
	static HitAndDist evalQuadric(	const double A,
												const double B,
												const double C,
												const bool isInside);

	static void moveLoc(	const double distance,
								const std::vector<double> &direction,
								std::vector<double> &location);

	virtual ~Quadric() { /* * */ }
};

/*============================================================================*/
class Sphere : public Quadric {
public:
	//! constructor requires defining center and radius
	Sphere(	const std::vector<double> &_center,
				const double _radius) : m_center(_center),
												m_radius(_radius)
	{ 
		Require(m_center.size() == 3);
		Require(m_radius > 0.0);
	}
	
	//! construct a sphere at the origin
	Sphere(const double _radius) :   m_center(3, 0.0),
												m_radius(_radius)
	{ /* * */ }

	bool isLocPositive( const std::vector<double> &location) const;

	HitAndDist getIntercept(
									const std::vector<double> &location,
									const std::vector<double> &direction,
									const bool isInside) const;

	~Sphere(); 
private:
	typedef class Quadric Base;

	const std::vector<double> m_center;
	const double m_radius;
};
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif
