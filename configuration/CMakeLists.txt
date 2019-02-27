# record which git version is being built

if (GIT_FOUND)
  exec_program(
    ${GIT_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "describe --dirty"
    OUTPUT_VARIABLE VERSION_SHA1 )
else()
  set(VERSION_SHA1 ${BDSIM_VERSION})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bdsim.sh.in ${CMAKE_BINARY_DIR}/bin/bdsim.sh @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/bin/bdsim.sh DESTINATION bin)
