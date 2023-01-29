@echo off

set BUILD_DIR="build\release"

rmdir "%BUILD_DIR%" /s /q
cmake . -B "%BUILD_DIR%" -G Ninja -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake"
cmake --build "%BUILD_DIR%" --target lf --config release
cmake --install "%BUILD_DIR%"