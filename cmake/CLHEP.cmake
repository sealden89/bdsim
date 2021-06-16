if($ENV{VERBOSE})
  set(CLHEP_CONFIG_DEBUG TRUE)
endif()
message(STATUS "Looking for CLHEP")
find_package(CLHEP REQUIRED)
message(STATUS "Found CLHEP ${CLHEP_VERSION} in ${CLHEP_INCLUDE_DIRS}")

include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS})

# Mark CLHEP_Dir as advanced since can't be set in GUI and this turned out to be confusing
mark_as_advanced(CLHEP_DIR)

if (${CLHEP_VERSION} VERSION_GREATER "2.3.1.0")
  set(CLHEP_HAS_MIXMAX TRUE)
  add_definitions("-DCLHEPHASMIXMAX")
else()
  set(CLHEP_HAS_MIXMAX FALSE)
endif()