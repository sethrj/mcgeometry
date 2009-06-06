# - Find TranSupport library (and headers)
# Find the native TranSupport includes and library
# This module defines
#  TranSupport_INCLUDE_DIR, where to find blitz/blitz.h, etc.
#  TranSupport_LIBRARIES, libraries to link against to use TranSupport.
#  TranSupport_FOUND, If false, do not try to use TranSupport.
# also defined, but not for general use are
#  TranSupport_LIBRARY, where to find the TranSupport library.

FIND_PATH(TranSupport_INCLUDE_DIR transupport/dbc.hpp)

FIND_LIBRARY(TranSupport_LIBRARY
  NAMES transupport )

# handle the QUIETLY and REQUIRED arguments and set TranSupport_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TranSupport
  DEFAULT_MSG  TranSupport_LIBRARY
  TranSupport_INCLUDE_DIR)

IF(TranSupport_FOUND)
  SET( TranSupport_LIBRARIES ${TranSupport_LIBRARY} )
ENDIF(TranSupport_FOUND)

MARK_AS_ADVANCED(TranSupport_INCLUDE_DIR TranSupport_LIBRARY)
