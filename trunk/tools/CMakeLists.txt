#install cmake support files in ${PREFIX}/share/CMake/
file(GLOB INSTALL_INCLUDE_FILES . "*.cmake")
install(FILES ${INSTALL_INCLUDE_FILES}
        DESTINATION "share/cmake")
