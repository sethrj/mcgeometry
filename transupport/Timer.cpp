/*!
 * \file Timer.cpp
 * \brief Timer classes
 * \author Seth R. Johnson
 * 
 * A singleton timer class 
 *
 */

/*----------------------------------------------------------------------------*/

#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Timer.hpp"
#include "dbc.hpp"

using std::string;
using std::cout;
using std::endl;
using std::ostringstream;

namespace tranSupport {

/*----------------------------------------------------------------------------*/
bool SuperTimer::startTimer(string timerName) {
    TimerMap::iterator it = theTimers.find(timerName);

    if (it == theTimers.end()) {
        // no timer with this name exists, so make a new one
        Timer *newTimer = new Timer();
        
        // add it to our list of timers
        theTimers.insert(std::make_pair(timerName, newTimer));

        // start the time
        newTimer->start();

        return true;
    } else {
        bool success = it->second->start();

        if (!success) {
            ostringstream errorString;

            errorString << "error while starting timer \"" << timerName
                            << "\": " << *(it->second) << endl;
            Insist(0, errorString.str().c_str());
        }
    }

    return false;
}

/*----------------------------------------------------------------------------*/
bool SuperTimer::stopTimer(string timerName) {
    TimerMap::iterator it = theTimers.find(timerName);

    if (it == theTimers.end()) {
        ostringstream errorString;

        errorString << "timer \"" << timerName << "\" was not found";
        Insist(0, errorString.str().c_str());

        return false;
    }

    // stop the timer, verify that it was working
    bool stoppedResult;
    stoppedResult = it->second->stop();
    if (!stoppedResult) {
        ostringstream errorString;

        errorString << "error while stopping timer \"" << timerName
                        << "\": " << *(it->second) << endl;
        Insist(0, errorString.str().c_str());

        return false;
    }

    return true;
}

/*----------------------------------------------------------------------------*/
bool SuperTimer::resetTimer(string timerName) {
    TimerMap::iterator it = theTimers.find(timerName);

    if (it == theTimers.end()) {
        ostringstream errorString;

        errorString << "timer \"" << timerName << "\" was not found";
        Insist(0, errorString.str().c_str());

        return false;
    }

    // stop the timer, verify that it was working
    bool stoppedResult;
    stoppedResult = it->second->reset();
    if (!stoppedResult) {
        ostringstream errorString;

        errorString << "error while resetting timer \"" << timerName
                        << "\": " << *(it->second) << endl;

        Insist(0, errorString.str().c_str());

        return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
void SuperTimer::resetAllTimers()
{
    TimerMap::iterator it;

    bool success = true;
    for (it = theTimers.begin(); it != theTimers.end(); it++) {
        Timer& curTimer = *(it->second);
        success = success && curTimer.reset();
    }

    if (!success) {
        cout << "An error occurred while resetting the timers." << endl;
    }
}


/*----------------------------------------------------------------------------*/
double SuperTimer::getTimeForTimer(string timerName) {
    TimerMap::iterator it = theTimers.find(timerName);

    if (it == theTimers.end()) {
        ostringstream errorString;

        errorString << "timer \"" << timerName << "\" was not found";
        Insist(0, errorString.str().c_str());

        return -1.0;
    }

    return it->second->getElapsedTime();
}

/*----------------------------------------------------------------------------*/
void SuperTimer::printTimers() {
    TimerMap::iterator it;

    cout << "----------------------------------------"
         << "-----------------------------------" << endl;
    cout << "TIMING DATA:                             Name";
    cout.width(30);
    cout << "Time" << endl;
    cout << "----------------------------------------"
         << "-----------------------------------" << endl;

    for (it = theTimers.begin(); it != theTimers.end(); it++) {
        string     timerName    = it->first;
        Timer *curTimer        = it->second;
        
        cout.width(50);
        cout << timerName;
        cout.width(25);
        cout << *curTimer << endl;
    }
    cout << "----------------------------------------"
         << "-----------------------------------" << endl;
}

/*----------------------------------------------------------------------------*/
SuperTimer::~SuperTimer() {
    TimerMap::iterator it;
    for (it = theTimers.begin(); it != theTimers.end(); it++) {
        Timer *curTimer        = it->second;

        delete curTimer;
    }
}

/*============================================================================*/
bool Timer::start() {

    if (runStatus) {
        // can't start it while it's running!

        return false;
    } else if (startClock != 0 || elapsedTime != 0.0) {
        // it looks like the thing has not been reset
        // so return false

        return false;
    }

    runStatus = RUNNING;
    startClock = std::clock();

    return true;
}

bool Timer::stop() {
    if (runStatus == RUNNING) {
        std::clock_t stopClock = std::clock();

        elapsedTime = (stopClock - startClock) / (1.0 * CLOCKS_PER_SEC);

        runStatus = FINISHED;
    } else {
    //    cout << "Whoops, tried to stop a timer that was already stopped." 
    //          << endl;
        return false;
    }
    return true;
}

bool Timer::reset() {
    if (runStatus == RUNNING ) {
        // should not be called during run, but reset it anyway
        // and return false to let someone know we're not happy
        runStatus  = NOTYETRUN;
        startClock = 0;
        elapsedTime = 0.0;

        return false;
    } else if (runStatus == NOTYETRUN) {
        // something's wrong if we try to reset a timer that's never been run
        return false;
    }

    runStatus  = NOTYETRUN;
    startClock = 0;
    elapsedTime = 0.0;
        
    return true;
}

std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& t)
{
    if (t.getRunStatus() == tranSupport::Timer::RUNNING) {
        os << "STILL RUNNING";
    } else if (t.getRunStatus() == tranSupport::Timer::NOTYETRUN) {
        os << "NOT YET RUN";
    } else {
        // need to make an intermediate string in case we had just done
        // setwidth or something (unless there is a better way I don't know about)
        ostringstream temp;

        temp << std::setprecision(3) << std::setiosflags(std::ios::fixed)
              << t.getElapsedTime() * 1000 << "ms";

        os << temp.str();
    }

    return os;
}

/*----------------------------------------------------------------------------*/
}

/*----------------------------------------------------------------------------*/
