/*!
 * \file FastTimer.hpp
 * \brief Contains code for class \c FastTimer
 * \author Seth R. Johnson
 */

#ifndef ts_FastTimer_hh
#define ts_FastTimer_hh
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <ctime>

#include "dbc.hpp"
#include "Timer.hpp"

namespace tranSupport {
/*============================================================================*/

/*! \class FastTimer
 *  \brief A singleton class that uses indices rather than maps to
 *  store the timers, for fast access.
 *
 *  Note that this may be about as fast as you can get a timer class to be, but
 *  it's still relatively slow. 
 *
 *  For example, considering an IMC problem with or without timers around
 *  every particle lifespan (300000 particles):
 *   - With SuperTimer: ~17 seconds
 *   - With FastTimer: ~9 seconds
 *   - Without timers: ~3  seconds
 */
class FastTimer {
    //! A vector of pointers to timers
    typedef std::vector< Timer >  VecTimer;
    //! A vector of strings for timer names 
    typedef std::vector< std::string >  VecString;
public:

    //! Get the only global timer base instance
    static FastTimer& Instance() {
        static FastTimer theOnlyTimer;
        
        return theOnlyTimer;
    }

    //! Initialize timers
    void initTimers(unsigned int numTimers)
    {
        theTimers.resize(numTimers);
        timerNames.assign(numTimers,"UNTITLED");
    }

    //! Set timer names
    void initTimerName(unsigned int timerIndex,
                       const std::string& newTimerName)
    {
        Insist(timerIndex < timerNames.size(), 
                "Tried to assign a timer name out of bounds.");
        timerNames[timerIndex] = newTimerName;
    }

    /*------------------------------------------------------------*/
    //! \name Timer control
    //\{

    //! Start a timer; return success
    void startTimer(unsigned int timerIndex)
    {
        Check(timerIndex < theTimers.size());
        theTimers[timerIndex].start();
    }

    //! Stop a timer; return success
    void stopTimer(unsigned int timerIndex)
    {
        Check(timerIndex < theTimers.size());
        theTimers[timerIndex].stop();
    }

    //! Reset a timer; return success
    void resetTimer(unsigned int timerIndex)
    {
        Check(timerIndex < theTimers.size());
        theTimers[timerIndex].reset();
    }

    //! Reset all the timers
    void resetAllTimers()
    {
        VecTimer::iterator it;

        for (it = theTimers.begin(); it != theTimers.end(); ++it) {
            if (it->getRunStatus() == Timer::FINISHED) 
                it->reset();
        }
    }
        
    //\}
    /*------------------------------------------------------------*/
    //! \name Information access
    //\{

    //! Get the time (in seconds) from a timer
    double getTimeForTimer(unsigned int timerIndex) const
    {
        Check(timerIndex < theTimers.size());
        return theTimers[timerIndex].getElapsedTime();
    }

    //! Print all the timers
    void printTimers() const;
    //\}
    /*------------------------------------------------------------*/
private:
    //! disallow construction from elsewhere
    FastTimer() {/* * */}
    //! disallow copy constructor
    FastTimer(FastTimer const&) { /* * */ }
    //! disallow assignment
    FastTimer& operator=(FastTimer const&) { return *this; }

private:
    //! All the timers in the problem.
    VecTimer theTimers;
    //! Names for each timer
    VecString timerNames;
};
/*============================================================================*/
inline void FastTimer::printTimers() const
{
    using std::cout;
    using std::endl;
    VecTimer::const_iterator it;

    cout << "----------------------------------------"
            "-----------------------------------" << endl;
    cout << "TIMING DATA:                             Name";
    cout.width(30);
    cout << "Time" << endl;
    cout << "----------------------------------------"
            "-----------------------------------" << endl;


    VecString::const_iterator stringIt = timerNames.begin();
    for (it = theTimers.begin(); it != theTimers.end(); ++it)
    {
        cout.width(50);
        cout << *stringIt; //print the label
        cout.width(25);
        cout << *it << endl; //print the timer

        ++stringIt;
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
        //! Initialize with the right number of timers 
#       define TIMER_INIT(numTimers) \
            tranSupport::FastTimer::Instance().initTimers(numTimers)
        //! Initialize with the right number of timers 
#       define TIMER_SETNAME(index, name) \
            tranSupport::FastTimer::Instance().initTimerName(index, name)

        //! Start a new timer
#       define TIMER_START(index) \
            tranSupport::FastTimer::Instance().startTimer(index)
    
        //! Stop an existing timer that is running
#       define TIMER_STOP(index) \
            tranSupport::FastTimer::Instance().stopTimer(index)

        //! Clear or reset an existing timer that has run
#       define TIMER_RESET(index) \
            tranSupport::FastTimer::Instance().resetTimer(index)

        //! Print the timing data
#       define TIMER_PRINT() \
            tranSupport::FastTimer::Instance().printTimers()
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
#       define TIMER_START(index)
#       define TIMER_STOP(index)
#       define TIMER_RESET(index)
#       define TIMER_PRINT() tranSupport::printTimersDisabledMessage()
#   endif
#else
// Timers have already been defined
#    warning "TIMER has already been defined. This probably means" \
             " that you're including both SuperTimer and FastTimer."
#endif


/*----------------------------------------------------------------------------*/
#endif
