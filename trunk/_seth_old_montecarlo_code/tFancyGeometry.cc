/*!
 * \file tFancyGeometry.cc
 * \brief Unit tests for FancyGeometry
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "montecarlo/FancyGeometry.hh"

#include <iostream>
#include <vector>
#include "transupport/dbc.hh"
#include "transupport/UnitTester.hh"
#include "transupport/SoftEquiv.hh"
#include "montecarlo/Quadric.hh"
#include <boost/tr1/memory.hpp>

using std::cout;
using std::endl;

using monteCarlo::Sphere;

typedef std::tr1::shared_ptr<const monteCarlo::Quadric> spCQuadric;
typedef std::vector<double> doubleVec;
typedef std::vector<int> intVec;

/*============================================================================*/
void makeGeometry(monteCarlo::FancyGeometry& geom) {

	spCQuadric innerSphere(new const Sphere(1.0));
	unsigned int innerSurfInd, outerSurfInd;

	innerSurfInd = geom.addSurface(innerSphere);

	spCQuadric outerSphere(new const Sphere(2.0));

	outerSurfInd = geom.addSurface(outerSphere);

	// define regions
	intVec regionVec(1,0);

	regionVec[0] = -innerSurfInd;
	geom.addRegion(regionVec);

	regionVec.resize(2);
	regionVec[0] = innerSurfInd;
	regionVec[1] = -outerSurfInd;

	geom.addRegion(regionVec);

	regionVec.resize(1);
	regionVec[0] = outerSurfInd;

	unsigned int outerReg = geom.addRegion(regionVec);

	geom.setDeadRegion(outerReg);
}
/*============================================================================*/
void runTests() {	
	monteCarlo::FancyGeometry geom;

	makeGeometry(geom);

	doubleVec aPoint(3,0.0);
	aPoint[1] = 1.5;
	
	TESTER_CHECKFORPASS(geom.getSurface(1)->isLocPositive(aPoint));
	TESTER_CHECKFORPASS(!geom.getSurface(2)->isLocPositive(aPoint));

	/* ------------- */
	aPoint[1] = 0.5;
	TESTER_CHECKFORPASS(geom.getCellFromPoint(aPoint) == 1);
	aPoint[1] = 1.5;
	TESTER_CHECKFORPASS(geom.getCellFromPoint(aPoint) == 2);
	aPoint[1] = 2.5;
	TESTER_CHECKFORPASS(geom.getCellFromPoint(aPoint) == 3);

	TESTER_CHECKFORPASS(geom.isDeadRegion(3));
	/* ------------- */
	doubleVec aDirection(3,0.0);
	aDirection[1] = 1.0;
	aPoint[1] = 0.5;

	bool didHit;
	unsigned int newCell;
	unsigned int theSurf;
	double distance;

	geom.getIntercept(1, aPoint, aDirection, didHit, newCell, theSurf, distance);
	TESTER_CHECKFORPASS(didHit == true);
	TESTER_CHECKFORPASS(newCell == 2);
	TESTER_CHECKFORPASS(theSurf == 1);
	TESTER_CHECKFORPASS(softEquiv(distance, 0.5));
}

/*============================================================================*/
int main(int argc, char *argv[]) {
	TESTER_INIT("FancyGeometry");
	try {
		runTests();
	}
	catch (tranSupport::tranError &theErr) {
		cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
			<< theErr.what() << endl;
		TESTER_CHECKFORPASS(false);
	}

	TESTER_PRINTRESULT();
	
	return 0;
}
