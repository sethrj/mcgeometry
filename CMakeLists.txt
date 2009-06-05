# MCGeometry CMake file
# Seth R. Johnson
# 

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

project(MCGeometry)

set( ${PROJECT_NAME}_MAJOR_VERSION 1 ) 
set( ${PROJECT_NAME}_MINOR_VERSION 0 ) 
set( ${PROJECT_NAME}_PATCH_LEVEL   0 )

#------------------------------ OPTIONS ------------------------------#
#---------- DBC
set(DBC 0 CACHE STRING
  "Enable design-by-contract code, 0-7 (0 = off, 7 = all)"
  )
#verify DBC setting (if bad, pretend it's zero, and fail at the end)
if(NOT DBC MATCHES "[0-7]")
  message(SEND_ERROR "Invalid DBC value (must be 0-7);")
  set(DBC 0)
endif(NOT DBC MATCHES "[0-7]")
message(STATUS "Using DBC=${DBC}")
add_definitions("-DDBC=${DBC}")

if(DBC EQUAL 0)
  add_definitions("-DNOASSERT")
endif(DBC EQUAL 0)

#---------- DEBUG
option(DEBUG
  "Turn optimization off, enable debug code"
  OFF)

if( DEBUG )
  # DEBUG ON
  message(STATUS "Configuring with debug ON")
else( DEBUG )
  # DEBUG OFF
  message(STATUS "Configuring with debug OFF")
  if(NOT DBC EQUAL 0)
    message(STATUS "WARNING: using non-zero DBC with Release configuration")
  endif(NOT DBC EQUAL 0)

  #full optimization
  add_definitions("-O3 -DNDEBUG")

  #if GCC and version > 4.0 ?
  if( CMAKE_COMPILER_IS_GNUCC )
    add_definitions("-ftree-vectorize")
  endif( CMAKE_COMPILER_IS_GNUCC )

endif( DEBUG )

#---------- BLITZ
add_definitions("-Wall -Wextra -ansi -pedantic")

# Set default output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#------------------------------ INCLUDES ------------------------------#
list(APPEND CMAKE_MODULE_PATH ${MCGeometry_SOURCE_DIR}/tools/)
include(SRJutils)

# Set this directory as an include directory so we correctly get
# transupport/blah.hpp and mcgeometry/blah.hpp
include_directories(${PROJECT_SOURCE_DIR})

# blitz
include(FindBlitz)

#------------------------------ TARGETS ------------------------------#
ENABLE_TESTING()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}) # deficiency in cmake

add_custom_target(lib)


#------------------------------  RECURSE ------------------------------#
#recurse into subdirectories
add_subdirectory(transupport)
add_subdirectory(mcgeometry)
add_subdirectory(examples)
