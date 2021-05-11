bdsim_install_gdml(gdml.xsd gdml_core.xsd gdml_define.xsd gdml_extensions.xsd gdml_materials.xsd gdml_parameterised.xsd gdml_replicas.xsd gdml_solids.xsd)

if (GIT_FOUND)
  copy_gdml_to_build()
else()
  file(GLOB LOCAL_FILES 
       RELATIVE ${CMAKE_SOURCE_DIR}
       *.xsd)
  foreach(file ${LOCAL_FILES})
    configure_file(${CMAKE_SOURCE_DIR}/${file} ${CMAKE_BINARY_DIR}/${file} COPYONLY)
  endforeach()
endif()
  