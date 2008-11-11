/*!
 * \file FancyGeometry.hh
 * \brief Holds quadric geometries and regions etc.
 * \author Seth R. Johnson
 * 
 */

#include <vector>
#include <cmath>
#include <limits>
#include "transupport/dbc.hh"
#include "transupport/VectorComp.hh"
#include "transupport/VectorMath.hh"
#include "FancyGeometry.hh"
#include "Quadric.hh"

namespace monteCarlo {
/*----------------------------------------------------------------------------*/
bool FancyGeometry::checkRegion(const std::vector<int> &regionDef ) const
{

	std::vector<int>::const_iterator it = regionDef.begin();

	while(it != regionDef.end()) {
		unsigned int absSurface = std::abs(*it);

		if (absSurface == 0 || absSurface > theSurfaces.size()) {
			return false;
		}
		++it;
	}
	return true;	
}
/*============================================================================*/
unsigned int FancyGeometry::getCellFromPoint(
											const std::vector<double> &location) const
{
	for(unsigned int region = 1; region <= regions.size(); ++region) {
		// loop through every region

		intVec::const_iterator surfsIt = regions[region - 1].begin();

		bool couldBeInRegion = true;

		while(surfsIt != regions[region - 1].end() ) {
			// loop through every surface definition in that region
			// negative surface number --> negative orientation

			couldBeInRegion = couldBeInRegion &&
				((*surfsIt > 0) == getSurface(std::abs(*surfsIt))
										->isLocPositive(location));

			if (!couldBeInRegion) {
				break;
			}
			++surfsIt;
		}
		if (couldBeInRegion) {
			// everything in this region checks out, so this is it
			return region;
		}
	}
	// not found in any region
	return 0;
}

/*============================================================================*/
void FancyGeometry::getIntercept(
							const unsigned int currentCell,
							const std::vector<double> &location,
							const std::vector<double> &direction,
							bool &didHit,
							unsigned int &newCell,
							unsigned int &crossedSurf,
							double &dist) const
{
	Require(currentCell > 0);
	Require(currentCell <= regions.size());

	const intVec &theSurfaces = regions[currentCell - 1];
	//+++++++++ check all surfaces in current cell for intercept
	intVec::const_iterator surfsIt = theSurfaces.begin();
	int theSurf = 0;

	didHit = false;
	newCell = 0;
	crossedSurf = 0;
	dist = std::numeric_limits<double>::infinity();

	while(surfsIt != theSurfaces.end() ) {
		theSurf = *surfsIt;

		std::pair<bool, double> hitAndDist =
			getSurface(std::abs(theSurf))->
				getIntercept( location, direction, (theSurf > 0) );

		if (hitAndDist.first) {
			// we intercept that surface
			dist = hitAndDist.second;
			didHit = true;

			break;
		}
		++surfsIt;
	}

	crossedSurf = std::abs(theSurf);

	//+++++++++ find the other cell that the surface belongs to
	if (didHit == true) {
		unsigned int belongsCount = 0;

		for(unsigned int region = 1; region <= regions.size(); ++region) {
			// loop through every region to make sure this surface doesn't
			// belong to two things

			intVec::const_iterator surfsIt = regions[region - 1].begin();

			while(surfsIt != regions[region - 1].end() ) {
				// loop through every surface definition in that region
				// negative surface number --> negative orientation
				if (*surfsIt == -theSurf) {
					newCell = region;
					++belongsCount;
				}
				++surfsIt;
			}
		}
		Insist(belongsCount == 1, "Geometry too complex to do this!");
	}
}

/*============================================================================*/
FancyGeometry::~FancyGeometry()
{ 
	/* * */ 
}
/*----------------------------------------------------------------------------*/
}
