# MCGeometry CMake file
# Seth R. Johnson
# 

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

project(MCGeometry)

#------------------------------ VERSION INFO ------------------------------#
set( ${PROJECT_NAME}_MAJOR_VERSION 1 ) 
set( ${PROJECT_NAME}_MINOR_VERSION 0 ) 

find_package(Subversion)
if(Subversion_FOUND)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} MCGSVN)
  set( ${PROJECT_NAME}_PATCH_LEVEL ${MCGSVN_WC_REVISION} ) 
else(Subversion_FOUND)
  set( ${PROJECT_NAME}_PATCH_LEVEL 0 ) 
endif(Subversion_FOUND)

# these variables are used to set the correct numbering of shared libraries
set( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
set( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

#--------------------------- INCLUDE AND PATHS --------------------------#

list(APPEND CMAKE_MODULE_PATH ${MCGeometry_SOURCE_DIR}/tools/)
include(SRJutils)

# Set this directory as an include directory so we correctly get
# transupport/blah.hpp and mcgeometry/blah.hpp
include_directories(${PROJECT_SOURCE_DIR})

# blitz
find_package(Blitz REQUIRED)

# Set default output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#------------------------------ OPTIONS ------------------------------#

# Set the default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Handle DBC, extended debug options, etc.
srj_set_compiler_defs()

# on Linux systems, need to build shared library if linking for SWIG
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  list(APPEND STATIC_LIBRARY_FLAGS "-fPIC" )
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

#------------------------------ TARGETS ------------------------------#
enable_testing()
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
message(STATUS "${PROJECT_NAME} version  ${${PROJECT_NAME}_VERSION}")
message(STATUS "Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "Design by contract: DBC=${DBC}")
#message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
#message(STATUS "CMAKE_CXX_FLAGS_RELEASE       : ${CMAKE_CXX_FLAGS_RELEASE}")       
#message(STATUS "CMAKE_CXX_FLAGS_DEBUG         : ${CMAKE_CXX_FLAGS_DEBUG}")         
