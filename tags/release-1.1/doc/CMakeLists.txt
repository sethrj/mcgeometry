#Optionally build documentation with doxygen
include(FindDoxygen)
if(DOXYGEN_FOUND)
  #### values to pass into the config file
  get_directory_property(compile_def_list COMPILE_DEFINITIONS) 
  foreach(def ${compile_def_list})
    set( current_definitions "${current_definitions} ${def}")
  endforeach(def)
  set(PROJECT_VERSION "${${PROJECT_NAME}_VERSION}")  

  #### configure everything
  set(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")  
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
    ${DOXY_CONFIG}
    @ONLY #operate on @VAR@, not ${VAR}
    )
  #make target doc
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG}
    )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/index.html.in
    ${CMAKE_CURRENT_BINARY_DIR}/index.html
    )
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen not found - target 'doc' will not be created")
endif(DOXYGEN_FOUND)
