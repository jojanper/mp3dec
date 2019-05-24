set(LIBNAME "coreutils")

set(HDRS
)

set(SRCS
   bitsbuffer.cpp
   bits.cpp
   bitsring.cpp
   eqband.cpp
   meta.cpp
)

set(EXTRA_LINK_LIBRARIES
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")
