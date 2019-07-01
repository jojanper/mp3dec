set(APPNAME "mp3decapp")
set(APPNAME2 "mp3streamapp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/codec/mp3)

set(EXE_SRCS_1
  file.cpp
  parse.cpp
)

set(EXE_SRCS_2
  stream.cpp
  parse.cpp
)

set(EXTRA_LINK_LIBRARIES
  mp3dec_static
)

build_exe("${APPNAME}" "${EXE_SRCS_1}" "${EXTRA_LINK_LIBRARIES}" "")
build_exe("${APPNAME2}" "${EXE_SRCS_2}" "${EXTRA_LINK_LIBRARIES}" "")
