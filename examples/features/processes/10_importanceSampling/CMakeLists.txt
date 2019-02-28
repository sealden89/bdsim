simple_testing(processes-importance-sampling    "--file=importanceSampling.gmad"   "")
simple_testing(processes-importance-sampling-gz "--file=importanceSamplingGZ.gmad" "")
simple_fail(processes-importance-sampling-fail  "--file=importanceSamplingFail.gmad" "")
