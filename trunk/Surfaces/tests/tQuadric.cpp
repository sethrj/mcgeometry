/*!
 * \file tQuadric.cc
 * \brief Test the Quadric geometry capability
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "Surfaces/Quadric.hpp"
#include "Surfaces/Sphere.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"
#include "transupport/VectorPrint.hpp"

using std::cout;
using std::endl;


typedef std::vector<double> doubleVec;

/*============================================================================*/
void runTests() {
	doubleVec sphCenter(3,0.0);
	double 	 sphRadius = 0.0;
	doubleVec particleLoc(3,0.0);
	doubleVec particleDir(3,0.0);
	bool isParticlePositiveWrtSurface;

	Quadric::HitAndDist theResult;

	sphRadius = 2.0;
	sphCenter[0] = 1.0;

	particleLoc[0] = 1.5;
	particleDir[1] = 1.0;

	isParticlePositiveWrtSurface = false;

//	cout << "sphere center: " << sphCenter << " radius: " << sphRadius << endl;

	Sphere	theSphere(sphCenter, sphRadius);

//	cout << "particle loc: " << particleLoc
//			<< " direction: " << particleDir << endl;

	theResult = theSphere.intersect(particleLoc, particleDir,
			isParticlePositiveWrtSurface);

//	cout << "Did hit: " << theResult.first << endl
//		<< "distance: " << theResult.second << endl;

//	TESTER_CHECKFORPASS(theSphere.isLocPositive(particleLoc) == false);
	TESTER_CHECKFORPASS(theResult.first == true);
	TESTER_CHECKFORPASS(softEquiv(theResult.second, 1.936491673103709));

	/********************/

	particleLoc[0] = -1;
	particleLoc[1] = -1;
	particleLoc[2] = 0.5;

	particleDir[0] = 0.707106781186547;
	particleDir[1] = 0.707106781186547;
	particleDir[2] = 0.0;

	isParticlePositiveWrtSurface = true;

	theResult = theSphere.intersect(particleLoc, particleDir,
			isParticlePositiveWrtSurface);


//	cout << "Did hit: " << theResult.first << endl
//		<< "distance: " << theResult.second << endl;

//	TESTER_CHECKFORPASS(theSphere.isLocPositive(particleLoc) == true);
	TESTER_CHECKFORPASS(theResult.first == true);
	TESTER_CHECKFORPASS(softEquiv(theResult.second, 0.318544705827648));

	/********************/

	particleDir[0] = 0.707106781186547;
	particleDir[1] = -0.707106781186547;
	particleDir[2] = 0.0;

	theResult = theSphere.intersect(particleLoc, particleDir,
			isParticlePositiveWrtSurface);

	TESTER_CHECKFORPASS(theResult.first == false);
}
/*============================================================================*/
int main(int argc, char *argv[]) {
	TESTER_INIT("Quadric");
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
