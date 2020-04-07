file(GLOB comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove mains from dictionary
list(REMOVE_ITEM comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/comparator.cc)

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(comparator SHARED ${comparatorLibSources})
target_link_libraries(comparator bdsimRootEvent rebdsim ${ROOT_LIBRARIES})

add_executable(comparatorexec comparator.cc)
set_target_properties(comparatorexec PROPERTIES OUTPUT_NAME "comparator" VERSION ${BDSIM_VERSION})
target_link_libraries(comparatorexec comparator ${BDSIM_LIB_NAME} ${ROOT_LIBRARIES})

# Installation
bdsim_install_targets(comparatorexec comparator)

file(GLOB comparatorHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
bdsim_install_comparator_headers(${comparatorHeaders})

get_target_property(comparatorBinaryName comparatorexec OUTPUT_NAME)
set(comparatorBinary ${CMAKE_CURRENT_BINARY_DIR}/${comparatorBinaryName} CACHE STRING "comparator binary")
mark_as_advanced(comparatorBinary)
