set(LIBNAME "codecmcu")

include_directories(${CMAKE_SOURCE_DIR}/..)

set(HDRS
)

set(SRCS
  mp3dec.cpp
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
