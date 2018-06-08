// system includes
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
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

typedef struct {
    bool success;
    int retcode;
    std::string stdoutOutput;
    std::string stderrOutput;
} procOutput;

procOutput check_command(const std::initializer_list<const char*> args) {
    subprocess::Popen proc(args, subprocess::output(subprocess::PIPE), subprocess::error(subprocess::PIPE));

    auto output = proc.communicate();

    std::string out, err;

    if (output.first.length > 0)
        out = output.first.buf.data();

    if (output.second.length > 0)
        err = output.second.buf.data();

    return {proc.retcode() == 0, proc.retcode(), out, err};
};

const std::map<std::string, std::string> queryQmake(const bf::path& qmakePath) {
    auto qmakeCall = check_command({qmakePath.c_str(), "-query"});

    if (!qmakeCall.success)
        return {};

    std::map<std::string, std::string> rv;

    std::stringstream ss;
    ss << qmakeCall.stdoutOutput;

    std::string line;

    auto stringSplit = [](const std::string& str, const char delim = ' ') {
        std::stringstream ss;
        ss << str;

        std::string part;
        std::vector<std::string> parts;

        while (std::getline(ss, part, delim)) {
            parts.push_back(part);
        }

        return parts;
    };

    while (std::getline(ss, line)) {
        auto parts = stringSplit(line, ':');

        if (parts.size() != 2)
            continue;

        rv[parts[0]] = parts[1];
    }

    return rv;
};

static std::string which(const std::string& name) {
    subprocess::Popen proc({"which", name.c_str()}, subprocess::output(subprocess::PIPE));
    auto output = proc.communicate();

    if (proc.retcode() != 0) {
        ldLog() << LD_DEBUG << "which call failed:" << output.first.buf.data() << std::endl;
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

bool deployPlatformPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying platform plugins" << std::endl;

    if (!appDir.deployLibrary(qtPluginsPath / "platforms/libqxcb.so", appDir.path() / "usr/plugins/platforms/"))
        return false;

    for (bf::directory_iterator i(qtPluginsPath / "platforminputcontexts"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/platforminputcontexts/"))
            return false;
    }

    for (bf::directory_iterator i(qtPluginsPath / "imageformats"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/imageformats/"))
            return false;
    }

    // TODO: platform themes -- https://github.com/probonopd/linuxdeployqt/issues/236

    return true;
}

bool deployXcbglIntegrationPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "xcbglintegrations"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/xcbglintegrations/"))
            return false;
    }

    return true;
}

bool deploySvgPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying svg icon engine" << std::endl;

    if (!appDir.deployLibrary(qtPluginsPath / "iconengines/libqsvgicon.so", appDir.path() / "usr/plugins/iconengines/"))
        return false;

    return true;
}

bool deployBearerPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying bearer plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "bearer"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/bearer/"))
            return false;
    }

    return true;
}

bool deploySqlPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying SQL plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "bearer"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/sqldrivers/"))
            return false;
    }

    return true;
}

bool deployPositioningPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying positioning plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "position"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/position/"))
            return false;
    }

    return true;
}

bool deployMultimediaPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying mediaservice plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "mediaservice"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/mediaservice/"))
            return false;
    }
    ldLog() << "Deploying audio plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "audio"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/audio/"))
            return false;
    }

    return true;
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
        ldLog() << LD_ERROR << "--appdir parameter required" << std::endl;
        std::cout << std::endl << parser;
        return 1;
    }

    if (!bf::is_directory(appDirPath.Get())) {
        ldLog() << LD_ERROR << "No such directory:" << appDirPath.Get() << std::endl;
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
            ldLog() << LD_DEBUG << "Failed to parse file as ELF file:" << path << std::endl;
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
        ldLog() << "Found Qt modules:" << join(moduleNames) << std::endl;
    }

    // search for qmake
    auto qmakePath = which("qmake-qt5");

    if (qmakePath.empty()) {
        qmakePath = which("qmake");

        if (qmakePath.empty()) {
            ldLog() << "Failed to find suitable qmake" << std::endl;
            return 1;
        }
    }

    ldLog() << "Using qmake:" << qmakePath << std::endl;

    auto qmakeVars = queryQmake(qmakePath);

    if (qmakeVars.empty())
        return 1;

    auto qtPluginsPath = qmakeVars["QT_INSTALL_PLUGINS"];

    for (const auto& module : foundQtModules) {
        ldLog() << std::endl << "-- Deploying module:" << module.name << "--" << std::endl;

        if (module.name == "gui") {
            if (!deployPlatformPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "opengl" || module.name == "gui" || module.name == "xcbqpa") {
            if (!deployXcbglIntegrationPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "network") {
            if (!deployBearerPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "svg") {
            if (!deploySvgPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "sql") {
            if (!deploySqlPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "positioning") {
            if (!deployPositioningPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "multimedia") {
            if (!deployMultimediaPlugins(appDir, qtPluginsPath))
                return 1;
        }
    }

    ldLog() << std::endl << "-- Executing deferred operations --" << std::endl;
    if (!appDir.executeDeferredOperations()) {
        ldLog() << LD_ERROR << "Failed to execute deferred operations" << std::endl;
        return 1;
    }

    ldLog() << std::endl << "Done!" << std::endl;
}
