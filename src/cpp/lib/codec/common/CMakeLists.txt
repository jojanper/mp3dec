set(LIBNAME "commoncodec")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

set(HDRS
  hufinfo.h
  huftree.h
)

set(SRCS
  huftree.cpp
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
