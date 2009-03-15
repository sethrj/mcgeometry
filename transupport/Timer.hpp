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
#include <sstream>
#include <iomanip>
#include <map>
#include <list>
#include <ctime>
#include "dbc.hpp"

namespace tranSupport {
/*============================================================================*/
/*!
 * \class Timer
 * \brief An individual timer that starts, stops, and can be reset.
 * 
 * It knows the startClock, and subtracts the ending computer clock whenever
 * "stop" is called. It stores accumulated times.
 *
 * Note that on some systems, std::clock() returns a number relative to the
 * start of the program, and the timing resolution may be arbitrarily coarse, so
 * a timer started at the beginning and stopped soon after may show a startClock
 * of zero and a total time of zero, even though it's actually been run.
 *
 */
class Timer {
public:
    enum TimerStatus {
        NOTYETRUN = 0, 
        RUNNING,
        FINISHED
    };
public:

    //! Return the estimated minimum timing resolution.
    static double TimeResolution() {
        return (1.0 / CLOCKS_PER_SEC);
    }

    /*------------------------------------------------------------*/
    //! \name Timer control
    //\{
    //! Start the timer. This fails if it has already run or is running.
    void start()
    {
        Insist(runStatus != RUNNING,
                "Can't start a timer while it is running.");

        runStatus = RUNNING;
        startClock = std::clock();
    }
        
    //! Stop the timer. This fails if the timer is not running.
    void stop()
    {
        Insist(runStatus == RUNNING,
                "Can't stop a timer while that is not running.");

        std::clock_t stopClock = std::clock();

        // += to increment the time; = to reset the time
        elapsedTime += (stopClock - startClock) / (1.0 * CLOCKS_PER_SEC);

        runStatus = FINISHED;
    }
    
    //! Reset the timer.
    void reset()
    {
        Insist(runStatus == FINISHED,
                "Can't reset a timer while that has not finished running.");

        runStatus  = NOTYETRUN;
        startClock = 0;
        elapsedTime = 0.0;
    }
        
    //\}
    /*------------------------------------------------------------*/
    //! \name ACCESSORS
    //\{

    //! return the elapsed time in seconds
    double getElapsedTime() const {
        Insist(runStatus != RUNNING, "Calling getElapsedTime on a timer"
                " that is still running");
        if (runStatus == NOTYETRUN) {
            return -1.0;
        }

        return elapsedTime;
    }

    //! return the run status
    TimerStatus getRunStatus() const {
        return runStatus;
    }
    //\}
    /*------------------------------------------------------------*/

    //! constructor: initialize the timer
    Timer() : runStatus(NOTYETRUN), startClock(0), elapsedTime(0)
    { /* * */ }

private:
    //! Are we running?
    Timer::TimerStatus runStatus;
    //! What time did we start?
    std::clock_t     startClock;
    //! Elapsed time in seconds.
    double           elapsedTime;
};
} //end namespace tranSupport

//! Print a timer to a stream.
std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& t);

namespace tranSupport {
/*============================================================================*/

/*! \class SuperTimer
 *  \brief A singleton class that contains a std::string-referenced map of many
 *          timers.
 *
 */
class SuperTimer {
    //! Translate std::string to an actual timer.
    typedef std::map<std::string, Timer* >  MapTimer;
    //! List of timers for ordered printing purposes. 
    typedef std::list< Timer* >  ListTimer;
public:

    //! Get the only global timer base instance
    static SuperTimer& Instance() {
        static SuperTimer theOnlyTimer;
        
        return theOnlyTimer;
    }

    /*------------------------------------------------------------*/
    //! \name Timer control
    //\{

    //! Start a timer; return success
    void startTimer(const std::string timerName)
    {
        MapTimer::iterator it = theTimers.find(timerName);

        if (it == theTimers.end()) {
            // no timer with this name exists, so make a new one
            Timer *newTimer = new Timer();
            
            // add it to the map of timers for start/stop purposes
            theTimers.insert(std::make_pair(timerName, newTimer));

            // add it to the list for ordering purposes
            //orderedTimers.push_back(newTimer);

            // start the time
            newTimer->start();
        } else {
            // it already exists; start it
            it->second->start();
        }
    }

    //! Stop a timer; return success
    void stopTimer(const std::string timerName)
    {
        MapTimer::iterator it = theTimers.find(timerName);

        Insist(it != theTimers.end(), "Timer not found");

        // stop the timer, verify that it was working
        it->second->stop();
    }

    //! Reset a timer; return success
    void resetTimer(const std::string timerName)
    {
        MapTimer::iterator it = theTimers.find(timerName);

        Insist(it != theTimers.end(), "Timer not found");

        // stop the timer, verify that it was working
        it->second->reset();
    }

    //! Reset all the timers
    void resetAllTimers()
    {
        MapTimer::iterator it;

        for (it = theTimers.begin(); it != theTimers.end(); it++) {
            if ((it->second)->getRunStatus() == Timer::FINISHED) 
                (it->second)->reset();
        }
    }
        
    //\}
    /*------------------------------------------------------------*/
    //! \name Information access
    //\{

    //! Get the time (in seconds) from a timer
    double getTimeForTimer(std::string timerName) const
    {
        MapTimer::const_iterator it = theTimers.find(timerName);

        Insist(it != theTimers.end(), "Timer not found");

        return it->second->getElapsedTime();
    }

    //! Print all the timers
    void printTimers() const;
    //\}
    /*------------------------------------------------------------*/
private:
    //! disallow construction from elsewhere
    SuperTimer() {/* * */}
    //! disallow copy constructor
    SuperTimer(SuperTimer const&) { /* * */ }
    //! disallow assignment
    SuperTimer& operator=(SuperTimer const&) { return *this; }

    //! destructor
    ~SuperTimer()
    {
        // Iterator is a key (string)/value (timer pointer) pair
        for (MapTimer::iterator it = theTimers.begin();
                it != theTimers.end(); it++)
        {
            delete it->second;
        }
    }

private:
    //! All the timers in the problem.
    MapTimer theTimers;
};
/*============================================================================*/
inline void SuperTimer::printTimers() const
{
    using std::cout;
    using std::endl;
    MapTimer::const_iterator it;

    cout << "----------------------------------------"
            "-----------------------------------" << endl;
    cout << "TIMING DATA:                             Name";
    cout.width(30);
    cout << "Time" << endl;
    cout << "----------------------------------------"
            "-----------------------------------" << endl;

    for (it = theTimers.begin(); it != theTimers.end(); it++) {
        std::string     timerName    = it->first;
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
//! If timers are off, define TIMER_PRINT to call this
inline void printTimersDisabledMessage()
{
    std::cout << "----------------------------------------"
                 "-----------------------------------" << std::endl;
    std::cout << "NOTICE: timer will not print because timers are disabled."
              << std::endl
              << "Leave off the -DNOTIMERS option to turn timers on."
              << std::endl;
    std::cout << "----------------------------------------"
                 "-----------------------------------" << std::endl;
}
/*----------------------------------------------------------------------------*/
}
/*============================================================================*/
inline std::ostream& operator<<(std::ostream& os, const tranSupport::Timer& t)
{
    if (t.getRunStatus() == tranSupport::Timer::RUNNING) {
        os << "STILL RUNNING";
    } else if (t.getRunStatus() == tranSupport::Timer::NOTYETRUN) {
        os << "NOT YET RUN";
    } else {
        // need to make an intermediate string in case we had just done
        // setwidth or something (unless there is a better way I don't
        // know about)
        std::ostringstream temp;

        temp  << std::setprecision(3) << std::setiosflags(std::ios::fixed)
              << t.getElapsedTime() * 1000 << "ms";

        os << temp.str();
    }

    return os;
}
/*============================================================================*/

#ifndef NOTIMERS
// Timers are enabled
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

#else
// Disable timers!
#define TIMER_START(name)
#define TIMER_STOP(name)
#define TIMER_RESET(name)
#define TIMER_PRINT(name) tranSupport::printTimersDisabledMessage()

#endif

/*----------------------------------------------------------------------------*/
#endif
