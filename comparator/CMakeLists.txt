file(GLOB comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove main from dictionary
list(REMOVE_ITEM comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/comparator.cc)

add_library(comparator SHARED ${comparatorLibSources})
target_link_libraries(comparator bdsimRootEvent rebdsim ${ROOT_LIBRARIES})

add_executable(comparatorExec comparator.cc)
set_target_properties(comparatorExec PROPERTIES OUTPUT_NAME "comparator" VERSION ${BDSIM_VERSION})
target_link_libraries(comparatorExec comparator ${BDSIM_LIB_NAME} ${ROOT_LIBRARIES})

# Installation
bdsim_install_targets(comparatorExec comparator)

file(GLOB comparatorHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
bdsim_install_comparator_headers(${comparatorHeaders})
