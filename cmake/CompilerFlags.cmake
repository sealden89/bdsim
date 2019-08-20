# warning flags
set(WFLAGS "-Wall -Wextra -pedantic -Wpointer-arith -Woverloaded-virtual -Wnon-virtual-dtor -Wshadow")
# for ROOT dictionaries
# stop warnings from Clang version (for versions 7 and greater)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  if (CLANG_VERSION_STRING VERSION_GREATER 6.9)
    set(WFLAGS "${WFLAGS} -Wno-keyword-macro")
  endif()
  if (CLANG_VERSION_STRING VERSION_GREATER 8.0)
    set(WFLAGS "${WFLAGS} -Wno-undefined-var-template")
  endif()
endif()

# cmake flags
set(CMAKE_FLAGS "${CMAKE_FLAGS} ${CLHEP_DEFINITIONS}  ${WFLAGS}")
set(BDSIM_FLAGS "${CMAKE_FLAGS} -DCLHEP_VERSION=${CLHEP_VERSION} -DG4VERSION=${G4_MAJOR_VERSION} -DG4MINORVERSION=${G4_MINOR_VERSION} ${BDSIM_FLAGS}")
# add original flags to cmake c and c++ flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_ORIG_FLAGS} ${BDSIM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_ORIG_FLAGS} ${BDSIM_FLAGS}")
if(APPLE)
  # For MacOS bundle
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -headerpad_max_install_names")
  if($ENV{VERBOSE})
    message(STATUS "CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}")
  endif()
endif()

if($ENV{VERBOSE})
  message(STATUS "BDSIM_FLAGS ${BDSIM_FLAGS}")
  message(STATUS "CMAKE_C_FLAGS ${CMAKE_C_FLAGS}")
  message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()
