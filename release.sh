#!/bin/sh

BUILD_DIR="build/release"

rm -rf "$BUILD_DIR"
cmake --preset default .
cmake --build "$BUILD_DIR" --target lf --config Release
cmake --install "$BUILD_DIR"