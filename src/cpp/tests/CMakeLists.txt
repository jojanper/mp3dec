set(TEST_SUITE_NAME libTests)

set(SRCS
  core/test_bits.cpp
  core/test_filebuf.cpp
  core/test_bitsring.cpp
  core/test_utils.cpp
  core/test_membuffer.cpp
  mcu/test_streamable.cpp
)

set(EXTRA_LINK_LIBRARIES
  mp3dec_static
)

build_unittest(${TEST_SUITE_NAME} "${SRCS}" "${EXTRA_LINK_LIBRARIES}")
