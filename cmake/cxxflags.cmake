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
