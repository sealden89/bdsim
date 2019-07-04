# configure tester with correct BDSIM and MadX executables
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Globals.py.in ${CMAKE_CURRENT_BINARY_DIR}/Globals.py)

