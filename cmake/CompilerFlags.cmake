# warning flags
add_compile_options(-Wall -Wextra -pedantic -Wpointer-arith -Woverloaded-virtual -Wnon-virtual-dtor -Wshadow)

# stop warnings from Clang version (for versions 7 and greater)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  # remove warning about lex.yy.cc
  add_compile_options(-Wno-unneeded-internal-declaration)
  if (CLANG_VERSION_STRING VERSION_GREATER 6.9)
    add_compile_options(-Wno-keyword-macro)
  endif()
  if (CLANG_VERSION_STRING VERSION_GREATER 8.0)
    add_compile_options(-Wno-undefined-var-template)
  endif()
  if (CLANG_VERSION_STRING VERSION_GREATER 10.0)
    add_compile_options(-Wno-register)
  endif()
endif()

if(APPLE)
  # For MacOS bundle
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -headerpad_max_install_names")
  if($ENV{VERBOSE})
    message(STATUS "CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}")
  endif()
endif()

if($ENV{VERBOSE})
  message(STATUS "CMAKE_C_FLAGS ${CMAKE_C_FLAGS}")
  message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()
