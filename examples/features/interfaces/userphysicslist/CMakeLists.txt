cmake_minimum_required(VERSION 3.2)
project(userphysicslist)

# point cmake to my own custom installation directory that's not a system dir
# this is where BDSIMConfig.cmake exists
set(CMAKE_PREFIX_PATH /Users/nevay/physics/reps/bdsim-develop-install/lib/cmake/bdsim)

# find the package and set up variables
find_package(BDSIM REQUIRED)

file(GLOB SOURCES "src/*.cc")
add_library(uphysics SHARED ${SOURCES})
target_include_directories(uphysics PUBLIC include ${BDSIM_INCLUDE_DIR})
target_link_libraries(uphysics ${BDSIM_LIBRARIES})

# make a program and link to udipole source code library
add_executable(bdsimuserpl userphysicslist)
target_include_directories(bdsimuserpl PUBLIC include {BDSIM_INCLUDE_DIR})
target_link_libraries(bdsimuserpl uphysics)
