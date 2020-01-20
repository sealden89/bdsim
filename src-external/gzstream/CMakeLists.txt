# gzstream

if ($ENV{VERBOSE})
  message(STATUS "Building gzstream using library - ${ZLIB_LIB_LINK}")
endif()

# glob gzstream sources
file(GLOB GZSTREAM_SOURCES gzstream.C;)
file(GLOB GZSTREAM_HEADERS *.h)
if($ENV{VERBOSE})
  message(STATUS "GZSTREAM sources ${GZSTREAM_SOURCES}")
endif()

add_library(gzstream SHARED ${GZSTREAM_SOURCES})
target_link_libraries(gzstream ${ZLIB_LIBS})
# the $< expressions are 'generators' that mean the package is moveable for installation
target_include_directories(gzstream INTERFACE
		           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}:include/gzstream>)

bdsim_install_targets(gzstream)
