set(SUBPROJECT_NAME mcgeometry)

set(HEADER_DIRECTORY "include/${SUBPROJECT_NAME}/")

#include blitz
include_directories(${Blitz_INCLUDE_DIR})

############ Create a library ############
set(TARGET_NAME ${SUBPROJECT_NAME})
set(SOURCES
  MCGeometry.cpp
  )

add_library(${TARGET_NAME} ${SOURCES})

# requires the transupport library
target_link_libraries(${TARGET_NAME} transupport)

add_dependencies(lib ${TARGET_NAME})

############ Install targets ############
file(GLOB INSTALL_INCLUDE_FILES . "*.hpp")
install(FILES ${INSTALL_INCLUDE_FILES}
  DESTINATION ${HEADER_DIRECTORY}
  )

install(TARGETS ${TARGET_NAME}
  ARCHIVE DESTINATION lib
  )

#recurse into tests directory
add_subdirectory(tests)
