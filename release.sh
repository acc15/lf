#!/bin/sh

DIR="build/release"

rm -rf "$DIR"
cmake . -B "$DIR" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build "$DIR" --target lf