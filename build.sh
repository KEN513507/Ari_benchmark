#!/bin/bash
# ants のビルドスクリプト: ./build.sh でコンパイル
set -e
cd "$(dirname "$0")"
g++ -O3 -march=native -std=c++20 ants.cpp \
    -Iglad_output/include \
    -Iglm \
    glad_output/src/glad.c \
    $(pkg-config --cflags --libs sdl2) \
    -lGL -ldl \
    -o ants
# --- v0.1 Phase 1: core skeleton test target ---
# Usage: ./build.sh core | ./build.sh all
if [ "${1:-}" = "core" ] || [ "${1:-}" = "all" ]; then
g++ -O3 -march=native -std=c++20 -Wall -Wextra -Werror \
    src/core_test.cpp \
    -Isrc \
    -o core_test
echo CORE_BUILD_OK
./core_test
fi
if [ "${1:-}" = "core" ]; then
exit 0
fi
# Grasshopper build
g++ -O3 -march=native -std=c++20 grasshopper.cpp \
    -Iglad_output/include -Iglm \
    glad_output/src/glad.c \
    $(pkg-config --cflags --libs sdl2) -lGL -ldl \
    -o grasshopper
echo GRASSHOPPER_BUILD_OK
echo BUILD_OK
