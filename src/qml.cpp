// system includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

// library includes
#include <json.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/core/elf.h>

// local includes
#include "util.h"
#include "qml.h"

namespace bf = boost::filesystem;
using namespace linuxdeploy::core;
using namespace linuxdeploy::core::log;
using namespace nlohmann;

bf::path findQmlImportScanner() {
    return which("qmlimportscanner");
}

std::string runQmlImportScanner(const bf::path& projectRootPath, const std::vector<bf::path>& qmlImportPaths) {
    auto qmlImportScannerPath = findQmlImportScanner();
    std::vector<std::string> command = {qmlImportScannerPath.string().c_str(),
                                        "-rootPath", projectRootPath.c_str()};

    for (const auto &qmlImportPath : qmlImportPaths) {
        command.push_back("-importPath");
        command.push_back(qmlImportPath.string());
    }

    ldLog() << LD_DEBUG << "Calling: ";
    for (const auto& string : command)
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

std::vector<QmlModuleImport> parseQmlImportScannerOutput(const std::string& output) {
    std::vector<QmlModuleImport> imports;

    auto jsonOutput = json::parse(output);

    for (const auto& qmlModuleImportJson : jsonOutput) {
        auto type = qmlModuleImportJson.at("type").get<std::string>();
        if (type == "module") {
            QmlModuleImport moduleImport;
            moduleImport.name = qmlModuleImportJson.at("name").get<std::string>();
            if (qmlModuleImportJson.find("path") != qmlModuleImportJson.end())
                moduleImport.path = qmlModuleImportJson.at("path").get<std::string>();

            if (qmlModuleImportJson.find("relativePath") != qmlModuleImportJson.end())
                moduleImport.relativePath = qmlModuleImportJson.at("relativePath").get<std::string>();

            imports.push_back(moduleImport);
        }
    }

    return imports;
}

std::vector<boost::filesystem::path> getExtraQmlImportPaths() {
    std::vector<boost::filesystem::path> extraQmlImportPaths;
    const std::string extraQmlImportPathsRaw = getenv(EXTRA_QML_IMPORT_PATHS_ENV_KEY);

    if (!extraQmlImportPathsRaw.empty()) {
        for (const auto &path: subprocess::util::split(extraQmlImportPathsRaw, ":")) {
            extraQmlImportPaths.emplace_back(boost::filesystem::path{path});
            ldLog() << "Using EXTRA QML IMPORT PATH: " << path << std::endl;
        }
    }

    return extraQmlImportPaths;
}

std::vector<QmlModuleImport> getQmlImports(const bf::path& projectRootPath, const bf::path& installQmlPath) {
    std::vector<QmlModuleImport> imports;

    std::vector<boost::filesystem::path> qmlImportPaths = {installQmlPath};
    const auto extraQmlImportPaths = getExtraQmlImportPaths();

    qmlImportPaths.reserve(extraQmlImportPaths.size());
    qmlImportPaths.insert(qmlImportPaths.end(), extraQmlImportPaths.begin(), extraQmlImportPaths.end());
    auto output = runQmlImportScanner(projectRootPath, qmlImportPaths);

    try {
        imports = parseQmlImportScannerOutput(output);
    } catch (...) {
        ldLog() << LD_ERROR << "Unable to parse 'qmlimportscanner' output!" << std::endl;
    }

    return imports;
}

void deployQml(appdir::AppDir& appDir, const bf::path& installQmlPath) {
    auto qmlImports = getQmlImports(appDir.path(), installQmlPath);
    bf::path targetQmlModulesPath = appDir.path().string() + "/usr/qml/";

    for (const auto &qmlImport: qmlImports) {
        if (!qmlImport.path.empty()) {
            if (bf::is_directory(qmlImport.path)) {
                for (bf::directory_iterator i(qmlImport.path); i != bf::directory_iterator(); ++i) {
                    const auto& entry = *i;

                    auto relativeFilePath = qmlImport.relativePath / bf::relative(entry.path(), qmlImport.path);

                    try {
                        elf::ElfFile file(entry.path());
                        appDir.deployLibrary(entry.path(), targetQmlModulesPath / relativeFilePath);
                    } catch (const elf::ElfFileParseError&) {
                        appDir.deployFile(entry.path(), targetQmlModulesPath / relativeFilePath);
                    }
                }
            }
        } else
            ldLog() << LD_ERROR << "Missing qml module: " << qmlImport.name << std::endl;
    }
}
