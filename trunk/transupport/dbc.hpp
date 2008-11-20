/*!
 * \file dbc.hpp
 * \brief Design by contract
 * \author Seth R. Johnson
 * 
 * Support for assertions in a program that can be disabled
 *
 */

#ifndef TS_DBC_HPP
#define TS_DBC_HPP
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

namespace tranSupport {
/*----------------------------------------------------------------------------*/

/*! \class tranError
 *  \brief An error for us to throw at will
 *  We subclass std::logic_error so that we can do catch(tranError) which will 
 *  only catch errors that *we* throw.
 */
class tranError : public std::logic_error {
public:
   //! \brief basic constructor called by throwError
   tranError(std::string const &msg) : std::logic_error(msg)
      { /* * */ }

   virtual ~tranError() throw() // don't allow destructor to throw an error
      { /* * */ }
};

//! \brief Throw a tranError
//* Called by Require, Check, and Ensure
inline void throwError( const char assertType[], const char theCode[],
                        const char fileName[], int lineNumber)
{
   std::ostringstream errorString;
   errorString << "Assertion \"" << assertType << "\" failed in file <"
         << fileName << "> line " << lineNumber << ":\n"
         << "\t" << theCode << "\n";

   throw tranError(errorString.str());
}

//! \brief Throw a tranError if an Insist fails
inline void throwInsist(const char theMessage[], const char fileName[], 
                        int lineNumber)
{
   std::ostringstream errorString;
   errorString << "Insist failed in file <"
         << fileName << "> line " << lineNumber << ":\n"
         << "\t" << theMessage << "\n";

   throw tranError(errorString.str());

}

/*----------------------------------------------------------------------------*/
}
/*============================================================================*/

//! \brief Assert for user-given input
// will never be turned off
#define Insist(cond, msg) if (!(cond)) tranSupport::throwInsist(msg, __FILE__, __LINE__)

//! \brief Checks incoming data
#if DBC & 1
#define Require(cond) if (!(cond)) tranSupport::throwError("Require", #cond, __FILE__, __LINE__)
#else
#define Require(cond)
#endif

//! \brief Checks inside a subroutine
#if DBC & 2
#define Check(cond) if (!(cond)) tranSupport::throwError("Check", #cond, __FILE__, __LINE__)
#else
#define Check(cond)
#endif

//! \brief Checks outgoing information
#if DBC & 4
#define Ensure(cond) if (!(cond)) tranSupport::throwError("Ensure", #cond, __FILE__, __LINE__)
#else
#define Ensure(cond)
#endif

/*============================================================================*/
//! Debug code to print some code to evaluate and the result of it (integer)
#define PRINTINT(code)    std::printf("Result of {%s}: %d\n",    #code, (code))

//! Debug code to print some code to evaluate and the result of it (double)
#define PRINTDOUBLE(code) std::printf("Result of {%s}: %.16e\n", #code, (code))

/*----------------------------------------------------------------------------*/
#endif
