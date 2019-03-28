set(LIBNAME "mp3dec_obj")
set(LIBNAME_STATIC "mp3dec_static")
set(LIBNAME_SHARED "mp3dec")

set(HDRS
)

set(SRCS
   lib_wrapper.cpp
)

set(EXTRA_LINK_LIBRARIES
)

set(DEFINES)

# Create object library (as intermediate target)
set(OBJECT_SRCS ${HDRS} ${SRCS})
build_object_library("${LIBNAME}" "${OBJECT_SRCS}" "${DEFINES}")

set(LIB_SRCS ${HDRS} $<TARGET_OBJECTS:${LIBNAME}>)

# Shared library is installed to shared package
build_shared_library(${LIBNAME_SHARED}
  "${LIB_SRCS}"
  "${EXTRA_LINK_LIBRARIES}"
  ${CMAKE_CURRENT_SOURCE_DIR}
  ""
)
install_target_binary(${LIBNAME_SHARED} shared CDraalMp3SharedConfig CDraal::)

# Static library is installed to static package
build_static_library(${LIBNAME_STATIC}
  "${LIB_SRCS}"
  "${EXTRA_LINK_LIBRARIES}"
  ${CMAKE_CURRENT_SOURCE_DIR}
  ""
)
install_target_binary(${LIBNAME_STATIC} static CDraalMp3StaticConfig CDraal::)
