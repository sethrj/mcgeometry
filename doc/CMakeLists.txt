#Optionally build documentation with doxygen
include(FindDoxygen)
if(DOXYGEN_FOUND)
  #make target doc
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen not found - target 'doc' will not be created")
endif(DOXYGEN_FOUND)
