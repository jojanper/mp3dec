include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

if (BUILD_WEBASSEMBLY)
  add_definitions("-DHAVE_WEBASSEMBLY")
endif(BUILD_WEBASSEMBLY)

add_subdirectory(lib)

if(BUILD_APPS)
  add_subdirectory(app)
endif(BUILD_APPS)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif(BUILD_TESTS)
