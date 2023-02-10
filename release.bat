@echo off

rmdir "build" /s /q
cmake --workflow --preset release
cmake --install "build"