/*!
 * \file Timer.hpp
 * \brief Contains code for class \c Timer
 * \author Seth R. Johnson
 */

#ifndef ts_Timer_hh
#define ts_Timer_hh
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <sstream>
#include <iomanip>
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
    //! Possible values for the status of the timer
    enum TimerStatus {
        NOTYETRUN = 0,  //!< Timer hasn't yet been run
        RUNNING,  //!< Timer is now running
        FINISHED  //!< Timer has run and stopped
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

/*============================================================================*/
//! Print a timer to a stream.
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

/*----------------------------------------------------------------------------*/
#endif
