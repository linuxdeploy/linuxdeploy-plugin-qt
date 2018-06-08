add_executable(linuxdeploy-plugin-qt main.cpp qt-modules.h)
target_link_libraries(linuxdeploy-plugin-qt linuxdeploy_core args)
set_target_properties(linuxdeploy-plugin-qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
