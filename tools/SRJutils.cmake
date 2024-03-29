# A collection of cmake macros for use in my projects
#

################################################################################
# Set up DBC and debug compiler options 
#
# srj_set_compiler_defs()
#
macro(srj_set_compiler_defs)
  #---------- DBC
  set(DBC 0 CACHE STRING
    "Enable design-by-contract code, 0-7 (0 = off, 7 = all)"
    )

  #verify DBC setting (if bad, pretend it's zero, and fail at the end)
  if(NOT DBC MATCHES "[0-7]")
    message(SEND_ERROR "Invalid DBC value (must be 0-7);")
    set(DBC 0)
  endif(NOT DBC MATCHES "[0-7]")
  add_definitions("-DDBC=${DBC}")

  if(DBC EQUAL 0)
    add_definitions("-DNOASSERT")
  endif(DBC EQUAL 0)

  #---------- COMPILER FLAGS
  if (CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    # vectorize if it's supported, only on Release build
    check_cxx_compiler_flag("-ftree-vectorize" USE_VECTORIZE)
    if( USE_VECTORIZE )
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-vectorize")
    endif( USE_VECTORIZE )
    
    # turn on extra compiler warnings regardless
    add_definitions(-Wall -Wextra)
  endif (CMAKE_COMPILER_IS_GNUCXX)

  #warn if release and DBC
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(NOT DBC EQUAL 0)
      message(STATUS "WARNING: using non-zero DBC with Release configuration")
    endif(NOT DBC EQUAL 0)
  endif(CMAKE_BUILD_TYPE STREQUAL "Release")

endmacro(srj_set_compiler_defs)
################################################################################
# Make a unit test 
#
# srj_make_test(TESTS [test1 test2 ...]
#               [DEPENDS lib1 [lib2 [...]]]
#               [SUBPROJECT name]
#              )
#
macro(srj_make_test)
  parse_arguments(MKTEST "TESTS;DEPENDS;SUBPROJECT" "" ${ARGN})
  #message(STATUS "TESTS: ${MKTEST_TESTS}")
  #message(STATUS "DEPENDS: ${MKTEST_DEPENDS}")
  #message(STATUS "SUBPROJECT: ${MKTEST_SUBPROJECT}")

  # verify that tests are actually defined 
  if(NOT MKTEST_TESTS)
    message(SEND_ERROR "Cannot call srj_make_test without TESTS <testnames>")
  endif(NOT MKTEST_TESTS)

  # if we have a subproject name, put our tests in a subfolder
  if(MKTEST_SUBPROJECT)
    set(TEST_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test/${MKTEST_SUBPROJECT}" )
  else(MKTEST_SUBPROJECT)
    set(TEST_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test" )
  endif(MKTEST_SUBPROJECT)


  # loop over each unit test
  foreach(TEST_NAME ${MKTEST_TESTS})
    set(TEST_FILE "${TEST_NAME}.cpp")

    add_executable("${TEST_NAME}" 
      EXCLUDE_FROM_ALL
      ${TEST_FILE}
      )

    # tell each to link against whatever libraries we need
    if(MKTEST_DEPENDS)
      target_link_libraries(${TEST_NAME} ${MKTEST_DEPENDS})
    endif(MKTEST_DEPENDS)

    # if ENABLE_TESTING is defined at the root level, add this test to it
    add_test( ${TEST_NAME} ${TEST_OUTPUT_DIRECTORY}/${TEST_NAME} )
  endforeach(TEST_NAME ${TEST_NAMES})

  # set the output directory for all the tests
  set_target_properties(${MKTEST_TESTS} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}
    )
  # add the test to "make check"
  add_dependencies(check ${MKTEST_TESTS})
endmacro(srj_make_test)

################################################################################
# PARSE_ARGUMENTS is from http://www.cmake.org/Wiki/CMakeMacroParseArguments
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})            
    SET(larg_names ${arg_names})    
    LIST(FIND larg_names "${arg}" is_arg_name)                   
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})    
      LIST(FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
	     SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
	     SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

