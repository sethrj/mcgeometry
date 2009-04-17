/*!
 * \file BasicTally.hpp
 * \brief A basic tally that collects a statistic and returns mean etc. 
 * \author Seth R. Johnson
 * 
 * This tally takes some $x$ of type $T$ and returns $\overline{x}$
 * and estimated standard deviation $\sigma$ etc.
 * 
 * Instead of the more numerically stable but much slower algorithm from
 *   http://en.wikipedia.org/wiki/Standard_deviation
 * we just accumulate x and x^2 .
 *
 *
    void makePath(double path, unsigned int cellId) {
        Require(cellId > 0 && cellId <= plTallies.size());
        Require(cellId > 0 && cellId <= volumes.size());
        Require(path > 0);

        plTallies[cellId - 1].accumulateValue(path / volumes[cellId - 1]);
    }

    void flush() {
        for (unsigned int i = 0; i < plTallies.size(); ++i) {
            plTallies[i].flush();
        }
    }
 *
 */

#ifndef mc_BasicTally_hpp
#define mc_BasicTally_hpp
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include "transupport/dbc.hpp"

//#include <cstdio>

namespace monteCarlo {

/*----------------------------------------------------------------------------*/
/*
 * \class BasicTally
 * \brief Simple tally that reports mean, stdev, estimated deviation of the mean
 */
template <typename T>
class BasicTally {
public:
    BasicTally() : 
            sumX(0.0), sumXSquared(0.0), numTrials(0), curVal(0.0)
    { /* * */ }



    //! add a value to the next quantity that will be tallied
    void accumulateValue(const T &newData) {
        curVal += newData;
    }

    //! Count the accumulated quantity as a single sample
    void flush() {
        addValue(curVal);
        curVal = 0.0;
    }

    //! Make sure that there is nothing in the accumulating buffer
    bool checkFlushed() {
        return curVal == 0.0;
    }

    //! Add a number
    void addValue(const T &newDataPoint) {
        sumX += newDataPoint;
        sumXSquared += newDataPoint * newDataPoint;
    }

    //! Overload the += operator to add a new data point
    BasicTally<T>& operator+= (const T &newDataPoint) {
        addValue(newDataPoint);
        Check(curVal == 0);
        return *this;
    }
    
    //! Return the mean given the number of trials elapsed
    T getMean() const {
        Insist(numTrials > 0, "Must have set positive number of trials.");

        return sumX / numTrials;
    }

    //! Return the square root of the standard variance
    T getStdev() const {
        Insist(numTrials > 1, "Must have set more than one trials.");

        return sqrt((sumXSquared - (sumX * sumX)/numTrials) 
                     /(numTrials - 1));
    }

    //! Return the estimated standard deviation of the estimated mean
    T getMeanStdev() const {
        Insist(numTrials > 1, "Must have set more than one trials.");

//        PRINTDOUBLE(sumXSquared);
//        PRINTDOUBLE(sumXSquared - (sumX * sumX)/numTrials);
//        PRINTDOUBLE(numTrials * double(numTrials - 1));
//        PRINTDOUBLE((sumXSquared - (sumX * sumX)/numTrials) 
//                         /(numTrials * (numTrials - 1)));

        return sqrt((sumXSquared - (sumX * sumX)/numTrials) 
                     /(numTrials * double(numTrials - 1)));
    }
    
    //! Need the user to tell us how many trials were run
    void setNumTrials(const int _numTrials) {
        numTrials = _numTrials;
    }

    //! Return the number of data points added to this
    int getNumTrials() const {
        return numTrials;
    }
private:
    T sumX;
    T sumXSquared;
    int numTrials;
    T curVal;
};

//! Function to output the tally information
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const BasicTally<T>& theTally)
{
    os  << "<" << "Number of samples:" << theTally.getNumTrials() <<", "
        << std::setprecision(5)
        << "Sample mean: "  << std::setw(10) << theTally.getMean() << " "
        << std::setprecision(3)
        << "+-" << std::setw(8) <<theTally.getMeanStdev() << ", "
        << std::setprecision(5)
        << "Sample stdev: " << std::setw(10) << theTally.getStdev() << ">";

    return os;
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif
