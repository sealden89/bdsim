## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)

set(CTEST_PROJECT_NAME "BDSIM")
set(CTEST_NIGHTLY_START_TIME "00:02:00 GMT")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "jaiserv1.pp.rhul.ac.uk")
set(CTEST_DROP_LOCATION "/cdash/submit.php?project=BDSIM")
set(CTEST_DROP_SITE_CDASH TRUE)

#find_program(CTEST_GIT_COMMAND NAMES git)
set(CTEST_UPDATE_COMMAND "git")

#set(CTEST_CUSTOM_COVERAGE_EXCLUDE "*Dict.cc")