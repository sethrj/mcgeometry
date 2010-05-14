/*!
 * \file tVectorComp.cpp
 * \brief Unit tests for soft equivalence
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "transupport/VectorComp.hpp"

#include <iostream>
#include <vector>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"

using std::cout;
using std::endl;

using tranSupport::softEquiv;
using tranSupport::hasNoZeros;
using tranSupport::hasNoNegatives;

typedef std::vector<double> doubleVec;
typedef std::vector< doubleVec > doubleTwoDVec;

/*============================================================================*/
void runTests() {
    // ===== check just regular double equivalence ===== //
    TESTER_CHECKFORPASS(!softEquiv(1.0, 0.0));

    TESTER_CHECKFORPASS(!softEquiv(50.0, 50.00001, 1e-7));

    TESTER_CHECKFORPASS(softEquiv(-3.14159, -3.14159));

    TESTER_CHECKFORPASS(softEquiv(0.0, 0.0));
    TESTER_CHECKFORPASS(softEquiv(100.1, 100.0, 0.01));

    double someNumber = 12.0;
    someNumber /= 3; //4
    someNumber += 2; //6
    someNumber /= 2; //3
    someNumber *= 1.5; //4.5

    TESTER_CHECKFORPASS(softEquiv(someNumber, 4.5));

    // ===== check vector of doubles ===== //
    doubleVec targetOneD(3), referenceOneD(3);

    for (int i = 0; i < 3; i++) {
        targetOneD[i]     = i + 1.5;

        referenceOneD[i] = i * 2.0;
        referenceOneD[i] += 3.0;
        referenceOneD[i] /= 2.0;
    }

    TESTER_CHECKFORPASS(softEquiv(targetOneD, referenceOneD));

    // ===== check vector of vector of doubles ===== //
    doubleTwoDVec targetTwoD(4), referenceTwoD(4);

    for (int i = 0; i < 4; i++) {
        targetTwoD[i].resize(3);
        referenceTwoD[i].resize(3);
        for (int j = 0; j < 3; j++) {
            targetTwoD[i][j] = 0.1 + i + 3.0*j;

            referenceTwoD[i][j] = 0.2 + 2.0*i + 1.0;
            referenceTwoD[i][j] += 6.0 * j;
            referenceTwoD[i][j] /= 2.0;
            referenceTwoD[i][j] -= 0.5;

            //            cout    << setw(4) << i << setw(4) << j
            //                    << setw(10) << targetTwoD[i][j]
            //                    << setw(10) <<    referenceTwoD[i][j] << endl;
        }
    }
    TESTER_CHECKFORPASS(softEquiv(targetTwoD, referenceTwoD));

    // ===== make sure it catches an error ===== //
    targetTwoD[2][2] = 99.1;

    TESTER_CHECKFORPASS(!softEquiv(targetTwoD, referenceTwoD));

    // ===== try our checkzeroes ===== //
    TESTER_CHECKFORPASS(hasNoZeros(targetTwoD));
    targetTwoD[2][1] = 0;

    TESTER_CHECKFORPASS(!hasNoZeros(targetTwoD));

    // ===== try our nonnegative ===== //
    TESTER_CHECKFORPASS(hasNoNegatives(targetTwoD));
    targetTwoD[3][2] = -10.0;

    TESTER_CHECKFORPASS(!hasNoNegatives(targetTwoD));
}

/*============================================================================*/
int main(int, char**) {
    TESTER_INIT("VectorComp");
    try {
        runTests();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS(CAUGHT_UNEXPECTED_EXCEPTION);
    }

    TESTER_PRINTRESULT();

    if (!TESTER_HASPASSED()) {
        return 1;
    }

    return 0;
}
