# executable options

# print help
simple_testing(option-executable-help "--help" "")

# print list of materials
simple_testing(option-executable-materials "--materials" "")

# primaries only
set(TESTING_ARGS --generatePrimariesOnly)
simple_testing(option-executable-generatePrimariesOnly "--file=sm.gmad" "")

# output survey
set(TESTING_ARGS --survey=survey.dat)
simple_testing(option-executable-survey "--file=sm.gmad" "")


# output export geometry
if(USE_GDML)
  set(TESTING_ARGS --exportGeometryTo=export.gdml)
  simple_testing(option-executable-exportGeometryTo "--file=sm.gmad" "")
endif()

# seed
set(TESTING_ARGS --seed=1729)
simple_testing(option-executable-seed "--file=sm.gmad" "")

# skip events
set(TESTING_ARGS --startFromEvent=10)
simple_testing(option-executable-startFromEvent "--file=sm.gmad" "")

# verbose
set(TESTING_ARGS --verbose)
simple_testing(option-executable-verbose "--file=sm.gmad" "")

# verbose event
set(TESTING_ARGS --verbose_event)
simple_testing(option-executable-verbose_event "--file=sm.gmad" "")
simple_testing(option-verboseEvent             "--file=verboseEvent.gmad" "")
simple_testing(option-verboseEvent-primaries   "--file=verboseEvent-primaries.gmad" "")

# verbose event number
set(TESTING_ARGS --verbose_event_num=0)
simple_testing(option-executable-verbose_event_num "--file=sm.gmad" "")

# verbose step
set(TESTING_ARGS --verbose_step)
simple_testing(option-executable-verbose_step "--file=sm.gmad" "")

# verbose levels
set(TESTING_ARGS "--verbose_G4run=5 --verbose_G4event=5 --verbose_G4tracking=5 --verbose_G4stepping=5")
simple_testing(option-executable-verbose-g4levels "--file=sm.gmad" "")

# Test that ptc one turn map does not crash bdsim
simple_testing(option-ptc-otm "--file=ptcOneTurnMap.gmad --circular" "")

# Test that original teleporter approach does not crash bdsim
simple_testing(option-originalTeleporterMethod "--file=originalTeleporterMethod.gmad --circular" "")

## Some tests that check wrong input options will fail
simple_testing(option-length-safety-too-small "--file=smallLengthSafety.gmad" "")
set_tests_properties(option-length-safety-too-small PROPERTIES WILL_FAIL 1)

simple_testing(option-outer-diameter-too-small "--file=smallOuterDiameter.gmad" "")
set_tests_properties(option-outer-diameter-too-small PROPERTIES WILL_FAIL 1)

simple_testing(option-zero-beam-energy "--file=zeroBeamEnergy.gmad" "")
set_tests_properties(option-zero-beam-energy PROPERTIES WILL_FAIL 1)

simple_testing(option-ignore-local-aperture "--file=overrideAperture.gmad" "")
simple_testing(option-ignore-local-magnet-geometry "--file=overrideMagnetGeometry.gmad" "")

simple_testing(option-noeloss-beampipes      "--file=noeloss-beampipes.gmad" "")
simple_testing(option-noeloss-outer          "--file=noeloss-outer.gmad"     "")
simple_testing(option-eloss-sensitive-vacuum "--file=eloss-vacuum.gmad"      "")
