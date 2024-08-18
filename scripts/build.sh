#!/bin/sh

. scripts/env.sh

NINJA_PATH=$1
QT_PATH=$2

if [ -z "$NINJA_PATH" ]; then
    echo "Ninja executable is not specified as first argument"
fi
if [ -z "$QT_PATH" ]; then
    echo "The Path to Qt binaries is not specified as second argument"
else
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM="$NINJA_PATH" -DCMAKE_PREFIX_PATH="$QT_PATH" -G Ninja -S "$SOURCE_DIR" -B "$DEBUG_BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="$NINJA_PATH" -DCMAKE_PREFIX_PATH="$QT_PATH" -G Ninja -S "$SOURCE_DIR" -B "$RELEASE_BUILD_DIR"
    cmake --build cmake-build-debug --target all -j6
    cmake --build cmake-build-release --target all -j6
fi