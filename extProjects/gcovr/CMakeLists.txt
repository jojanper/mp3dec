set(GCOVR gcovr)

add_custom_command(OUTPUT run_gcovr_parser
  POST_BUILD
  COMMAND ${GCOVR} -d --root=${CMAKE_SOURCE_DIR} --gcov-exclude=".*gtest.*"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(coverage DEPENDS run_gcovr_parser)
