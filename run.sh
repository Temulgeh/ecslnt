#!/bin/sh

# TEMP: probably give it parameters and separate the first cmake command to
# whenever i save in nvim

cd "${0%/*}"
cmake -S src -B build
cmake --build build
build/ecslnt_main
