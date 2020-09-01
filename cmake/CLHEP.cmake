if($ENV{VERBOSE})
  set(CLHEP_CONFIG_DEBUG TRUE)
endif()
find_package(CLHEP REQUIRED)
message(STATUS "Looking for CLHEP... - found")
message(STATUS "Found CLHEP ${CLHEP_VERSION} in ${CLHEP_INCLUDE_DIRS}")

include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS})

# Mark CLHEP_Dir as advanced since can't be set in GUI and this turned out to be confusing
mark_as_advanced(CLHEP_DIR)
