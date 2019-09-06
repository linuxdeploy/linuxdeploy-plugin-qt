cmake_minimum_required(VERSION 3.6)
project(linuxdeploy-plugin-qt)

include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(src)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
