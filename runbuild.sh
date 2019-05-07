#!/bin/bash
#
# Build code using CMake.
#
BUILD_TYPE=${1-'Debug'}
BUILD_DIR=${2-'build'}
CWD=`pwd`
OSNAME=`uname`

rm -Rf ${BUILD_DIR}/*
mkdir -p ${BUILD_DIR}

# Build library, apps and tests
cmake -H. -B./${BUILD_DIR} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cd ${BUILD_DIR}
cmake --build . --config ${BUILD_TYPE}

# Execute tests
GTEST_COLOR=1 ctest --verbose
