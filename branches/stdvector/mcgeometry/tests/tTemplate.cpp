/*!
 * \file <+TODO+>
 * \brief Unit tests for <+TODO+>
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "mcgeometry/<+header+>.hpp"

#include <iostream>
#include <vector>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/SoftEquiv.hpp"

using namespace mcGeometry;

using std::cout;
using std::endl;

/*============================================================================*/
void runTests() {   

    //   TESTER_CHECKFORPASS(softEquiv(1.0, 1.0));
}

/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("<+File name+>");
    try {
        runTests();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS( CAUGHT_UNEXPECTED_EXCEPTION );
    }

    TESTER_PRINTRESULT();

    return 0;
}
