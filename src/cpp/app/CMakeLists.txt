set(APPNAME "mp3decapp")

set(SRCS
  main.cpp
)

set(EXE_SRCS
  main.cpp
  $<TARGET_OBJECTS:mp3dec_obj>
)

build_exe("${APPNAME}" "${EXE_SRCS}" "" "")
