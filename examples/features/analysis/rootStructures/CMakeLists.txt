configure_file(${CMAKE_CURRENT_SOURCE_DIR}/loadLibraries.C.in ${CMAKE_CURRENT_BINARY_DIR}/loadLibraries.C)

simple_root_test("root-library-load" loadLibraries.C "Error")

# create and destroy an instance of the analysis classes in the libs we provide
simple_root_test("root-options" optionsTest.C "Error")
simple_root_test("root-model"   modelTest.C   "Error")
simple_root_test("root-run"     runTest.C     "Error")
simple_root_test("root-event"   eventTest.C   "Error")

