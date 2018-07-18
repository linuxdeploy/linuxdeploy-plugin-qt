// library includes
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

// local includes
#include "../src/qml.h"
#include "../src/util.h"

namespace bf = boost::filesystem;

namespace LINUXDEPLOY_PLUGGIN_QT_TESTS {
    class TestDeployQml : public testing::Test {

    public:
        boost::filesystem::path appDirPath;
        boost::filesystem::path projectQmlRoot;
        boost::filesystem::path defaultQmlImportPath;

        std::string generateRandomString(int length) {
            static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
            std::string result;
            result.resize(length);

            for (int i = 0; i < length; i++)
                result[i] = charset[rand() % charset.length()];

            return result;
        }

        void SetUp() override {
            srand(time(NULL));
            appDirPath = "/tmp/linuxdeploy-plugin-qt-tests-appdir-" + generateRandomString(8);

            projectQmlRoot = appDirPath.string() + "/usr/qml";
            try {
                boost::filesystem::create_directories(projectQmlRoot);
                boost::filesystem::copy_file(TESTS_DATA_DIR "/qml_project/file.qml",
                                             projectQmlRoot.string() + "/file.qml");
            } catch (...) {
            }

            setenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY, TESTS_DATA_DIR, 1);

            defaultQmlImportPath = getQmlImportPath();
        }

        void TearDown() override {
            boost::filesystem::remove_all(appDirPath);
            unsetenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY);
        }

        bf::path getQmlImportPath() {
            const auto &qmakePath = findQmake();
            return queryQmake(qmakePath)["QT_INSTALL_QML"];
        }
    };

    TEST_F(TestDeployQml, find_qmlimporter_path) {
        auto result = findQmlImportScanner();
        boost::filesystem::path expected = "/usr/bin/qmlimportscanner";

        ASSERT_FALSE(result.empty());
        ASSERT_EQ(result.string(), expected.string());
    }

    TEST_F(TestDeployQml, runQmlImportScanner) {
        auto result = runQmlImportScanner(projectQmlRoot,
                                          {TESTS_DATA_DIR, defaultQmlImportPath});
        ASSERT_FALSE(result.empty());
        std::cout << result;
    }

    TEST_F(TestDeployQml, run_qmlimportscanner_without_qml_import_paths) {
        auto result = runQmlImportScanner(projectQmlRoot, {});
        ASSERT_FALSE(result.empty());
        std::cout << result;
    }

    TEST_F(TestDeployQml, getQmlImports) {
        auto results = getQmlImports(projectQmlRoot, defaultQmlImportPath);
        ASSERT_FALSE(results.empty());
        std::cout << "Imported Qml Modules Found:";
        for (auto result : results) {
            std::cout << "\n\tName: " << result.name << "\n\tPath: " << result.path << "\n\tRelative path:"
                      << result.relativePath << std::endl;
            ASSERT_FALSE(result.path.empty());
            ASSERT_FALSE(result.relativePath.empty());
        }
    }

    TEST_F(TestDeployQml, deploy_qml_imports) {
        linuxdeploy::core::appdir::AppDir appDir(appDirPath);
        deployQml(appDir, defaultQmlImportPath);
        appDir.executeDeferredOperations();

        ASSERT_TRUE(boost::filesystem::exists(projectQmlRoot.string() + "/QtQuick.2"));
        ASSERT_TRUE(boost::filesystem::exists(projectQmlRoot.string() + "/qml_module"));
    }
}
