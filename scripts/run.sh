#!/bin/sh

. scripts/env.sh

WORK_DIR=$1

if [ -z "$WORK_DIR" ]; then
    WORK_DIR=$(pwd)
fi

OS=$(uname)

if [ "$OS" = "Darwin" ]; then
    cd "$MAC_OS_BIN_PATH" || exit
    ./bmp-zipper --dir "$WORK_DIR"
elif [ "$OS" = "Linux" ]; then
    cd "$LINUX_OS_BIN_PATH" || exit
    ./bmp-zipper --dir "$WORK_DIR"
else
    echo "Unknown OS: $OS"
fi