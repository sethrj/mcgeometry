set(SUBPROJECT_NAME transupport)

set(HEADER_DIRECTORY "include/${SUBPROJECT_NAME}/")

############ Create a library ############
set(TARGET_NAME ${SUBPROJECT_NAME})
set(SOURCES
  UnitTester.cpp
  )
add_library(${TARGET_NAME} ${SOURCES})
#set_target_properties(${TARGET_NAME} PROPERTIES
#  VERSION ${${PROJECT_NAME}_VERSION}
#  )
add_dependencies(lib ${TARGET_NAME})

############ Install targets ############
file(GLOB INC . "*.hpp")
#message(STATUS "Globbed headers: ${INC}")
install(FILES ${INC}
  DESTINATION ${HEADER_DIRECTORY}
  )

install(TARGETS ${TARGET_NAME}
  ARCHIVE DESTINATION lib
  )

#recurse into tests directory
add_subdirectory(tests)
