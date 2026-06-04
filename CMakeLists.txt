cmake_minimum_required(VERSION 3.20)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Testing stuff
enable_testing()
add_subdirectory(tests)

add_library(chip8 src/Chip8.cpp)

target_include_directories(chip8  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

