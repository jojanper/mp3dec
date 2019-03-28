# Macro to strip unwanted symbols
macro(strip_library target)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")

    set(STRIP_OPTIONS "")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      set(STRIP_OPTIONS "--strip-unneeded")
    endif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    add_custom_command(TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_STRIP} ${STRIP_OPTIONS} $<TARGET_FILE:${target}>
    )
  endif(CMAKE_BUILD_TYPE STREQUAL "Release")
endmacro()
