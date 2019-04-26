find_program(MEMORYCHECK_COMMAND valgrind CACHE INTERNAL "" FORCE)

set(SUPPRESSIONS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/valgrind_suppressions.txt")

# To generate suppressions include to command options: --gen-suppressions=all
set(MEMORYCHECK_COMMAND_OPTIONS
    "--tool=memcheck --show-reachable=yes --undef-value-errors=yes --leak-check=full \
    --track-origins=yes --error-exitcode=1 --suppressions=${SUPPRESSIONS_FILE}"
    CACHE INTERNAL "" FORCE)
