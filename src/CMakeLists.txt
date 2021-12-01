# TODO: CMake <= 3.7 (at least!) doesn't allow for using OBJECT libraries with target_link_libraries
add_library(linuxdeploy-plugin-qt_util STATIC util.cpp util.h)
target_include_directories(linuxdeploy-plugin-qt_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(linuxdeploy-plugin-qt_util linuxdeploy_core args)

add_executable(linuxdeploy-plugin-qt main.cpp qt-modules.h qml.cpp qml.h deployment.h)
target_link_libraries(linuxdeploy-plugin-qt linuxdeploy_core args nlohmann_json::nlohmann_json linuxdeploy-plugin-qt_util)
set_target_properties(linuxdeploy-plugin-qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(deployers)
target_link_libraries(linuxdeploy-plugin-qt deployers)

install(
    TARGETS linuxdeploy-plugin-qt
    RUNTIME DESTINATION bin
)
