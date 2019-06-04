set(LIBNAME "codecmcu")

include_directories(${CMAKE_SOURCE_DIR}/..)

set(HDRS
)

set(SRCS
  decoders/mp3dec.cpp
  console/mp3decconsole.cpp
  consoledecoder.cpp
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
