#!/bin/sh

DEBUG_BUILD_DIR="cmake-build-debug"
RELEASE_BUILD_DIR="cmake-build-release"

MAC_OS_BIN_PATH="$DEBUG_BUILD_DIR/bmp-zipper.app/Contents/MacOS"
LINUX_OS_BIN_PATH="$RELEASE_BUILD_DIR"

SOURCE_DIR="$(pwd)"

export DEBUG_BUILD_DIR
export RELEASE_BUILD_DIR
export MAC_OS_BIN_PATH
export LINUX_OS_BIN_PATH
export SOURCE_DIR