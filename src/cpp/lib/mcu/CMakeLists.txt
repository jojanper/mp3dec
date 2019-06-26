set(LIBNAME "codecmcu")

include_directories(${CMAKE_SOURCE_DIR}/..)

set(HDRS
)

set(SRCS
  decoders/mp3dec.cpp
  streamable.cpp
)

# Do not include file I/O related implementations to WebAssembly
if (NOT BUILD_WEBASSEMBLY)
  set(SRCS
    "${SRCS}"
    console/mp3decconsole.cpp
    consoledecoder.cpp
  )
endif(NOT BUILD_WEBASSEMBLY)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
