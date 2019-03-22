if (USE_GDML)
  set(TESTING_ARGS --output=none)
  simple_testing(gdml-magnet-outer "--file=1_magnet_outer.gmad" ${OVERLAP_CHECK})

  set(TESTING_ARGS --output=none)
  simple_testing(gdml-placement    "--file=2_placement.gmad" ${OVERLAP_CHECK})

  set(TESTING_ARGS --output=none)
  simple_testing(gdml-two-files    "--file=3_twogdmls.gmad" ${OVERLAP_CHECK})

  set(TESTING_ARGS --output=none)
  simple_testing(gdml-parametrised "--file=4_parametrised_gdml.gmad" ${OVERLAP_CHECK})

  set(TESTING_ARGS --output=none)
  simple_testing(gdml-sensitive-per-volume "--file=5_gdml_sensitivity.gmad" ${OVERLAP_CHECK})

  simple_testing(gdml-preprocess-schema    "--file=6_preprocess_schema_only.gmad --output=none" ${OVERLAP_CHECK})
endif()
