@echo off

set BUILD_DIR="build\release"

rmdir "%BUILD_DIR%" /s /q
cmake --preset default .
cmake --build "%BUILD_DIR%" --target lf --config Release
cmake --install "%BUILD_DIR%"