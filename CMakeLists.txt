cmake_minimum_required(VERSION 3.5)
project(mp3dec)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)

# Load all cmake modules
file(GLOB cmakeFiles ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
foreach(cmakeFile ${cmakeFiles})
  message("-- Include ${cmakeFile}")
  include(${cmakeFile})
endforeach(cmakeFile)

# Project specific config
include(${PROJECT_SOURCE_DIR}/projectConfig.cmake)

option(BUILD_APPS "Build test apps" ON)
option(BUILD_TESTS "Build gtest unit tests" ON)
option(HAVE_THREADS "Use pthread in unit tests" ON)

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/extProjects)

# Unit testing: gtest as external component
if(BUILD_TESTS)
  if(HAVE_THREADS)
    find_package(Threads REQUIRED)
  endif(HAVE_THREADS)

  find_program(MEMORYCHECK_COMMAND valgrind )
  set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --show-reachable=yes --undef-value-errors=yes --leak-check=full --track-origins=yes --error-exitcode=1")
  # --gen-suppressions=all --log-file=valgrind_log.txt --error-limit=no" )
  set(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppressions.txt" )
  #file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/resources/valgrind_supp.xml" MEMORYCHECK_SUPPRESSIONS_FILE)
  #add_custom_target(valgrind
  #      COMMAND "${VALGRIND}" --tool=memcheck --show-reachable=yes --undef-value-errors=yes --leak-check=full --track-origins=yes --error-exitcode=1
  #)

  #add_custom_target(test_memcheck
  #  COMMAND ${CMAKE_CTEST_COMMAND}
  #      --force-new-ctest-process --test-action memcheck --output-on-failure
  #  COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log")

  include(CTest)
  enable_testing()
  add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
  include_directories(${GTEST_INCLUDE_DIRS})
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
