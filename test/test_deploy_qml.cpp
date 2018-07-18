#include <gtest/gtest.h>
#include "../src/deploy-qml.hpp"

namespace LINUXDEPLOY_PLUGGIN_QT_TESTS {
    class TestDeployQml : public testing::Test {

    public:
    };

    TEST_F(TestDeployQml, find_qmlimporter_path) {
        auto result = find_qmlimportscanner_binary_path();
        boost::filesystem::path expected = "/usr/bin/qmlimportscanner";

        ASSERT_FALSE(result.empty());
        ASSERT_EQ(result.string(), expected.string());
    }

    TEST_F(TestDeployQml, run_qmlimportscanner) {
        auto result = run_qmlimportscanner(TESTS_DATA_DIR "/qml_project",
                                           {TESTS_DATA_DIR, "/usr/lib/x86_64-linux-gnu/qt5/qml/"});
        ASSERT_FALSE(result.empty());
    }

    TEST_F(TestDeployQml, get_default_qml_import_path) {
        auto result = get_default_qml_import_path();
        ASSERT_FALSE(result.empty());
    }

    TEST_F(TestDeployQml, get_qml_imports) {
        auto results = get_qml_imports(TESTS_DATA_DIR "/qml_project",
                                       {TESTS_DATA_DIR});
        ASSERT_FALSE(results.empty());
        std::cout << "Imported Qml Modules Found:";
        for (auto result: results) {
            std::cout << "\n\tName: " << result.name << "\n\tPath: " << result.path << "\n\tRelative path:" << result.relativePath << std::endl;
        }
    }
}