@echo off

rmdir "release" /s /q
cmake --workflow --preset release
cmake --install "release"