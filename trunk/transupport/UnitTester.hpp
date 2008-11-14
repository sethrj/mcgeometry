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
                     const std::string passMessage, const std::string failureMessage,
                     const char code[], const char fileName[],
                     const int lineNumber);

   //! verify the check condition, report if failure
   void checkForPass(const bool cond, 
                     const char code[], const char fileName[],
                     const int lineNumber)
   { checkForPass(cond, "", "", code, fileName, lineNumber); }


   //! print whether it passed or failed
   void printResult() const;

private:
   // hide constructors to prevent instantiation outside this class
   UnitTester() : stillPassing(true), testerName("Unnamed")
   { /* * */ }
   UnitTester(UnitTester const&) { /* * */ }
   UnitTester& operator=(UnitTester const&) { /* * */ return *this;  }


   bool      stillPassing;
   std::string    testerName;
};

/*----------------------------------------------------------------------------*/
}
/*============================================================================*/

#ifndef CHECKFORPASS

//! initialize unit tester
#define TESTER_INIT(name) \
   tranSupport::UnitTester::Instance().setName((name));

//! check passing condition, auto-insert file name and line number
#define TESTER_CHECKFORPASS(cond) \
   tranSupport::UnitTester::Instance().checkForPass((cond), #cond, __FILE__, __LINE__);

//! check passing condition, but with verbose pass and failure messages
#define TESTER_CHECKFORPASSV( cond, passMessage, failureMessage) \
   tranSupport::UnitTester::Instance().checkForPass((cond), (passMessage), (failureMessage), #cond, \
         __FILE__, __LINE__);

//! print the report
#define TESTER_PRINTRESULT() \
   tranSupport::UnitTester::Instance().printResult();

#endif


/*----------------------------------------------------------------------------*/
#endif
