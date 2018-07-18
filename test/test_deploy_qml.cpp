#include <gtest/gtest.h>
#include "../src/deploy-qml.hpp"

namespace LINUXDEPLOY_PLUGGIN_QT_TESTS {
    class TestDeployQml : public testing::Test {

    public:
        boost::filesystem::path appDirPath;
        boost::filesystem::path projectQmlRoot;

        void SetUp() {
            appDirPath = "/tmp/linuxdeploy-plugin-qt-tests-appdir";

            projectQmlRoot = appDirPath.string() + "/usr/qml";
            try {
                boost::filesystem::create_directories(projectQmlRoot);
                boost::filesystem::copy_file(TESTS_DATA_DIR "/qml_project/file.qml", projectQmlRoot.string() + "/file.qml");
            } catch (...){
            }

            setenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY, TESTS_DATA_DIR, 1);
        }

        void TearDown() {
            boost::filesystem::remove_all(appDirPath);
            unsetenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY);
        }
    };

    TEST_F(TestDeployQml, find_qmlimporter_path) {
        auto result = find_qmlimportscanner_binary_path();
        boost::filesystem::path expected = "/usr/bin/qmlimportscanner";

        ASSERT_FALSE(result.empty());
        ASSERT_EQ(result.string(), expected.string());
    }

    TEST_F(TestDeployQml, run_qmlimportscanner) {
        auto result = run_qmlimportscanner(projectQmlRoot,
                                           {TESTS_DATA_DIR, "/usr/lib/x86_64-linux-gnu/qt5/qml/"});
        ASSERT_FALSE(result.empty());
        std::cout << result;
    }

    TEST_F(TestDeployQml, run_qmlimportscanner_without_qml_import_paths) {
        auto result = run_qmlimportscanner(projectQmlRoot, {});
        ASSERT_FALSE(result.empty());
        std::cout << result;
    }

    TEST_F(TestDeployQml, get_default_qml_import_path) {
        auto result = get_default_qml_import_path();
        ASSERT_FALSE(result.empty());
    }

    TEST_F(TestDeployQml, get_qml_imports) {
        auto results = get_qml_imports(projectQmlRoot);
        ASSERT_FALSE(results.empty());
        std::cout << "Imported Qml Modules Found:";
        for (auto result: results) {
            std::cout << "\n\tName: " << result.name << "\n\tPath: " << result.path << "\n\tRelative path:"
                      << result.relativePath << std::endl;
            ASSERT_FALSE(result.path.empty());
            ASSERT_FALSE(result.relativePath.empty());
        }
    }

    TEST_F(TestDeployQml, deploy_qml_imports) {
        linuxdeploy::core::appdir::AppDir appDir(appDirPath);
        deploy_qml(appDir);
        appDir.executeDeferredOperations();

        ASSERT_TRUE(boost::filesystem::exists(projectQmlRoot.string() + "/QtQuick.2"));
        ASSERT_TRUE(boost::filesystem::exists(projectQmlRoot.string() + "/qml_module"));
    }
}