cmake_minimum_required(VERSION 3.6)
project(ProgressTracker)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -Wpedantic -Wunreachable-code
        -Wstrict-aliasing -Wno-sign-compare -Wno-unknown-pragmas)

set(SOURCE_FILES progress_tracker.cpp)
set(HEADER_FILES progress_tracker.h)
add_library(progresstracker STATIC ${SOURCE_FILES})
set_target_properties(progresstracker PROPERTIES PUBLIC_HEADER ${HEADER_FILES})
add_custom_target(library DEPENDS progresstracker) # alias "library"

add_executable(example main.cpp)
add_dependencies(example progresstracker)
target_link_libraries(example progresstracker)

add_executable(example_hpp main.cpp)
target_compile_definitions(example_hpp PRIVATE -DUSE_HPP)
