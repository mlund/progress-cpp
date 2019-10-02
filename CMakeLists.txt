cmake_minimum_required(VERSION 3.6)
project(ProgressBar)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -Wpedantic -Wunreachable-code
        -Wstrict-aliasing -Wno-sign-compare -Wno-unknown-pragmas)

set(SOURCE_FILES main.cpp)
add_executable(ProgressBar ${SOURCE_FILES})
