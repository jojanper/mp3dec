set(TEST_SUITE_NAME libTests)

set(SRCS
  core/test_bits.cpp
  core/test_filebuf.cpp
  core/test_bitsring.cpp
  core/test_utils.cpp
  $<TARGET_OBJECTS:mp3dec_obj>
)

set(EXTRA_LINK_LIBRARIES
)

build_unittest(${TEST_SUITE_NAME} "${SRCS}" "${EXTRA_LINK_LIBRARIES}")
