enable_testing()

add_executable(linuxdeploy-plugin-qt-tests test_main.cpp test_deploy_qml.cpp)
target_link_libraries(linuxdeploy-plugin-qt-tests linuxdeploy_core args json gtest)
target_compile_definitions(linuxdeploy-plugin-qt-tests PRIVATE TESTS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_test(linuxdeploy-plugin-qt-tests linuxdeploy-plugin-qt-tests)
