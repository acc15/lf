#!/bin/sh

rm -rf build
cmake --workflow --preset release
cmake --install build
