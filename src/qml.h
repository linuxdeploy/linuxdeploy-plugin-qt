// system headers
#include <filesystem>

// library includes
#include <linuxdeploy/core/appdir.h>

#pragma once

typedef struct {
    std::string name;
    std::filesystem::path path;
    std::filesystem::path relativePath;
} QmlModuleImport;

static const char* const ENV_KEY_QML_MODULES_PATHS = "QML_MODULES_PATHS";
static const char* const ENV_KEY_QML_SOURCES_PATHS = "QML_SOURCES_PATHS";

struct QmlImportScannerError : public std::runtime_error {
    explicit QmlImportScannerError(const std::string& message) : runtime_error(message) {}
};

// deploys QML files into AppDir
void deployQml(linuxdeploy::core::appdir::AppDir &appDir, const std::filesystem::path &installQmlPath);

std::filesystem::path findQmlImportScanner();

std::string runQmlImportScanner(const std::vector<std::filesystem::path> &sourcesPaths, const std::vector<std::filesystem::path>& qmlImportPaths);

std::filesystem::path getQmlModuleRelativePath(const std::vector<std::filesystem::path>& qmlModulesImportPaths,
                                                 const std::filesystem::path& qmlModulePath);

std::vector<QmlModuleImport> getQmlImports(const std::filesystem::path& projectRootPath, const std::filesystem::path& installQmlPath);
