cmake_minimum_required(VERSION 3.5)
project(mp3dec)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)

option(BUILD_APPS "Build test apps" ON)
option(BUILD_TESTS "Build gtest unit tests" ON)
option(BUILD_WEBASSEMBLY "Build as WebAssembly" OFF)
option(HAVE_THREADS "Use pthread in unit tests" ON)
option(HAVE_COVERAGE "Enable code coverage" ON)

# Load all cmake modules
file(GLOB cmakeFiles ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
foreach(cmakeFile ${cmakeFiles})
  message("-- Include ${cmakeFile}")
  include(${cmakeFile})
endforeach(cmakeFile)

# Project specific config
include(${PROJECT_SOURCE_DIR}/projectConfig.cmake)

# Unit testing: gtest as external component
if(BUILD_TESTS)
  if(HAVE_THREADS)
    find_package(Threads REQUIRED)
  endif(HAVE_THREADS)

  add_subdirectory(${PROJECT_SOURCE_DIR}/extProjects)
  include_directories(${GTEST_INCLUDE_DIRS})

  include(CTest)
  enable_testing()
endif(BUILD_TESTS)

# Include source codes
add_subdirectory(src)

# Set up cpack
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_NAME})
set(CPACK_GENERATOR "ZIP")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
include(CPack)

# Show system settings
message("System name         ${CMAKE_SYSTEM_NAME}")
message("Build options:")
message(" C Compiler         ${CMAKE_C_COMPILER}")
message(" C++ Compiler       ${CMAKE_CXX_COMPILER}")
message(" Build type         ${CMAKE_BUILD_TYPE}")
message(" C_FLAGS            ${CMAKE_CXX_FLAGS}")
message(" CXX_FLAGS          ${CMAKE_CXX_FLAGS}")
message(" Apps               ${BUILD_APPS}")
message(" Tests              ${BUILD_TESTS}")
message("  -threads:         ${HAVE_THREADS}")
message("  -thread library:  ${CMAKE_THREAD_LIBS_INIT}")
message(" Package            ${PACKAGE_NAME}")
