cmake_minimum_required(VERSION 3.6)
project(linuxdeploy-plugin-qt)

include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

if(BUILD_TESTING)
    # including this before including lib/ makes sure that the top level project's gtest is used everywhere
    include(${PROJECT_SOURCE_DIR}/lib/cmake-scripts/include-or-build-gtest.cmake)
endif()

add_subdirectory(lib)

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
