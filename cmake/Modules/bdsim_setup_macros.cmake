# based on CMake from Geant4 to set environmental variables for differnet platforms
# see geant4.10.02.p03/cmake/Modules/Geant4ToolchainBackwardCompatibility.cmake

function(_append_path TEMPLATE_NAME PATH_VARIABLE APPEND_VARIABLE)
  # We have to make this section verbatim
  set(${TEMPLATE_NAME}
  "
# set library path
if test \"x\$${PATH_VARIABLE}\" = \"x\" ; then
  export ${PATH_VARIABLE}=${APPEND_VARIABLE}
else
  export ${PATH_VARIABLE}=\${${PATH_VARIABLE}}:${APPEND_VARIABLE}
fi
"
  PARENT_SCOPE
  )
endfunction()

# Set library path, based on relative paths between bindir and libdir
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(_libpathname DYLD_LIBRARY_PATH)
else()
  set(_libpathname LD_LIBRARY_PATH)
endif()

set(BDSIM_LIB_PATH_SETUP "# set library path")
_append_path(BDSIM_LIB_PATH_SETUP
  ${_libpathname}
  "$BDSIM/lib"
  )