#!/bin/bash

readonly scriptdir="$(dirname "$(readlink -f "$0")")"
readonly debug_dir=$scriptdir/build/debug
readonly release_dir=$scriptdir/build/release

echo "Building Debug"
cmake -S $scriptdir -B $debug_dir -DCMAKE_BUILD_TYPE=Debug\
    && cmake --build $debug_dir -j 4

echo -e "\nBuilding Release"
cmake -S $scriptdir -B $release_dir -DCMAKE_BUILD_TYPE=Release\
    && cmake --build $release_dir -j 4
