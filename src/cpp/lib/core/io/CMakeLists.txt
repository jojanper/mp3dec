set(LIBNAME "io")

set(HDRS
)

set(SRCS
   iobuf.cpp
   console.cpp
   log.cpp
   uci.cpp
   wave.cpp
)

set(EXTRA_LINK_LIBRARIES
)

set(DEFINES)

# Create object library (as intermediate target)
if (NOT BUILD_WEBASSEMBLY)
   set(OBJECT_SRCS ${HDRS} ${SRCS})
   build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
endif(NOT BUILD_WEBASSEMBLY)
