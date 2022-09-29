find_package(Threads)

# read Git revision ID
# WARNING: this value will be stored in the CMake cache
# to update it, you will have to reset the CMake cache
# (doesn't matter for CI builds like Travis for instance, where there's no permanent CMake cache)
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# set version and build number
set(VERSION 1-alpha)
if(DEFINED ENV{GITHUB_RUN_NUMBER})
    set(BUILD_NUMBER "GitHub actions build $ENV{GITHUB_RUN_NUMBER}")
else()
    set(BUILD_NUMBER "<local dev build>")
endif()

# get current date
execute_process(
    COMMAND env LC_ALL=C date -u "+%Y-%m-%d %H:%M:%S %Z"
    OUTPUT_VARIABLE DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# TODO: CMake <= 3.7 (at least!) doesn't allow for using OBJECT libraries with target_link_libraries
add_library(linuxdeploy-plugin-qt_util STATIC util.cpp util.h)
target_include_directories(linuxdeploy-plugin-qt_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(linuxdeploy-plugin-qt_util linuxdeploy_core args Threads::Threads)

add_executable(linuxdeploy-plugin-qt main.cpp qt-modules.h qml.cpp qml.h deployment.h)
target_link_libraries(linuxdeploy-plugin-qt linuxdeploy_core args nlohmann_json::nlohmann_json linuxdeploy-plugin-qt_util Threads::Threads)
set_target_properties(linuxdeploy-plugin-qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
target_compile_definitions(linuxdeploy-plugin-qt
    PRIVATE -DLD_GIT_COMMIT="${GIT_COMMIT}"
    PRIVATE -DLD_VERSION="${VERSION}"
    PRIVATE -DLD_BUILD_NUMBER="${BUILD_NUMBER}"
    PRIVATE -DLD_BUILD_DATE="${DATE}"
)

if(STATIC_BUILD)
    message(WARNING "static builds enabled")
    cmake_minimum_required(VERSION 3.13)
    target_link_options(linuxdeploy-plugin-qt PUBLIC -static -static-libgcc -static-libstdc++)
endif()

add_subdirectory(deployers)
target_link_libraries(linuxdeploy-plugin-qt deployers)

install(
    TARGETS linuxdeploy-plugin-qt
    RUNTIME DESTINATION bin
)
