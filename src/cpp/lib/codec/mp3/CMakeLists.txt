set(LIBNAME "mp3codec")

set(HDRS
  mstream.h
)

set(SRCS
  brate.cpp
  common.cpp
  huffman.cpp
  imdct2.cpp
  layer1.cpp
  layer2.cpp
  layer3.cpp
  mp3.cpp
  mp3_q.cpp
  mp3wind.cpp
  mpheader.cpp
  mstream.cpp
  sfb.cpp
  stereo.cpp
  synfilt.cpp
)

set(DEFINES)

if (BUILD_WEBASSEMBLY)
  set(DEFINES "-DHAVE_WEBASSEMBLY")
endif(BUILD_WEBASSEMBLY)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
