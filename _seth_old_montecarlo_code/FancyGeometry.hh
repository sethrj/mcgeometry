/*!
 * \file FancyGeometry.hh
 * \brief FancyGeometry surface geomerty
 * \author Seth R. Johnson
 * 
 */

#ifndef mc_FancyGeometry_hh
#define mc_FancyGeometry_hh
/*----------------------------------------------------------------------------*/

#include <boost/tr1/memory.hpp>
#include <vector>
#include "transupport/dbc.hh"
#include "Quadric.hh"

namespace monteCarlo {

/*----------------------------------------------------------------------------*/
class FancyGeometry {
public:
	typedef std::tr1::shared_ptr<const monteCarlo::Quadric> spCQuadric;

	typedef std::vector< int > intVec;
	typedef std::vector< std::vector<int> > intVecVec;

	// CONSTRUCTORS
	FancyGeometry() :
		theSurfaces(),
		regions(),
		deadRegion(0)
	{
		/* * */
	}


	// ************* //
	unsigned int getCellFromPoint( const std::vector<double> &location) const;

	//! return the number of cells
	unsigned int getNumCells() {
		return regions.size();
	}

	//! return the number of cells
	unsigned int getNumSurfaces() {
		return theSurfaces.size();
	}
	// ************ //
	//! add a new quadric
	// return value: the surface's index (one-based)
	unsigned int addSurface( const spCQuadric &newSurface ) { 
		//add the smart pointer to our list
		theSurfaces.push_back(newSurface);

		return theSurfaces.size();
	}

	// ************ //
	//! get a quadric
	const spCQuadric& getSurface( const unsigned int surfaceIndex) const { 
		Require(surfaceIndex > 0 && surfaceIndex <= theSurfaces.size());

		return theSurfaces[surfaceIndex - 1];
	}

	// ************ //
	//! define a new region
	int addRegion( const std::vector<int> &regionDef ) {
		Require(checkRegion(regionDef));

		regions.push_back( regionDef );

		return regions.size();
	}

	//! add a dead region, where a cell is killed
	void setDeadRegion(const unsigned int theRegion) {
		Require(theRegion > 0 && theRegion <= regions.size());

		Insist(deadRegion == 0, "Trying to set the dead region for the second time!");

		deadRegion = theRegion;
	}

	//! see if the region is dead
	bool isDeadRegion(const unsigned int theRegion) const {
		return (theRegion == deadRegion);
	}

	// ************ //
	//! see if some location and direction in a cell hits another cell
	void getIntercept(	const unsigned int currentCell,
								const std::vector<double> &location,
								const std::vector<double> &direction,
								bool &didHit,
								unsigned int &newCell,
								unsigned int &crossedSurf,
								double &dist) const;

	//! make sure a region is composed of valid surfaces
	bool checkRegion(const std::vector<int> &regionDef ) const;

	~FancyGeometry();
private:
	// disallow some as-yet unsupported operations
	FancyGeometry(FancyGeometry const&) { /* * */ }
	FancyGeometry& operator=(FancyGeometry const&) { /* * */ return *this; }

private:
	//! store the quadrics that make up our surfaces
	std::vector<spCQuadric> theSurfaces;

	//! store the region definitions
	// each region is defined by a vector of surfaces with orientation
	intVecVec regions;

	//! the outside universe that kills particles dead!
	unsigned int deadRegion;
};
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif
