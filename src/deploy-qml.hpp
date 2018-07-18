//
// Created by alexis on 7/17/18.
//

#ifndef LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H
#define LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <nlohmann/json.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/log.h>

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

const bf::path find_qmlimportscanner_binary_path() {
    return which("qmlimportscanner");
}

std::string run_qmlimportscanner(bf::path projectRootPath, const std::vector<bf::path> &qmlImportPaths) {
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
            moduleImport.path = qmlModuleImportJson.at("path").get<std::__cxx11::string>();
            moduleImport.relativePath = qmlModuleImportJson.at("relativePath").get<std::__cxx11::string>();

            imports.push_back(moduleImport);
        }
    }

    return imports;
}

std::vector<QmlModuleImport>
get_qml_imports(bf::path projectRootPath, const std::vector<bf::path> &extraQmlImportPaths) {
    std::vector<QmlModuleImport> imports;

    std::vector<bf::path> qmlImportPaths = extraQmlImportPaths;
    qmlImportPaths.push_back(get_default_qml_import_path());
    auto output = run_qmlimportscanner(projectRootPath, qmlImportPaths);

    try {
        imports = try_parse_qmlimportscanner_output(output);
    } catch (...) {
        ldLog() << LD_ERROR << "Unable to parse 'qmlimportscanner' output!" << std::endl;
    }

    return imports;
}

void deploy_qml(appdir::AppDir &appDir, const std::vector<bf::path> &qmlImportPaths) {


    for (const auto &qmlModuleDir: qmlImportPaths) {

    }
}

#endif //LINUXDEPLOY_PLUGIN_QT_DEPLOY_QML_H
