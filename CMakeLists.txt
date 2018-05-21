cmake_minimum_required(VERSION 2.8)

project(x86CodeGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(USE_CLANG_TIDY "Run clang-tidy on build" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-potentially-evaluated-expression")

include_directories("include/")

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)
add_subdirectory(examples)

