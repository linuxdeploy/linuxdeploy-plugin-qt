# common CMake macro for linuxdeploy, adding tests only if they are supposed to be built
# otherwise if you try to run CTest the tests aren't build and CTest can't find them
if(NOT COMMAND ld_add_test)
    function(ld_add_test TARGET_NAME)
        get_target_property(${TARGET_NAME}_EXCLUDE_FROM_ALL ${TARGET_NAME} EXCLUDE_FROM_ALL)
        get_directory_property(PROJECT_EXCLUDE_FROM_ALL DIRECTORY ${PROJECT_SOURCE_DIR} EXCLUDE_FROM_ALL)

        if(NOT ${TARGET_NAME}_EXCLUDE_FROM_ALL AND NOT PROJECT_EXCLUDE_FROM_ALL)
            message(STATUS "[${PROJECT_NAME}] Adding test ${TARGET_NAME}")

            # https://stackoverflow.com/a/35697020
            add_test(NAME ${TARGET_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}>)
        else()
            message(STATUS "[${PROJECT_NAME}] Test ${TARGET_NAME} is excluded from ALL, not adding as test")
        endif()
    endfunction()
endif()

add_executable(linuxdeploy-plugin-qt-tests test_main.cpp test_deploy_qml.cpp ../src/qml.cpp)
target_link_libraries(linuxdeploy-plugin-qt-tests linuxdeploy_core args json gtest linuxdeploy-plugin-qt_util)
target_compile_definitions(linuxdeploy-plugin-qt-tests PRIVATE TESTS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

ld_add_test(linuxdeploy-plugin-qt-tests)

