/*!
 * \file Timer.hpp
 * \brief Timer class and the global singleton that holds it
 * \author Seth R. Johnson
 * 
 * A singleton timer
 *
 */

#ifndef ts_Timer_hh
#define ts_Timer_hh
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <map>
#include <ctime>

using std::string;

namespace tranSupport {

/*----------------------------------------------------------------------------*/
/* \class Timer
 * \brief An individual timer that starts, stops, and can be reset
 * 
 * It knows the startClock
 *
 */
class Timer {
    friend std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& theTimer);

public:
    bool start();
    bool stop();
    bool reset();

    //! return the elapsed time in seconds
    double getElapsedTime() const {
        return elapsedTime;
    }

    // constructor: initialize the timer
    Timer() : isRunning(false), startClock(0), elapsedTime(0)
        { /* * */ }

    //! return the estimated minimum delta_t
    static double TimeResolution() {
        return (1.0 / CLOCKS_PER_SEC);
    }

private:
    bool             isRunning;
    std::clock_t     startClock;
    //! elapsed time in seconds
    double           elapsedTime;
};

/*----------------------------------------------------------------------------*/

//! a singleton class that contains a string-referenced map of many timers
class SuperTimer {

public:
    //! get the only global timer base instance
    static SuperTimer& Instance() {
        static SuperTimer theOnlyTimer;
        
        return theOnlyTimer;
    }

    //! start a timer; return success
    bool startTimer(string timerName);

    //! stop a timer; return success
    bool stopTimer(string timerName);

    //! reset a timer; return success
    bool resetTimer(string timerName);

    //! get the time from a timer in second
    double getTimeForTimer(string timerName);

    //! print all the timers
    void printTimers();

private:
    // hide constructors and assignment operator
    SuperTimer() { /* * */ }
    ~SuperTimer();
    SuperTimer(SuperTimer const&) { /* * */ }
    SuperTimer& operator=(SuperTimer const&) { /* * */ return *this; }

    typedef std::map<string, Timer* >  TimerMap;
    typedef std::pair<string, Timer* > TimerMapPair;

    TimerMap theTimers;
};

//! print a timer to a stream
std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& t);

/*----------------------------------------------------------------------------*/
}
/*============================================================================*/

#ifndef TIMER_START

//! start a new timer
#define TIMER_START(name) \
    tranSupport::SuperTimer::Instance().startTimer(name)
    
//! stop an existing timer that is running
#define TIMER_STOP(name) \
    tranSupport::SuperTimer::Instance().stopTimer(name)

//! clear or reset an existing timer that has run
#define TIMER_RESET(name) \
    tranSupport::SuperTimer::Instance().resetTimer(name)

//! print the timing data
#define TIMER_PRINT() \
    tranSupport::SuperTimer::Instance().printTimers()

#endif


/*----------------------------------------------------------------------------*/
#endif
