set(APPNAME "mp3decapp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/codec/mp3)

set(SRCS
  main.cpp
)

set(EXE_SRCS
  main.cpp
)

set(EXTRA_LINK_LIBRARIES
  mp3dec_static
)

build_exe("${APPNAME}" "${EXE_SRCS}" "${EXTRA_LINK_LIBRARIES}" "")
