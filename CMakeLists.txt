cmake_minimum_required(VERSION 3.31)
project(file C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig)

add_subdirectory(src)