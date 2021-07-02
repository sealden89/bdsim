# tests for running a root macro in batch mode


# run a root macro in batch mode
macro(root_test test_name input_args)
  # convert input arguments into a cmake list to separate
  set(args ${input_args})
  separate_arguments(args)
  set(TESTING_ARGS ${TESTING_ARGS})
  separate_arguments(TESTING_ARGS)
  add_test(NAME ${test_name} COMMAND ${ROOT_EXECUTABLE} -b -q -l ${args} ${TESTING_ARGS})
  # Set ROOT_INCLUDE_PATH
  if (USE_BOOST)
    set_tests_properties(${test_name}
      PROPERTIES ENVIRONMENT ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/include:${CMAKE_SOURCE_DIR}/analysis:${Boost_INCLUDE_DIRS})
  else()
    set_tests_properties(${test_name}
      PROPERTIES ENVIRONMENT ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/include:${CMAKE_SOURCE_DIR}/analysis)
  endif ()
  # unset TESTING_ARGS so only used for this test
  unset(TESTING_ARGS)
endmacro()

# run a root macro in batch mode and look for a certain string - if found, the test fails
macro(simple_root_test test_name args expression)
  root_test(${test_name} ${args})
  if(NOT "${expression}" STREQUAL "")
        set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${expression}")
  endif()
endmacro()

# similarly, run a macro in batch with regular expression checking and with LONG label
macro(simple_root_test_long test_name args expression)
  simple_root_test(${test_name} ${args})
  set_tests_properties(${test_name} PROPERTIES LABELS LONG)
endmacro()