# comparator ctests

# run a comparison job
macro(comparator_test testName refFile testFile)
  add_test(NAME ${testName} COMMAND comparatorExec ${refFile} ${testFile})
  set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endmacro()