// system includes
#include <boost/filesystem.hpp>

// library includes
#include <json.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/core/elf.h>
#include <linuxdeploy/util/util.h>

// local includes
#include "util.h"
#include "qml.h"

namespace bf = boost::filesystem;
using namespace linuxdeploy::core;
using namespace linuxdeploy::core::log;
using namespace linuxdeploy::util;
using namespace nlohmann;

bf::path findQmlImportScanner() {
    return which("qmlimportscanner");
}

std::string
runQmlImportScanner(const std::vector<boost::filesystem::path> &sourcesPaths,
                    const std::vector<bf::path> &qmlImportPaths) {
    auto qmlImportScannerPath = findQmlImportScanner();
    std::vector<std::string> command{qmlImportScannerPath.string()};

    for (const auto &sourcesPath : sourcesPaths)
        if (!sourcesPath.empty()) {
            command.emplace_back("-rootPath");
            command.emplace_back(sourcesPath.string());
        }


    for (const auto &qmlImportPath : qmlImportPaths)
        if (!qmlImportPath.empty()){
        command.emplace_back("-importPath");
        command.emplace_back(qmlImportPath.string());
    }

    ldLog() << LD_INFO << "Calling: ";
    for (const auto &string : command)
        ldLog() << LD_INFO << string << " ";
    ldLog() << LD_INFO << std::endl;

    auto output = check_command(command);

    if (output.retcode == 0)
        return output.stdoutOutput;
    else {
        ldLog() << LD_ERROR << output.stderrOutput << std::endl;
        return std::string{};
    }
}

std::vector<QmlModuleImport> parseQmlImportScannerOutput(const std::string &output) {
    std::vector<QmlModuleImport> imports;

    auto jsonOutput = json::parse(output);

    if (jsonOutput.empty())
        throw NoImportsFound("No qml imports found");

    for (const auto &qmlModuleImportJson : jsonOutput) {
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

std::vector<bf::path> getExtraQmlModulesPaths() {
    const auto* envVarContents = std::getenv(ENV_KEY_QML_MODULES_PATHS);

    if (envVarContents == nullptr)
        return {};

    auto paths = split(envVarContents);
    std::vector<bf::path> extraQmlSourcesPaths;

    std::copy(paths.begin(), paths.end(), std::back_inserter(extraQmlSourcesPaths));

    return extraQmlSourcesPaths;
}

std::vector<bf::path> getExtraQmlSourcesPaths() {
    const auto* envVarContents = std::getenv(ENV_KEY_QML_SOURCES_PATHS);

    if (envVarContents == nullptr)
        return {};

    auto paths = split(envVarContents);
    std::vector<bf::path> extraQmlSourcesPaths;

    std::copy(paths.begin(), paths.end(), std::back_inserter(extraQmlSourcesPaths));

    return extraQmlSourcesPaths;
}

std::vector<QmlModuleImport> getQmlImports(const bf::path &projectRootPath, const bf::path &installQmlPath) {
    std::vector<QmlModuleImport> moduleImports;

    auto qmlImportPaths = getExtraQmlModulesPaths();
    auto qmlSourcesPaths = getExtraQmlSourcesPaths();
    qmlImportPaths.emplace_back(installQmlPath);
    qmlSourcesPaths.emplace_back(projectRootPath);

    auto output = runQmlImportScanner(qmlSourcesPaths, qmlImportPaths);

    try {
        moduleImports = parseQmlImportScannerOutput(output);
    } catch (const json::parse_error &e) {
        ldLog() << LD_ERROR << e.what() << std::endl;
        throw QmlImportScannerError("Unable to parse 'qmlimportscanner' output!");
    } catch (const NoImportsFound &) {
        if (moduleImports.empty()) {
            ldLog() << LD_ERROR << "No qml module imports fount at: " << std::endl;
            for (const auto &sourcesPath: qmlSourcesPaths)
                ldLog() << LD_ERROR << "\t" << sourcesPath.string() << std::endl;


            ldLog() << std::endl << LD_INFO << "Use the environment variable" << ENV_KEY_QML_SOURCES_PATHS <<
                    "to specify the paths to the directories containing your project QML files." << std::endl;
            ldLog() << LD_INFO << "\t" << "Example: " << ENV_KEY_QML_SOURCES_PATHS << LD_NO_SPACE <<
                    "=\"/project_root/src/qml\"" << std::endl;
        }
        throw;
    }

    // Resolve modules relative path if qmlimportscanner din't provided it
    for (auto &moduleImport: moduleImports) {
        if (!moduleImport.path.empty() && moduleImport.relativePath.empty())
            moduleImport.relativePath = getQmlModuleRelativePath(qmlImportPaths, moduleImport.path);
    }

    return moduleImports;
}

boost::filesystem::path getQmlModuleRelativePath(const std::vector<boost::filesystem::path> &qmlModulesImportPaths,
                                                 const boost::filesystem::path &qmlModulePath) {
    boost::filesystem::path relativePath;
    for (const auto &qmlImportPath: qmlModulesImportPaths) {
        auto candidate = relative(qmlModulePath, qmlImportPath);
        if (qmlImportPath / candidate == qmlModulePath && (candidate < relativePath || relativePath.empty()))
            relativePath = candidate;
    }

    return relativePath;
}

void deployQml(appdir::AppDir &appDir, const boost::filesystem::path &installQmlPath) {
    auto qmlImports = getQmlImports(appDir.path(), installQmlPath);
    bf::path targetQmlModulesPath = appDir.path().string() + "/usr/qml/";

    for (const auto &qmlImport: qmlImports) {
        if (!qmlImport.path.empty()) {
            if (bf::is_directory(qmlImport.path)) {
                for (bf::directory_iterator i(qmlImport.path); i != bf::directory_iterator(); ++i) {
                    const auto &entry = *i;

                    auto relativeFilePath = qmlImport.relativePath / bf::relative(entry.path(), qmlImport.path);

                    try {
                        elf::ElfFile file(entry.path());
                        appDir.deployLibrary(entry.path(), targetQmlModulesPath / relativeFilePath);
                    } catch (const elf::ElfFileParseError &) {
                        appDir.deployFile(entry.path(), targetQmlModulesPath / relativeFilePath);
                    }
                }
            }
        } else
            ldLog() << LD_ERROR << "Missing qml module: " << qmlImport.name << std::endl;
    }
}
