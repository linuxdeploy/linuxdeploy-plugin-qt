enable_testing()

add_executable(linuxdeploy-plugin-qt-tests test_deploy_qml.cpp test_main.cpp)
target_link_libraries(linuxdeploy-plugin-qt-tests  gtest)

add_test(linuxdeploy-plugin-qt-tests linuxdeploy-plugin-qt-tests)
