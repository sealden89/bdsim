rebdsim_test(analysis-rebdsim                 "analysisConfig.txt")
rebdsim_test(analysis-rebdsim-multiple-files  "multipleFilesAnalysis.txt")
rebdsim_test(analysis-rebdsim-selection       "selection.txt")
rebdsim_test(analysis-rebdsim-event-range     "analysisConfigRange.txt")
rebdsim_test(analysis-rebdsim-uneven-binning  "unevenBinning.txt")

rebdsim_test_fail(analysis-uneven-binning-bad1      "unevenBinning-bad1.txt")
rebdsim_test_fail(analysis-uneven-binning-bad2      "unevenBinning-bad2.txt")
rebdsim_test_fail(analysis-bad-binning-x            "analysisCongig-bad-binning-x.txt")
rebdsim_test_fail(analysis-bad-binning-y            "analysisCongig-bad-binning-y.txt")
rebdsim_test_fail(analysis-bad-binning-z            "analysisCongig-bad-binning-z.txt")


# tests that should fail
add_test(NAME analysis-rebdsim-event-range-invalid COMMAND rebdsimExec analysisConfigRangeInvalid.txt)
set_tests_properties(analysis-rebdsim-event-range-invalid PROPERTIES WILL_FAIL 1)

rebdsim_combine_test(analysis-combine combined-ana.root ../../data/ana1.root ../../data/ana2.root)

rebdsim_histomerge_test(analysis-histomerge ../../data/sample1.root histomerge.root )