add_executable(makematerialfileExec makematerialfile.cc)
set_target_properties(makematerialfileExec PROPERTIES OUTPUT_NAME "makematerialfile" VERSION ${BDSIM_VERSION})
target_link_libraries(makematerialfileExec ${Geant4_LIBRARIES})
bdsim_install_targets(makematerialfileExec)

add_test(NAME materials-nist COMMAND makematerialfileExec)
