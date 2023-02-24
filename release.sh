#!/bin/sh

rm -rf release
cmake --workflow --preset release
cmake --install release
