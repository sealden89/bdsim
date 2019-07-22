# with all available gui and visualisation options available
# Try first with Geant4 built-in Config (versions 9.5 and higher should have it):
if ($ENV{VERBOSE})
   set(Geant4_CONFIG_DEBUG TRUE)
endif()

# Search for Geant4 CONFIG mode, if prefix given search that path only.
# Add to GUI
set(Geant4_PREFIX "" CACHE PATH "Location of Geant4 (optional)")
if (NOT ${Geant4_PREFIX} STREQUAL "")
   if($ENV{VERBOSE})
      message(STATUS "Geant4 prefix: ${Geant4_PREFIX}")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG PATHS ${Geant4_PREFIX} NO_DEFAULT_PATH)
else()
   if($ENV{VERBOSE})
      message(STATUS "No Geant4 prefix, searching default paths")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG)
endif()

if (NOT Geant4_FOUND AND USE_AFS)
   # Use AFS directory
   if(APPLE)
      message(STATUS "WARNING NO MACOS GEANT4 LIBRARIES AVAILABLE ON AFS")
   elseif(RHL6)
      set(Geant4_INCLUDE_DIRS /afs/cern.ch/sw/lcg/external/geant4/10.3.p01/x86_64-slc6-gcc49-opt/share/include)
      set(Geant4_LIBRARY_DIR  /afs/cern.ch/sw/lcg/external/geant4/10.3.p01/x86_64-slc6-gcc49-opt/lib64)
      set(Geant4_DIR          /afs/cern.ch/sw/lcg/external/geant4/10.3.p01/x86_64-slc6-gcc49-opt/lib64/Geant4-10.3.1)
      set(Geant4_VERSION 10.3.1)
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all PATHS ${Geant4_DIR} NO_DEFAULT_PATH)
endif()

if (Geant4_FOUND)
      message(STATUS "Geant4 Use File: ${Geant4_USE_FILE}")
      include(${Geant4_USE_FILE})
      message(STATUS "Geant4 Definitions: ${Geant4_DEFINITIONS}")

      # We require geant4 be built against the system clhep (as bdsim will be)
      # to ensure strong reproducibility
      # the geant4 config search bit is taken from their cmake script as they don't
      # export the variable for use by others
      find_program(GEANT4_CONFIG NAMES geant4-config
             PATHS $ENV{GEANT4_INSTALL}/bin
                   ${GEANT4_INSTALL}/bin
                   /usr/local/bin /opt/local/bin)
      execute_process(COMMAND ${GEANT4_CONFIG} --has-feature clhep
                  OUTPUT_VARIABLE _TMP)
      if($ENV{VERBOSE})
         message(STATUS "Geant4 config executable:  ${GEANT4_CONFIG}")
         message(STATUS "Geant4 uses its own clhep: ${_TMP}")
      endif()
      if (_TMP MATCHES "yes")
      	 message(FATAL_ERROR "BDSIM requires Geant4 to be compiled using the system CLHEP so it's the same as BDSIM for strong reproducibility - please reconfigure and reinstall Geant4")
      endif()

      execute_process(COMMAND ${GEANT4_CONFIG} --prefix  OUTPUT_VARIABLE _TMP2)
      string(REGEX REPLACE "\n$" "" _TMP2 "${_TMP2}")
      set(Geant4_LIBRARY_DIR ${_TMP2}/lib)

      # We don't support multithreading for now
      if ("${Geant4_DEFINITIONS}" MATCHES "G4MULTITHREADED")
	message(FATAL_ERROR "Currently Geant4 builds with multithreading are not supported at the moment! Please build Geant4 with multithreading off. Exiting")
      endif()
      
      if($ENV{VERBOSE})
        message(STATUS "Geant4_INCLUDE_DIRS: ${Geant4_INCLUDE_DIRS}")
        message(STATUS "Geant4_LIBRARY_DIR: ${Geant4_LIBRARY_DIR}")
        message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
      endif()
      # Mark Geant4_DIR as advanced since can't be set in GUI and this turned out to be confusing
      # Mark other Geant4 things as advanced to hide from bdsim's cmake configuration
      mark_as_advanced(Geant4_DIR)
      mark_as_advanced(Geant4_PREFIX)
      mark_as_advanced(Qt5Core_DIR)
      mark_as_advanced(Qt5Gui_DIR)
      mark_as_advanced(Qt5OpenGL_DIR)
      mark_as_advanced(Qt5PrintSupport_DIR)
      mark_as_advanced(Qt5Widgets_Dir)

      option(USE_CUSTOM_CHANNELLING "Use custom build of crystal channelling in Geant4" OFF)
      if(USE_CUSTOM_CHANNELLING)
        add_definitions("-DCUSTOMCHANNELING")
        set(PREPROCESSOR_DEFS "-DCUSTOMCHANNELING")
      endif()
else(Geant4_FOUND)
      message(FATAL_ERROR "Geant4 not found. Exiting. You can help cmake by adding -DGeant4_PREFIX=<PATH>")
endif()

if (Geant4_VERSION AND NOT "${Geant4_VERSION}" MATCHES "UNKNOWN")
  string(SUBSTRING ${Geant4_VERSION} 0 1 G4_MAJOR_VERSION)
  if(${G4_MAJOR_VERSION} STREQUAL "1")
    string(SUBSTRING ${Geant4_VERSION} 0 2 G4_MAJOR_VERSION)
    string(SUBSTRING ${Geant4_VERSION} 3 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 5 1 G4_PATCH_LEVEL)
  else()
    string(SUBSTRING ${Geant4_VERSION} 2 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 4 1 G4_PATCH_LEVEL)
  endif()

  message(STATUS "G4_VERSION: ${Geant4_VERSION}")
  if($ENV{VERBOSE})
    message(STATUS "G4_MAJOR_VERSION: ${G4_MAJOR_VERSION}")
    message(STATUS "G4_MINOR_VERSION: ${G4_MINOR_VERSION}")
    message(STATUS "G4_PATCH_LEVEL: ${G4_PATCH_LEVEL}")
  endif()
  if(10 GREATER ${G4_MAJOR_VERSION})
    message(FATAL_ERROR "Geant4 version 10 is needed!")
  endif()
else()
  set(G4_VERSION 10.9.9)
  set(G4_MAJOR_VERSION 10)
  set(G4_MINOR_VERSION 9)
  set(G4_PATCH_LEVEL 9)
endif()
