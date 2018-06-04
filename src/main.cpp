// system includes
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// library includes
#include <boost/filesystem.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/elf.h>
#include <linuxdeploy/core/log.h>
#include <args.hxx>

// local includes
#include "qt-modules.h"

namespace bf = boost::filesystem;

using namespace linuxdeploy::core;
using namespace linuxdeploy::core::log;

static ldLog ldQtLog() {
    return ldLog() << "linuxdeploy-plugin-qt:";
}

std::string join(const std::vector<std::string>& list) {
    std::stringstream rv;

    if (!list.empty()) {
        rv << list[0];

        std::for_each(list.begin() + 1, list.end(), [&rv](const std::string& s) {
            rv << " " << s;
        });
    }

    return rv.str();
}

int main(const int argc, const char* const* argv) {
    args::ArgumentParser parser("linuxdeploy Qt plugin", "Bundles Qt resources. For use with an existing AppDir, created by linuxdeploy.");

    args::ValueFlag<bf::path> appDirPath(parser, "appdir path", "Path to an existing AppDir", {"appdir"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::ParseError&) {
        std::cerr << parser;
        return 1;
    }

    if (!appDirPath) {
        ldQtLog() << LD_ERROR << "--appdir parameter required" << std::endl;
        std::cout << std::endl << parser;
        return 1;
    }

    if (!bf::is_directory(appDirPath.Get())) {
        ldQtLog() << LD_ERROR << "No such directory:" << appDirPath.Get() << std::endl;
        return 1;
    }

    appdir::AppDir appDir(appDirPath.Get());

    // check which libraries and plugins the binaries and libraries depend on
    std::set<std::string> libraryNames;
    for (const auto& path : appDir.listExecutables()) {
        try {
            for (const auto& dependency : elf::ElfFile(path).traceDynamicDependencies()) {
                libraryNames.insert(dependency.filename().string());
            }
        } catch (const elf::ElfFileParseError& e) {
            ldQtLog() << LD_DEBUG << "Failed to parse file as ELF file:" << path << std::endl;
        }
    }

    // check for Qt modules
    std::vector<QtModule> foundQtModules;

    std::copy_if(QtModules.begin(), QtModules.end(), std::back_inserter(foundQtModules), [&libraryNames](const QtModule& module) {
        return std::find_if(libraryNames.begin(), libraryNames.end(), [&module](const std::string& libraryFileName) {
            const auto& libraryPrefix = module.libraryFilePrefix;
            return strncmp(libraryFileName.c_str(), libraryPrefix.c_str(), libraryPrefix.size()) == 0;
        }) != libraryNames.end();
    });

    {
        std::vector<std::string> moduleNames;
        std::for_each(foundQtModules.begin(), foundQtModules.end(), [&moduleNames](const QtModule& module) {
            moduleNames.push_back(module.name);
            moduleNames.push_back(module.name);
            moduleNames.push_back(module.name);
        });
        ldQtLog() << "Found Qt modules:" << join(moduleNames) << std::endl;
    }

    for (const auto& module : foundQtModules) {
        ldQtLog() << "-- Deploying module:" << module.name << "--" << std::endl;

        // TODO: add if statements for all plugins that require special treatment

        ldQtLog() << "Nothing to do for module:" << module.name << std::endl;
    }

    ldQtLog() << "Done!" << std::endl;
}
