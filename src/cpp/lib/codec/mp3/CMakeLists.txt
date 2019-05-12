set(LIBNAME "mp3codec")

set(HDRS
  mstream.h
)

set(SRCS
  common.cpp
  mpheader.cpp
  mstream.cpp
  sfb.cpp
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
