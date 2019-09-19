rebdsim_optics_test(analysis-optics ../../data/fodo.root optics.root)

# manual test for incorrect number of arguments and a fail
add_test(NAME analysis-optics-incorrect-arguments COMMAND rebdsimOptics)
set_tests_properties(analysis-optics-incorrect-arguments PROPERTIES WILL_FAIL 1)

# manual test for extra emmitanceOnFly argument
add_test(NAME analysis-optics-emittance-on-fly COMMAND rebdsimOptics ../../data/fodo.root opticsEOF.root --emittanceOnFly)

add_test(NAME analysis-optics-emittance-on-the-fly COMMAND rebdsimOptics ../../data/fodo.root opticsEOF.root --emittanceOnTheFly)

add_test(NAME analysis-optics-wrong-argument COMMAND rebdsimOptics ../../data/fodo.root opticsEOF.root --bananas)
set_tests_properties(analysis-optics-wrong-argument PROPERTIES WILL_FAIL 1)

rebdsim_orbit_test(analysis-orbit ../../data/fodo.root orbit.root 3)

# manual test for incorrect number of arguments
add_test(NAME analysis-orbit-incorrect-arguments COMMAND rebdsimOrbit)
set_tests_properties(analysis-orbit-incorrect-arguments PROPERTIES WILL_FAIL 1)
