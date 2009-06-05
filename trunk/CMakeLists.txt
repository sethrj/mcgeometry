# MCGeometry CMake file
# Seth R. Johnson
# 

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

project(MCGeometry)

set( ${PROJECT_NAME}_MAJOR_VERSION 1 ) 
set( ${PROJECT_NAME}_MINOR_VERSION 0 ) 
set( ${PROJECT_NAME}_PATCH_LEVEL   0 )

list(APPEND CMAKE_MODULE_PATH ${MCGeometry_SOURCE_DIR}/tools/)
include(SRJutils)
include(CheckCXXCompilerFlag)
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

message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

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

  check_cxx_compiler_flag("-ftree-vectorize" USE_VECTORIZE)
  if( USE_VECTORIZE )
    add_definitions("-ftree-vectorize")
  endif( USE_VECTORIZE )

endif( DEBUG )

#---------- BLITZ
add_definitions("-Wall -Wextra")

# Set default output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#------------------------------ INCLUDES ------------------------------#
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
