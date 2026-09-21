#!/bin/bash
# ants のビルドスクリプト: ./build.sh でコンパイル
set -e
cd "$(dirname "$0")"
# Part 2A: headless Nest Context tests.
if [ "${1:-}" = "test" ]; then
test_binary=$(mktemp /tmp/ari-test-nest.XXXXXX)
trap 'rm -f "$test_binary"' EXIT
g++ -O3 -march=native -std=c++20 -Wall -Wextra -Werror \
    src/nest/NestGrid.cpp src/nest/NestValidator.cpp test/test_nest.cpp \
    -Isrc $(pkg-config --cflags --libs gtest_main) -pthread \
    -o "$test_binary"
echo NEST_BUILD_OK
"$test_binary"
exit 0
fi
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
# --- Grasshopper mesh regression test (antennae/wings/eyes) ---
# Usage: ./build.sh mesh | ./build.sh all
if [ "${1:-}" = "mesh" ] || [ "${1:-}" = "all" ]; then
g++ -O2 -std=c++20 -Wall -Wextra -Werror \
    src/grasshopper_mesh_test.cpp \
    -Iglad_output/include -Iglm \
    glad_output/src/glad.c \
    $(pkg-config --cflags --libs sdl2) \
    -lGL -ldl \
    -o grasshopper_mesh_test
echo MESH_BUILD_OK
./grasshopper_mesh_test
fi
if [ "${1:-}" = "mesh" ]; then
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
