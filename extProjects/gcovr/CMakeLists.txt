#project(gcovr_builder)
#include(ExternalProject)

#ExternalProject_Add(
#  gcovr
#  URL https://github.com/gcovr/gcovr/archive/4.1.zip
#  GIT_TAG        4.1
#  #URL https://github.com/gcovr/gcovr/archive/3.2.zip
#  #URL_HASH SHA1=7411d3989116c5fa65519ee1a54237df16944ad2
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#  INSTALL_COMMAND ""
#)

#ExternalProject_Get_Property(gcovr source_dir)
set(GCOVR gcovr)

#set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
#set(GCC_COVERAGE_LINK_FLAGS "-lgcov")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_custom_command(OUTPUT run_gcovr_parser
  POST_BUILD
  COMMAND ${GCOVR} -d --root=${CMAKE_SOURCE_DIR} --gcov-exclude=".*gtest.*"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(coverage DEPENDS run_gcovr_parser)
