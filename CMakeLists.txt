cmake_minimum_required(VERSION 3.6)
project(linuxdeploy-plugin-qt)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

if(BUILD_TESTING)
    # including this before including lib/ makes sure that the top level project's gtest is used everywhere
    include(${PROJECT_SOURCE_DIR}/lib/cmake-scripts/include-or-build-gtest.cmake)
endif()

# dependencies
find_package(nlohmann_json)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found on system, fetching from GitHub")

    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.10.4
    )

    FetchContent_MakeAvailable(nlohmann_json)

    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
endif()

add_subdirectory(lib)

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
