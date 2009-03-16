/*!
 * \file tFastTimer.cpp
 * \brief Unit tests for the FastTimer class.
 * \author Seth R. Johnson
 */

/*----------------------------------------------------------------------------*/

#include "transupport/FastTimer.hpp"

#include <iostream>
#include <ctime>
#include "transupport/dbc.hpp"
#include "transupport/UnitTester.hpp"
#include "transupport/VectorComp.hpp"


using std::cout;
using std::endl;
using tranSupport::softEquiv;

using tranSupport::FastTimer;


#define TIMERNAME_FifthSecond 0
#define TIMERNAME_Quick       1
#define TIMERNAME_Reset       2

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
	FastTimer &theTimer = FastTimer::Instance();

	bool successfulGetTime = true;
	try {
		TESTER_CHECKFORPASS(
                softEquiv(theTimer.getTimeForTimer(TIMERNAME_FifthSecond),
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
void runTests()
{
	// get a reference to the global timer object
	FastTimer &theTimer = FastTimer::Instance();
	double timeTolerance = tranSupport::Timer::TimeResolution()*1000.0;


    // initialize fast timer
    theTimer.initTimers(3);
    theTimer.initTimerName(TIMERNAME_FifthSecond, "FifthSecond");
    theTimer.initTimerName(TIMERNAME_Quick, "Quick");
    theTimer.initTimerName(TIMERNAME_Reset, "Reset");

	// ===== check that a second is roughly a second ===== //
	theTimer.startTimer(TIMERNAME_FifthSecond);
	waitTime(0.2);
	theTimer.stopTimer(TIMERNAME_FifthSecond);
	TESTER_CHECKFORPASS(
			softEquiv(theTimer.getTimeForTimer(TIMERNAME_FifthSecond),
						 0.2, timeTolerance));
	
	// ===== this should start and stop really quickly ===== //
	TIMER_START(TIMERNAME_Quick);
	TIMER_STOP(TIMERNAME_Quick);
	TESTER_CHECKFORPASS(
			softEquiv(theTimer.getTimeForTimer(TIMERNAME_Quick),
				0.0, timeTolerance));

	// ===== try to stop a timer that's already stopped ===== //
	bool caughtError = false;

	try {
		TIMER_STOP(TIMERNAME_FifthSecond);
	} 
	catch (tranSupport::tranError &theErr) {
		//cout 	<< "Caught error:" << endl
		//		<< theErr.what();
		caughtError = true;
	}
	TESTER_CHECKFORPASS(caughtError);


	// ===== reset a timer ===== //
	bool successfulTrial = true;

	TIMER_START(TIMERNAME_Reset);
	waitTime(0.01);
	TIMER_STOP(TIMERNAME_Reset);

    TIMER_PRINT();
	try {
		TIMER_RESET(TIMERNAME_Reset);
	} 
	catch (tranSupport::tranError &theErr) {
		cout 	<< "Unexpected error:" << endl
				<< theErr.what();
		successfulTrial = false;
	}
	TESTER_CHECKFORPASS(successfulTrial);

	TESTER_CHECKFORPASS(theTimer.getTimeForTimer(TIMERNAME_Reset) == -1.0);
	TESTER_CHECKFORPASS(theTimer.getTimeForTimer(TIMERNAME_Reset) == -1.0);
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

