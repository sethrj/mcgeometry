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

namespace tranSupport {

/*----------------------------------------------------------------------------*/
/*!
 * \class Timer
 * \brief An individual timer that starts, stops, and can be reset.
 * 
 * It knows the startClock, and subtracts the ending computer clock whenever
 * "stop" is called.
 *
 */
class Timer {
    friend std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& theTimer);

public:
    //! Start the timer. This fails if it has already run or is running.
    bool start();
    //! Stop the timer. This fails if the timer is not running.
    bool stop();
    //! Reset the timer.
    bool reset();

    //! return the elapsed time in seconds
    double getElapsedTime() const {
        return elapsedTime;
    }

    //! constructor: initialize the timer
    Timer() : isRunning(false), startClock(0), elapsedTime(0)
        { /* * */ }

    //! Return the estimated minimum timing resolution.
    static double TimeResolution() {
        return (1.0 / CLOCKS_PER_SEC);
    }

private:
    //! Are we running?
    bool             isRunning;
    //! What time did we start?
    std::clock_t     startClock;
    //! Elapsed time in seconds.
    double           elapsedTime;
};

/*----------------------------------------------------------------------------*/

//! a singleton class that contains a std::string-referenced map of many timers
class SuperTimer {

public:
    //! get the only global timer base instance
    static SuperTimer& Instance() {
        static SuperTimer theOnlyTimer;
        
        return theOnlyTimer;
    }

    //! start a timer; return success
    bool startTimer(std::string timerName);

    //! stop a timer; return success
    bool stopTimer(std::string timerName);

    //! reset a timer; return success
    bool resetTimer(std::string timerName);

    //! Reset all the timers
    void resetAllTimers();

    //! get the time from a timer in second
    double getTimeForTimer(std::string timerName);

    //! print all the timers
    void printTimers();

private:
    //! disallow empty constructor
    SuperTimer() { /* * */ }
    ~SuperTimer();
    //! disallow copy constructor
    SuperTimer(SuperTimer const&) { /* * */ }
    //! disallow assignment
    SuperTimer& operator=(SuperTimer const&) { /* * */ return *this; }

    //! Translate std::string to an actual timer.
    typedef std::map<std::string, Timer* >  TimerMap;

    //! All the timers in the problem.
    TimerMap theTimers;
};

//! Print a timer to a stream.
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
