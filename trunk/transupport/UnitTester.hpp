/*!
 * \file UnitTester.hpp
 * \brief Unit test master class
 * \author Seth R. Johnson
 * 
 * A class that is responsible for handling unit tests
 *
 */

#ifndef TS_UNITTESTER_HPP
#define TS_UNITTESTER_HPP
/*----------------------------------------------------------------------------*/

#include <string>

namespace tranSupport {
/*----------------------------------------------------------------------------*/

//! \brief Unit tester class checks for failures in tests
// it also prints messages
class UnitTester {

public:
   //! return the only instance of the singleton
   static UnitTester& Instance() {
      static UnitTester theOnlyUnitTester;

      return theOnlyUnitTester;
   }

   //! set the name
   void setName(const std::string newName);

   //! see if we haven't failed any tests yet
   bool hasPassed()
   { return stillPassing; }

   //! verify the check condition, report if failure
   void checkForPass(const bool cond, 
                     const char code[], const char fileName[],
                     const int lineNumber);


   //! print whether it passed or failed
   void printResult() const;

private:
   //! hide constructor to prevent instantiation outside this class
   UnitTester() : stillPassing(true), testerName("Unnamed")
   { /* * */ }
   //! hide copy constructor
   UnitTester(UnitTester const&) { /* * */ }
   //! hide equivalence
   UnitTester& operator=(UnitTester const&) { /* * */ return *this;  }


   //! stores if we've failed a test or not
   bool      stillPassing;
   //! the name of the unit test for printing later
   std::string    testerName;
};

/*----------------------------------------------------------------------------*/
}
/*============================================================================*/

//! initialize unit tester
#define TESTER_INIT(name) \
   tranSupport::UnitTester::Instance().setName((name))

//! check passing condition, auto-insert file name and line number
#define TESTER_CHECKFORPASS(cond) \
   tranSupport::UnitTester::Instance().checkForPass((cond), #cond, __FILE__, __LINE__)

//! print the report
#define TESTER_PRINTRESULT() \
   tranSupport::UnitTester::Instance().printResult()

//! print the report
#define TESTER_HASPASSED() \
   tranSupport::UnitTester::Instance().hasPassed()

//! so that the output is more legible if we fail in the catch block
#define CAUGHT_UNEXPECTED_EXCEPTION false

/*----------------------------------------------------------------------------*/
#endif
