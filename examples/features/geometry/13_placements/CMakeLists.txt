if (USE_GDML)
  simple_testing(placement-global             "--file=1_placment_global.gmad --output=none"              ${OVERLAP_CHECK})
  simple_testing(placement-sxy                "--file=2_placment_sxy.gmad --output=none"                 ${OVERLAP_CHECK})
  simple_testing(placement-wrt-element        "--file=3_placement_wrt_element.gmad --output=none"        ${OVERLAP_CHECK})
  simple_testing(placement-wrt-element-unique "--file=4_placement_wrt_element_unique.gmad --output=none" ${OVERLAP_CHECK})
endif()

simple_testing(placement-sampler-no-beamline  "--file=sampler_placement_no_beamline.gmad"                ${OVERLAP_CHECK})
