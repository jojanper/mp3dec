set(TEST_SUITE_NAME libTests)

set(SRCS
  test_lib.cpp
)

set(EXTRA_LINK_LIBRARIES
)

build_unittest(${TEST_SUITE_NAME} "${SRCS}" "${EXTRA_LINK_LIBRARIES}")
