#!/bin/sh

BUILD_DIR="build/release"
DEST_DIR="$HOME/.local/bin/"

rm -rf "$BUILD_DIR"
cmake . -B "$BUILD_DIR" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --target lf
cmake --install "$BUILD_DIR"