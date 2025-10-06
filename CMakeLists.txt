cmake_minimum_required(VERSION 3.16)
project(grepr C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

add_executable(grepr
    src/main.c
    src/getopt.c
)