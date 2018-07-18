// library includes
#include <boost/filesystem.hpp>
#include <linuxdeploy/core/appdir.h>

#pragma once

typedef struct {
    std::string name;
    boost::filesystem::path path;
    boost::filesystem::path relativePath;
} QmlModuleImport;

static const char* const EXTRA_QML_IMPORT_PATHS_ENV_KEY = "EXTRA_QML_IMPORT_PATHS";

// deploys QML files into AppDir
void deployQml(linuxdeploy::core::appdir::AppDir& appDir, const boost::filesystem::path& installQmlPath);

boost::filesystem::path findQmlImportScanner();

std::string runQmlImportScanner(const boost::filesystem::path& projectRootPath, const std::vector<boost::filesystem::path>& qmlImportPaths);

std::vector<QmlModuleImport> getQmlImports(const boost::filesystem::path& projectRootPath, const boost::filesystem::path& installQmlPath);
