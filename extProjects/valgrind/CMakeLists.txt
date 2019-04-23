find_program(MEMORYCHECK_COMMAND valgrind PARENT_SCOPE)

set(MEMORYCHECK_COMMAND_OPTIONS
    "--tool=memcheck --show-reachable=yes --undef-value-errors=yes --leak-check=full
    --track-origins=yes --error-exitcode=1"
    PARENT_SCOPE)

# --gen-suppressions=all --log-file=valgrind_log.txt --error-limit=no" )
set(MEMORYCHECK_SUPPRESSIONS_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/valgrind_suppressions.txt"
    PARENT_SCOPE)

    # Enable code coverage for debug build
if (HAVE_COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug")
  set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
  set(GCC_COVERAGE_LINK_FLAGS "-lgcov")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" PARENT_SCOPE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" PARENT_SCOPE)
endif(HAVE_COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug")
