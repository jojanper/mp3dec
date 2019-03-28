#!/bin/bash
#
# Build code using CMake.
#
BUILD_TYPE=${1-'Debug'}
BUILD_DIR=${2-'build'}

rm -Rf ${BUILD_DIR}/*
mkdir -p ${BUILD_DIR}
cmake -H. -B./${BUILD_DIR} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cd ${BUILD_DIR}
cmake --build .
