set(LIBNAME_STATIC "mp3dec_static")
set(LIBNAME_SHARED "mp3dec")

set(EXTRA_LINK_LIBRARIES
)

set(OBJECT_SRCS decoder.h decoder.cpp)
build_object_library("capi" "${OBJECT_SRCS}" "${DEFINES}")

# Final library will consists of these object libraries
set(LIB_SRCS
  $<TARGET_OBJECTS:commoncodec>
  $<TARGET_OBJECTS:mp3codec>
  $<TARGET_OBJECTS:coreutils>
  $<TARGET_OBJECTS:codecmcu>
  $<TARGET_OBJECTS:capi>
)

if (BUILD_WEBASSEMBLY)
  set(LIB_SRCS
    "${LIB_SRCS}"
  )

  # WebAssembly target is actually an executable target
  build_exe(${LIBNAME_STATIC} "${LIB_SRCS}" "${EXTRA_LINK_LIBRARIES}" "")
else()

  # Enhance build target with I/O library
  set(LIB_SRCS
    "${LIB_SRCS}"
    $<TARGET_OBJECTS:io>
  )

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
endif(BUILD_WEBASSEMBLY)
