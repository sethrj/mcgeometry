/*!
 * \file tSuperTimer.cpp
 * \brief Unit tests for the SuperTimer class.
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "transupport/SuperTimer.hpp"

#include <iostream>
#include <ctime>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/VectorComp.hpp"


using std::cout;
using std::endl;
using tranSupport::softEquiv;

using tranSupport::SuperTimer;

/*----------------------------------------------------------------------------*/
void waitTime( double seconds ) {
	using std::clock;
	std::clock_t endwaitTime;
	endwaitTime = clock() + static_cast<std::clock_t>(seconds * CLOCKS_PER_SEC);

	while (clock() < endwaitTime) {
		;
	}
}

/*----------------------------------------------------------------------------*/
/* 
 * This subroutine will test make sure unit tester and timer are global
 */
void someSubroutine() {
	SuperTimer &theTimer = SuperTimer::Instance();

	bool successfulGetTime = true;
	try {
		TESTER_CHECKFORPASS(softEquiv(theTimer.getTimeForTimer("FifthSecond"),
				  	0.2, 1e-3) );
	} 
	catch (tranSupport::tranError &theErr) {
		cout 	<< "Caught error:" << endl
				<< theErr.what();
		successfulGetTime = false;
	}
	TESTER_CHECKFORPASS(successfulGetTime);
}

/*----------------------------------------------------------------------------*/
void runTests ()
{
	// get a reference to the global timer object
	SuperTimer &theTimer = SuperTimer::Instance();
	double timeTolerance = tranSupport::Timer::TimeResolution()*1000.0;

	//cout << "Timing resolution is " << tranSupport::Timer::TimeResolution()
	//	  << " seconds" << endl;
	// theTimer.printTimers();

	// ===== check that a second is roughly a second ===== //
	theTimer.startTimer("FifthSecond");
	waitTime(0.2);
	theTimer.stopTimer("FifthSecond");
	TESTER_CHECKFORPASS(
			softEquiv(theTimer.getTimeForTimer("FifthSecond"),
						 0.2, timeTolerance));
	
	// ===== this should start and stop really quickly ===== //
	TIMER_START("Quick");
	TIMER_STOP("Quick");
	TESTER_CHECKFORPASS(
			softEquiv(theTimer.getTimeForTimer("Quick"),
				0.0, timeTolerance));

	// ===== try to stop a timer that's already stopped ===== //
	bool caughtError = false;

	try {
		TIMER_STOP("FifthSecond");
	} 
	catch (tranSupport::tranError &theErr) {
		//cout 	<< "Caught error:" << endl
		//		<< theErr.what();
		caughtError = true;
	}
	TESTER_CHECKFORPASS(caughtError);

	// ===== try to stop a nonexistent timer ===== //
	caughtError = false;

	try {
		TIMER_STOP("Nonexistent!");
	} 
	catch (tranSupport::tranError &theErr) {
		caughtError = true;
	}
	TESTER_CHECKFORPASS(caughtError);

	// ===== reset a timer ===== //
	bool successfulTrial = true;

	TIMER_START("Reset");
	waitTime(0.01);
	TIMER_STOP("Reset");

	try {
		TIMER_RESET("Reset");
	} 
	catch (tranSupport::tranError &theErr) {
		cout 	<< "Unexpected error:" << endl
				<< theErr.what();
		successfulTrial = false;
	}
	TESTER_CHECKFORPASS(successfulTrial);

	TESTER_CHECKFORPASS(theTimer.getTimeForTimer("Reset") == -1.0);

	// ===== try to stop a nonexistent timer  ===== //
	caughtError = false;

	try {
		TIMER_STOP("Nonexistent");
    } 
    catch (tranSupport::tranError &theErr) {
        caughtError = true;
    }
    TESTER_CHECKFORPASS(caughtError);
}
/*============================================================================*/
int main(int argc, char *argv[]) {
    TESTER_INIT("SuperTimer");
    try {
        runTests();

        // ===== make sure the timer is global ===== //
        someSubroutine();
    }
    catch (tranSupport::tranError &theErr) {
        cout << "UNEXPECTED ERROR IN UNIT TEST: " << endl
            << theErr.what() << endl;
        TESTER_CHECKFORPASS(CAUGHT_UNEXPECTED_EXCEPTION);
    }

    TESTER_PRINTRESULT();

    return 0;
}

