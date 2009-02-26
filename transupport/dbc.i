/*!
 * \file dbc.i
 * \author Seth R. Johnson
 * \brief SWIG interface file that adds support for C++ -> Python exception
 *        translations.
 *
 * \todo
 * TODO: see if there is *any* performance penalty in the outer try/catch stuff.
 */

%header %{ 
// Include the dbc file so that whatever we're compiling can throw exceptions
#include <stdexcept>
#include "transupport/dbc.hpp"
%}

// Define a header for tranError simple enough so that SWIG can understand it
// (It can't process std::exception or know that it has a .what() method)
namespace tranSupport {
class tranError {
public:
   tranError(std::string const &msg);
   const char* what() const throw();
   virtual ~tranError() throw();
};
} //end namespace

// automatically set up exception catching if we're using DBC != 0
#if DBC > 0
%exception {
	try {
		$action
	} catch(tranSupport::tranError& err) {
      PyErr_SetString(PyExc_RuntimeError, err.what());
      return NULL; 
	} catch(std::exception& err) {
      PyErr_SetString(PyExc_RuntimeError, err.what());
      return NULL; 
	} catch(const char* msg) {
      PyErr_SetString(PyExc_RuntimeError, msg);
      return NULL; 
	} catch(...) {
      PyErr_SetString(PyExc_RuntimeError, "Unknown exception was thrown.");
      return NULL; 
	}
}
#endif

