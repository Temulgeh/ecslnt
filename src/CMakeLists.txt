# i do not know how to use cmake

cmake_minimum_required(VERSION 3.20.0) # i don't know i seem to have 3.25.0 so i just picked a few versions below lol
project(ecslnt VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_executable(
    ecslnt_main
    main.cpp
)
