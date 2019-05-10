set(APPNAME "mp3decapp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/codec/mp3)

set(SRCS
  main.cpp
)

set(EXE_SRCS
  main.cpp
  $<TARGET_OBJECTS:mp3dec_obj>
)

build_exe("${APPNAME}" "${EXE_SRCS}" "" "")
