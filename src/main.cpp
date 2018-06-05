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
#include <subprocess.hpp>

// local includes
#include "qt-modules.h"

namespace bf = boost::filesystem;

using namespace linuxdeploy::core;
using namespace linuxdeploy::core::log;

static ldLog ldQtLog(LD_LOGLEVEL logLevel = LD_INFO) {
    return ldLog() << logLevel << "linuxdeploy-plugin-qt:";
}

static std::string which(const std::string& name) {
    subprocess::Popen proc({"which", name.c_str()}, subprocess::output{subprocess::PIPE});
    auto output = proc.communicate();

    if (proc.retcode() != 0) {
        ldQtLog(LD_DEBUG) << "which call failed:" << output.first.buf << std::endl;
        return "";
    }

    std::string path =  output.first.buf.data();
    while (path.back() == '\n') {
        path.erase(path.end() - 1, path.end());
    }

    return path;
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

std::string join(const std::set<std::string>& list) {
    std::vector<std::string> data;
    std::copy(list.begin(), list.end(), std::back_inserter(data));
    return join(data);
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
        std::set<std::string> moduleNames;
        std::for_each(foundQtModules.begin(), foundQtModules.end(), [&moduleNames](const QtModule& module) {
            moduleNames.insert(module.name);
        });
        ldQtLog() << "Found Qt modules:" << join(moduleNames) << std::endl;
    }

    // search for qmake
    auto qmakePath = which("qmake-qt5");

    if (qmakePath.empty()) {
        qmakePath = which("qmake");

        if (qmakePath.empty()) {
            ldQtLog() << "Failed to find suitable qmake" << std::endl;
            return 1;
        }
    }

    ldQtLog() << "Using qmake:" << qmakePath << std::endl;

    for (const auto& module : foundQtModules) {
        ldQtLog() << "-- Deploying module:" << module.name << "--" << std::endl;

        ldQtLog() << "Nothing to do for module:" << module.name << std::endl;
    }

    ldQtLog() << "-- Executing deferred operations --" << std::endl;
    if (!appDir.executeDeferredOperations()) {
        ldQtLog(LD_ERROR) << "Failed to execute deferred operations" << std::endl;
        return 1;
    }

    ldQtLog() << "Done!" << std::endl;
}
