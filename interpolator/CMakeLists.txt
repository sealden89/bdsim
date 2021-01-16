# Configure source files
string(TIMESTAMP CURRENT_YEAR %Y)
configure_file(${CMAKE_SOURCE_DIR}/interpolator/bdsinterpolator.cc ${CMAKE_BINARY_DIR}/interpolator/bdsinterpolator.cc @ONLY)

if(USE_GZSTREAM)
  include_directories(${CMAKE_BINARY_DIR}/src-external/gzstream)
endif()

file(GLOB interpolatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove main from library
list(REMOVE_ITEM interpolatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/bdsinterpolator.cc)

# Build library
set(INTERPOLATOR_LIB_NAME "interpolator")
add_library(${INTERPOLATOR_LIB_NAME} SHARED ${interpolatorLibSources})
target_link_libraries(${INTERPOLATOR_LIB_NAME} ${BDSIM_LIB_NAME})
target_link_libraries(${INTERPOLATOR_LIB_NAME} gmadShared)
target_link_libraries(${INTERPOLATOR_LIB_NAME} ${CLHEP_LIBRARIES})
target_link_libraries(${INTERPOLATOR_LIB_NAME} ${GEANT4_LIBRARIES})

# Build executable and link against needed libraries
add_executable(interpolatorexec ${CMAKE_BINARY_DIR}/interpolator/bdsinterpolator.cc)
set_target_properties(interpolatorexec PROPERTIES OUTPUT_NAME "bdsinterpolator" VERSION ${BDSIM_VERSION})
target_link_libraries(interpolatorexec ${INTERPOLATOR_LIB_NAME})

# Installation
bdsim_install_targets(interpolatorexec interpolator)

file(GLOB interpolatorHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
bdsim_install_interpolator_headers(${interpolatorHeaders})

get_target_property(interpolatorBinaryName interpolatorexec OUTPUT_NAME)
set(interpolatorBinary ${CMAKE_CURRENT_BINARY_DIR}/${interpolatorBinaryName} CACHE STRING "interpolator binary")
mark_as_advanced(interpolatorBinary)
