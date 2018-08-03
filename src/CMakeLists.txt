add_executable(linuxdeploy-plugin-qt main.cpp qt-modules.h qml.cpp qml.h util.h)
target_link_libraries(linuxdeploy-plugin-qt linuxdeploy_core args json)
set_target_properties(linuxdeploy-plugin-qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

install(
    TARGETS linuxdeploy-plugin-qt
    RUNTIME DESTINATION bin
)
