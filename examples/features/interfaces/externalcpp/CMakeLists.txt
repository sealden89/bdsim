cmake_minimum_required (VERSION 3.2)
project (externalcpp)

# point cmake to my own custom installation directory that's not a system dir
# this is where BDSIMConfig.cmake exists
set(CMAKE_PREFIX_PATH /Users/nevay/physics/reps/bdsim-develop-install/lib/cmake/bdsim)

# find the package and set up variables
find_package(BDSIM REQUIRED)

# define program includes
include_directories(${BDSIM_INCLUDE_DIR}/bdsim)

# make a program and link to gmad (parser) and bdsim libraries
add_executable(customprogram customprogram.cc)
target_link_libraries(customprogram gmad bdsim)
