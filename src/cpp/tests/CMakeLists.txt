set(TEST_SUITE_NAME libTests)

set(SRCS
  test_lib.cpp
  $<TARGET_OBJECTS:mp3dec_obj>
)

set(EXTRA_LINK_LIBRARIES
)

build_unittest(${TEST_SUITE_NAME} "${SRCS}" "${EXTRA_LINK_LIBRARIES}")
