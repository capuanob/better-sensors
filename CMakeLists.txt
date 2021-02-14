cmake_minimum_required(VERSION 3.0.0)
project(better-sensors VERSION 1.0.0)

set(CMAKE_EXE_LINKER_FLAGS "-lsensors")
add_subdirectory(src)
