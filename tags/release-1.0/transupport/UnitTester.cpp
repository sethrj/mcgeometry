/*!
 * \file UnitTester.cpp
 * \brief Unit test master class
 * \author Seth R. Johnson
 * 
 * A class that is responsible for handling unit tests
 *
 */

#include <string>
#include <iostream>
#include "UnitTester.hpp"

using std::string;
using std::cout;
using std::endl;

namespace tranSupport {
/*----------------------------------------------------------------------------*/
void UnitTester::setName(const string newName) {
   if (testerName == "Unnamed") {
      testerName = newName;
   } else {
      cout << "WARNING: tried to set UnitTester name to \"" << newName 
           << "\" when it was already \"" << testerName << "\"." << endl;
   }
}

void UnitTester::checkForPass(
                  const bool cond, 
                  const string passMessage, const string failureMessage,
                  const char code[], const char fileName[],
                  const int lineNumber)
{
   if (cond) {
      //test passed
      if (passMessage != "") {
         cout << "  PASSED: " << passMessage << endl;
      } else {
         cout << "  PASSED: " << code << endl;
      }

   } else {
      //test failure
      cout << "  FAILED: " << failureMessage << endl;
      cout << "  |   in <" << fileName << "> on line " << lineNumber << endl;
      cout << "  |   " << code << endl;
      cout << "  +-------------------------------" << endl;

      stillPassing = false;
   }
}

void UnitTester::printResult() const
{
   cout << "==================================================" << endl;
   cout << "Unit testing for <" << testerName << ">" << endl;

   if (stillPassing) {
      cout << "PASSED" << endl;
   } else {
      cout << "FAILED" << endl;
   }

   cout << "==================================================" << endl;
}

/*----------------------------------------------------------------------------*/
}

