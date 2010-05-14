add_custom_target(examples)

#include blitz
include_directories(${Blitz_INCLUDE_DIR})

set(EXAMPLE_NAMES
  meshTiming meshComparison trickyGeometry
  )
add_executable(meshTiming
  EXCLUDE_FROM_ALL
  meshTiming.cpp createGeometry.cpp
  )
add_executable(meshComparison
  EXCLUDE_FROM_ALL
  meshComparison.cpp createGeometry.cpp extra/mtrand.cc
  )
add_executable(trickyGeometry
  EXCLUDE_FROM_ALL
  trickyGeometry.cpp createGeometry.cpp visualizeSurfaces.cpp extra/mtrand.cc
  )

# link each target to transupport, mcgeometry
foreach(TARGET_NAME ${EXAMPLE_NAMES})
  target_link_libraries(${TARGET_NAME} transupport mcgeometry)
endforeach(TARGET_NAME)

# new target: build examples builds these
add_dependencies(examples ${EXAMPLE_NAMES})

