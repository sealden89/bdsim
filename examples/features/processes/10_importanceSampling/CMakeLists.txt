simple_testing(processes-importance-sampling              "--file=importanceSampling.gmad"   "")
simple_testing(processes-importance-sampling-pyg4-prepend "--file=importanceSamplingWithPrepend.gmad" "")
if (USE_GZSTREAM)
  simple_testing(processes-importance-sampling-gz "--file=importanceSamplingGZ.gmad" "")
endif()

simple_fail(processes-importance-sampling-fail  "--file=importanceSamplingFail.gmad" "")
