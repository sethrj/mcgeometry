# MCGeometry CMake file
# Seth R. Johnson
# 

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

project(MCGeometry)
set( SVN_REVISION "$Rev$")

set( ${PROJECT_NAME}_MAJOR_VERSION 1 ) 
set( ${PROJECT_NAME}_MINOR_VERSION 0 ) 
string(REGEX REPLACE "\\$Rev:? *([0-9]*) *\\$" "\\1" 
  ${PROJECT_NAME}_PATCH_LEVEL ${SVN_REVISION})

# these variables are used to set the correct numbering of shared libraries
SET( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

list(APPEND CMAKE_MODULE_PATH ${MCGeometry_SOURCE_DIR}/tools/)
include(SRJutils)

# Set default output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#------------------------------ OPTIONS ------------------------------#

#Set the default build type
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

srj_set_compiler_defs()

# on Linux systems, need to build shared library if linking for SWIG
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  add_definitions( "-fPIC" )
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

#------------------------------ INCLUDES ------------------------------#
# Set this directory as an include directory so we correctly get
# transupport/blah.hpp and mcgeometry/blah.hpp
include_directories(${PROJECT_SOURCE_DIR})

# blitz
include(FindBlitz REQUIRED)

#------------------------------ TARGETS ------------------------------#
ENABLE_TESTING()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}) # deficiency in cmake
add_custom_target(lib)

#------------------------------  RECURSE ------------------------------#
#recurse into subdirectories
add_subdirectory(transupport)
add_subdirectory(mcgeometry)
add_subdirectory(examples)
add_subdirectory(tools)
add_subdirectory(doc)

#------------------------------ OUTPUT ------------------------------#
message(STATUS "${PROJECT_NAME} version ${${PROJECT_NAME}_VERSION}")
message(STATUS "Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "Design by contract: DBC=${DBC}")
#message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
#message(STATUS "CMAKE_CXX_FLAGS_RELEASE       : ${CMAKE_CXX_FLAGS_RELEASE}")       
#message(STATUS "CMAKE_CXX_FLAGS_DEBUG         : ${CMAKE_CXX_FLAGS_DEBUG}")         
