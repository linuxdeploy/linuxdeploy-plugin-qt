cmake_minimum_required(VERSION 3.6)
project(linuxdeploy-plugin-qt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)
