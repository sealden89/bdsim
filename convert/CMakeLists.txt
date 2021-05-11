file(GLOB convertLibSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# Remove mains from dictionary
list(REMOVE_ITEM convertLibSources ${CMAKE_CURRENT_SOURCE_DIR}/ptc2Bdsim.cc)

set(PREPROCESSOR_DEFS "-D__ROOTBUILD__;-D__ROOTDOUBLE__")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
add_library(convert SHARED ${convertLibSources})
target_link_libraries(convert bdsimRootEvent)

add_executable(ptc2bdsimExec ptc2Bdsim.cc)
set_target_properties(ptc2bdsimExec PROPERTIES OUTPUT_NAME "ptc2bdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(ptc2bdsimExec convert bdsimRootEvent)

bdsim_install_targets(ptc2bdsimExec convert)
