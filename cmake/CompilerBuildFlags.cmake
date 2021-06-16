# Compiler flags based on build type 
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugOutput")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DBDSDEBUG")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
  if(NOT CMAKE_COMPILER_IS_GNUCXX)
    message(WARNING "DebugCoverage only works with gcc compiler")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "None")
  # if specifically None optimise
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -DNDEBUG")
endif()

# Navigation debugging
option( USE_DEBUG_NAVIGATION "Debug navigation" OFF )
if (USE_DEBUG_NAVIGATION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBDSDEBUGNAV")
    message(STATUS "Debug navigation ON")
endif()
mark_as_advanced(USE_DEBUG_NAVIGATION)

# for preprocessing final install path of relocatable build - currently for vis macro final location
set(BDSIM_FINAL_INSTALL_LOCATION "NONE" CACHE PATH "Possible final install location if package will be relocated.")
mark_as_advanced(BDSIM_FINAL_INSTALL_LOCATION)
if (BDSIM_FINAL_INSTALL_LOCATION STREQUAL "NONE")
  set(BDSFINALINSTALLDIR ${CMAKE_INSTALL_PREFIX})
else()
  set(BDSFINALINSTALLDIR ${BDSIM_FINAL_INSTALL_LOCATION})
  message(STATUS "Setting final install dir for preprocessor: ${BDSIM_FINAL_INSTALL_LOCATION}")
endif()

# Field double precision (off by default)
option( USE_FIELD_DOUBLE_PRECISION "Field values in double precision."  OFF )
if(USE_FIELD_DOUBLE_PRECISION)
  add_definitions("-DFIELDDOUBLE")
  message(STATUS "Field double precision ON")
endif()
mark_as_advanced(USE_FIELD_DOUBLE_PRECISION)

# precision of root output (2x disk space)
option(USE_ROOT_DOUBLE_OUTPUT "Double precision root output" OFF)
if(USE_ROOT_DOUBLE_OUTPUT)
  add_definitions("-D__ROOTDOUBLE__")
  message(STATUS "ROOT double precision output ON")
  # this is custom varible used in the analysis cmake when running rootcint
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__;-D__ROOTDOUBLE__")
else()
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__")
endif()

option(USE_CUSTOM_CHANNELLING "Use custom build of crystal channelling in Geant4" OFF)
if(USE_CUSTOM_CHANNELLING)
  message(STATUS "Use custom crystal channelling with Geant4: ON")
  add_definitions("-DCUSTOMCHANNELING")
endif()


# ability to change includes in manual/source/conf.py.in
# the sphinxcontrib.googleanalytics packages is broken, but it's a 1 line
# local fix I have in mine and we want google analytics in the website
# but give ability to turn off so most people can build the manual locally
option( USE_SPHINX_GOOGLE "Use google analytics in sphinx." OFF)
mark_as_advanced(USE_SPHINX_GOOGLE)