set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-unused-const-variable -std=c99")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -ffunction-sections -fdata-sections")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -ffunction-sections -fdata-sections")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers -Wno-missing-braces")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-template-friend -Wno-missing-field-initializers -Wno-missing-braces")

  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL,--gc-sections")
endif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -ffunction-sections -fdata-sections")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -ffunction-sections -fdata-sections")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

# Enable code coverage for debug build (not for WebAssembly)
if (HAVE_COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT BUILD_WEBASSEMBLY)
  set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
  set(GCC_COVERAGE_LINK_FLAGS "-lgcov")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
endif(HAVE_COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT BUILD_WEBASSEMBLY)

# WebAssembly configurations
if (BUILD_WEBASSEMBLY)
  set(CMAKE_C_FLAGS_RELEASE "-O1")
  set(CMAKE_CXX_FLAGS_RELEASE "-O1")
  set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
  #set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif(BUILD_WEBASSEMBLY)
