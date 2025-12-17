cmake_minimum_required(VERSION 3.20)
project(fvm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_RPATH "/usr/local/gcc-14.1.0/lib64")
set(CMAKE_INSTALL_RPATH "/usr/local/gcc-14.1.0/lib64")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(fvm
    src/main.cpp
)

find_package(fmt REQUIRED)
target_link_libraries(fvm PRIVATE fmt::fmt)
target_include_directories(fvm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(fvm PRIVATE -Wall -Wextra -Wpedantic)
