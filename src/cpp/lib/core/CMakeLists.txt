set(LIBNAME "coreutils")

set(HDRS
)

set(SRCS
   iobuf.cpp
   bitsbuffer.cpp
   bits.cpp
   bitsring.cpp
)

set(EXTRA_LINK_LIBRARIES
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
