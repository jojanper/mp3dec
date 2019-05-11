set(LIBNAME "mp3codec")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

set(HDRS
  mstream.h
)

set(SRCS
  common.cpp
  mstream.cpp
  sfb.cpp
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
