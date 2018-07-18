//
// Created by alexis on 7/17/18.
//

#ifndef LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H
#define LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>

#include <nlohmann/json.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/core/elf.h>

#include "util.hpp"

namespace bf = boost::filesystem;
using namespace linuxdeploy::core;
using namespace linuxdeploy::core::log;
using json = nlohmann::json;

typedef struct {
    std::string name;
    bf::path path;
    bf::path relativePath;
} QmlModuleImport;

static const char *const EXTRA_QML_IMPORT_PATHS_ENV_KEY = "EXTRA_QML_IMPORT_PATHS";

std::vector<boost::filesystem::path> get_extra_qml_import_paths();

std::string getRelativeFilePath(const std::string &filePath, const std::string &relativeModulePath);

const bf::path find_qmlimportscanner_binary_path() {
    return which("qmlimportscanner");
}

std::string run_qmlimportscanner(const bf::path &projectRootPath, const std::vector<bf::path> &qmlImportPaths) {
    auto qmlimportscanner_binary = find_qmlimportscanner_binary_path();
    std::vector<std::string> command = {qmlimportscanner_binary.string().c_str(),
                                        "-rootPath", projectRootPath.c_str()};

    for (const auto &qmlImportPath : qmlImportPaths) {
        command.push_back("-importPath");
        command.push_back(qmlImportPath.string());
    }

    ldLog() << LD_DEBUG << "Calling: ";
    for (auto string : command)
        ldLog() << LD_DEBUG << string << " ";
    ldLog() << LD_DEBUG << std::endl;

    auto output = check_command(command);

    if (output.retcode == 0)
        return output.stdoutOutput;
    else {
        ldLog() << LD_ERROR << output.stderrOutput << std::endl;
        return std::string{};
    }
}

bf::path get_default_qml_import_path() {
    const auto qmakePath = find_qmake_path();
    const auto qmakeVars = queryQmake(qmakePath);
    const bf::path qtQmlModulesPath = qmakeVars.at("QT_INSTALL_QML");

    return qtQmlModulesPath;
}

std::vector<QmlModuleImport> try_parse_qmlimportscanner_output(const std::string &output) {
    std::vector<QmlModuleImport> imports;
    auto jsonOutput = json::parse(output);
    for (int i = 0; i < jsonOutput.size(); i++) {
        auto qmlModuleImportJson = jsonOutput[i];
        auto type = qmlModuleImportJson.at("type").get<std::__cxx11::string>();
        if (type == "module") {
            QmlModuleImport moduleImport;
            moduleImport.name = qmlModuleImportJson.at("name").get<std::__cxx11::string>();
            if (qmlModuleImportJson.find("path") != qmlModuleImportJson.end())
                moduleImport.path = qmlModuleImportJson.at("path").get<std::__cxx11::string>();

            if (qmlModuleImportJson.find("relativePath") != qmlModuleImportJson.end())
                moduleImport.relativePath = qmlModuleImportJson.at("relativePath").get<std::__cxx11::string>();

            imports.push_back(moduleImport);
        }
    }

    return imports;
}

std::vector<boost::filesystem::path> get_extra_qml_import_paths() {
    std::vector<boost::filesystem::path> extraQmlImportPaths;
    const std::__cxx11::string extraQmlImportPathsRaw = getenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY);
    if (!extraQmlImportPathsRaw.empty()) {
        for (const auto &path: subprocess::util::split(extraQmlImportPathsRaw, ":")) {
            extraQmlImportPaths.emplace_back(boost::filesystem::path{path});
            ldLog() << "Using EXTRA QML IMPORT PATH: " << path << std::endl;
        }
    }
    return extraQmlImportPaths;
}

std::vector<QmlModuleImport>
get_qml_imports(const bf::path &projectRootPath) {
    std::vector<QmlModuleImport> imports;

    std::vector<boost::filesystem::path> qmlImportPaths = {get_default_qml_import_path()};
    const auto extraQmlImportPath = get_extra_qml_import_paths();
    qmlImportPaths.insert(qmlImportPaths.end(), extraQmlImportPath.begin(), extraQmlImportPath.end());
    auto output = run_qmlimportscanner(projectRootPath, qmlImportPaths);

    try {
        imports = try_parse_qmlimportscanner_output(output);
    } catch (...) {
        ldLog() << LD_ERROR << "Unable to parse 'qmlimportscanner' output!" << std::endl;
    }

    return imports;
}

void deploy_qml(appdir::AppDir &appDir) {
    auto qmlImports = get_qml_imports(appDir.path());
    bf::path targetQmlModulesPath = appDir.path().string() + "/usr/qml/";

    for (const auto &qmlImport: qmlImports) {
        if (qmlImport.path.empty()) {
            ldLog() << LD_ERROR << "Missing qml module: " << qmlImport.name << std::endl;
        } else {
            if (bf::is_directory(qmlImport.path)) {
                for (auto &entry : boost::make_iterator_range(bf::directory_iterator(qmlImport.path))) {
                    const auto relativeFilePath = getRelativeFilePath(entry.path().string(),
                                                                      qmlImport.relativePath.string());
                    try {
                        elf::ElfFile file(entry.path());
                        ldLog() << LD_INFO << "Queued for deploying as LIBRARY: " << entry.path()
                                << "\n\t at: "<< targetQmlModulesPath.string() + relativeFilePath << std::endl;
                        appDir.deployLibrary(entry.path(), targetQmlModulesPath.string() + relativeFilePath);
                    } catch (...) {
                        ldLog() << LD_INFO << "Queued for deploying as REGULAR FILE: " << entry.path()
                                << "\n\t at: "<< targetQmlModulesPath.string() + relativeFilePath << std::endl;
                        appDir.deployFile(entry.path(), targetQmlModulesPath.string() + relativeFilePath);
                    }
                    std::cout << entry.path() << std::endl;
                }
            }

        }
    }
}

std::string getRelativeFilePath(const std::string &filePath, const std::string &relativeModulePath) {
    std::__cxx11::string relativeFilePath;
    std::size_t found = filePath.find(relativeModulePath);
    if (found != std::string::npos)
        relativeFilePath = filePath.substr(found, filePath.size());
    return relativeFilePath;
}

#endif //LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H
