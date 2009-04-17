/*!
 * \file SuperTimer.hpp
 * \brief Contains code for class \c SuperTimer
 * \author Seth R. Johnson
 */

#ifndef ts_SuperTimer_hh
#define ts_SuperTimer_hh
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <iomanip>
#include <map>

#include "dbc.hpp"
#include "Timer.hpp"

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
public:

    /*! \brief Get the only global timer base instance
     *
     *  \warning This is not multi-thread safe! In fact, this has been known to
     *  fail to share timers between Python and C++ when using SWIG on AMD64
     *  red hat systems.
     */
    static SuperTimer& Instance()
    {
        static SuperTimer theOnlyTimer;
        
        return theOnlyTimer;
    }

    /*------------------------------------------------------------*/
    //! \name Timer control
    //\{

    //! Start a timer; return success
    void startTimer(const std::string& timerName)
    {
        MapTimer::iterator it = theTimers.find(timerName);

        if (it == theTimers.end()) {
            // no timer with this name exists, so make a new one
            Timer *newTimer = new Timer();
            
            // add it to the map of timers for start/stop purposes
            theTimers.insert(std::make_pair(timerName, newTimer));

            // start the time
            newTimer->start();
        } else {
            // it already exists; start it
            it->second->start();
        }
    }

    //! Stop a timer; return success
    void stopTimer(const std::string& timerName)
    {
        MapTimer::iterator it = theTimers.find(timerName);

        Insist(it != theTimers.end(), "Timer not found");

        // stop the timer, verify that it was working
        it->second->stop();
    }

    //! Reset a timer; return success
    void resetTimer(const std::string& timerName)
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

        for (it = theTimers.begin(); it != theTimers.end(); ++it) {
            if ((it->second)->getRunStatus() == Timer::FINISHED) 
                (it->second)->reset();
        }
    }
        
    //\}
    /*------------------------------------------------------------*/
    //! \name Information access
    //\{

    //! Get the time (in seconds) from a timer
    double getTimeForTimer(const std::string& timerName) const
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
    //! Disallow construction from elsewhere
    SuperTimer() {/* * */}
    //! Disallow copy constructor
    SuperTimer(SuperTimer const&) { /* * */ }
    //! Disallow assignment
    SuperTimer& operator=(SuperTimer const&) { return *this; }

    //! Destructor deletes all timer instances
    ~SuperTimer()
    {
        // Iterator is a key (string)/value (timer pointer) pair
        for (MapTimer::iterator it = theTimers.begin();
                it != theTimers.end(); ++it)
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
//    cout << "[Reported time resolution: " << Timer::TimeResolution() * 1000
//         << " ms]" << endl;
    cout << "TIMING DATA:                             Name";
    cout.width(30);
    cout << "Time" << endl;
    cout << "----------------------------------------"
            "-----------------------------------" << endl;

    for (it = theTimers.begin(); it != theTimers.end(); ++it)
    {
        const std::string& timerName  = it->first;
        const Timer*  curTimer        = it->second;
        
        cout.width(50);
        cout << timerName;
        cout.width(25);
        cout << *curTimer << endl;
    }
    cout << "----------------------------------------"
         << "-----------------------------------" << endl;
}
/*----------------------------------------------------------------------------*/
} //end namespace tranSupport
/*============================================================================*/

#ifndef TIMER_START
// Timers have not yet been defined
#   ifndef NOTIMERS
    // Timers are enabled
        //! Start a new timer
#       define TIMER_START(name) \
            tranSupport::SuperTimer::Instance().startTimer(name)
    
        //! Stop an existing timer that is running
#       define TIMER_STOP(name) \
            tranSupport::SuperTimer::Instance().stopTimer(name)

        //! Clear or reset an existing timer that has run
#       define TIMER_RESET(name) \
            tranSupport::SuperTimer::Instance().resetTimer(name)

        //! Print the timing data
#       define TIMER_PRINT() \
            tranSupport::SuperTimer::Instance().printTimers()
#   else
    // Disable timers!
        //! If timers are off, define TIMER_PRINT to call this
        namespace tranSupport {
        inline void printTimersDisabledMessage()
        {
            std::cout << "----------------------------------------"
                         "-----------------------------------" << std::endl;
            std::cout << "NOTICE: timer will not print because timers"
                         "are disabled."
                      << std::endl
                      << "Leave off the -DNOTIMERS option to turn timers on."
                      << std::endl;
            std::cout << "----------------------------------------"
                         "-----------------------------------" << std::endl;
        }
        }
#       define TIMER_START(name)
#       define TIMER_STOP(name)
#       define TIMER_RESET(name)
#       define TIMER_PRINT() tranSupport::printTimersDisabledMessage()
#   endif
#else
// Timers have already been defined
#    warning "TIMER has already been defined. This probably means" \
             " that you're including both SuperTimer and FastTimer."
#endif


/*----------------------------------------------------------------------------*/
#endif
