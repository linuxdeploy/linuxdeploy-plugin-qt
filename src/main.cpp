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

    if (!qmakeCall.success) {
        ldLog() << LD_ERROR << "Call to qmake failed:" << qmakeCall.stderrOutput << std::endl;
        return {};
    }

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

template<typename Iter> std::string join(Iter beg, Iter end) {
    std::stringstream rv;

    if (beg != end) {
        rv << *beg;

        std::for_each(++beg, end, [&rv](const std::string& s) {
            rv << " " << s;
        });
    }

    return rv.str();
}

std::string join(const std::vector<std::string>& list) {
    return join(list.begin(), list.end());
}

std::string join(const std::set<std::string>& list) {
    return join(list.begin(), list.end());
}

bool strStartsWith(const std::string& str, const std::string& prefix) {
    if (str.size() < prefix.size())
        return false;

    return strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}

bool strEndsWith(const std::string& str, const std::string& suffix) {
    if (str.size() < suffix.size())
        return false;

    return strncmp(str.c_str() + (str.size() - suffix.size()), suffix.c_str(), suffix.size()) == 0;
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

    for (bf::directory_iterator i(qtPluginsPath / "sqldrivers"); i != bf::directory_iterator(); ++i) {
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

bool deployWebEnginePlugins(appdir::AppDir& appDir, const bf::path& qtLibexecsPath, const bf::path& qtDataPath, const bf::path& qtTranslationsPath) {
    ldLog() << "Deploying web engine plugins" << std::endl;

    for (bf::directory_iterator i(qtLibexecsPath); i != bf::directory_iterator(); ++i) {
        auto& entry = *i;
        const std::string prefix = "QtWeb";

        auto fileName = entry.path().filename();

        // skip files which don't start with prefix
        if (!strStartsWith(fileName.string(), prefix))
            continue;

        if (!appDir.deployExecutable(*i, appDir.path() / "usr/libexec/"))
            return false;
    }

    for (const auto& fileName : {"qtwebengine_resources.pak",
                                 "qtwebengine_devtools_resources.pak",
                                 "qtwebengine_resources_100p.pak",
                                 "qtwebengine_resources_200p.pak", "icudtl.dat"}) {
        auto path = qtDataPath / "resources" / fileName;

        if (bf::is_regular_file(path))
            appDir.deployFile(path, appDir.path() / "usr/data/resources/");
    }

    if (bf::is_directory(qtTranslationsPath / "qtwebengine_locales")) {
        for (bf::directory_iterator i(qtTranslationsPath / "qtwebengine_locales"); i != bf::directory_iterator(); ++i) {
            appDir.deployFile(*i, appDir.path() / "usr/qtwebengine_locales/");
        }
    }

    return true;
}

bool createQtConf(appdir::AppDir& appDir) {
    auto qtConfPath = appDir.path() / "usr" / "bin" / "qt.conf";

    if (bf::is_regular_file(qtConfPath)) {
        ldLog() << LD_WARNING << "Skipping creation of Qt conf file: file exists:" << qtConfPath << std::endl;
    } else {
        ldLog() << "Creating Qt conf file:" << qtConfPath << std::endl;
    }

    std::ofstream ofs(qtConfPath.string());

    if (!ofs) {
        ldLog() << LD_ERROR << "Failed to open qt.conf for writing" << std::endl;
        return false;
    }

    ofs << "# generated by linuxdeploy" << std::endl
        << "[Paths]" << std::endl
        << "Prefix = .." << std::endl
        << "Plugins = plugins" << std::endl
        << "Imports = qml" << std::endl
        << "Qml2Imports = qml" << std::endl;

    return true;
}

bool deployTranslations(appdir::AppDir& appDir, const bf::path& qtTranslationsPath, const std::vector<QtModule>& modules) {
    if (qtTranslationsPath.empty() || !bf::is_directory(qtTranslationsPath)) {
        ldLog() << LD_WARNING << "Translation directory does not exist, skipping deployment";
        return true;
    }

    ldLog() << "Qt translations directory:" << qtTranslationsPath << std::endl;

    auto checkName = [&appDir, &modules](const bf::path& fileName) {
        if (!strEndsWith(fileName.string(), ".qm"))
            return false;

        // always deploy basic Qt translations
        if (strStartsWith(fileName.string(), "qt_") && bf::basename(fileName).size() >= 5 && bf::basename(fileName).size() <= 6)
            return true;

        for (const auto& module : modules) {
            if (!module.translationFilePrefix.empty() && strStartsWith(fileName.string(), module.translationFilePrefix))
                return true;
        }

        return false;
    };

    for (bf::directory_iterator i(qtTranslationsPath); i != bf::directory_iterator(); ++i) {
        if (!bf::is_regular_file(*i))
            continue;

        const auto fileName = (*i).path().filename();

        if (checkName(fileName))
            appDir.deployFile(*i, appDir.path() / "usr/translations/");
    }

    return true;
}

int main(const int argc, const char* const* const argv) {
    // set up verbose logging if $DEBUG is set
    if (getenv("DEBUG"))
        ldLog::setVerbosity(LD_DEBUG);

    args::ArgumentParser parser("linuxdeploy Qt plugin", "Bundles Qt resources. For use with an existing AppDir, created by linuxdeploy.");

    args::ValueFlag<bf::path> appDirPath(parser, "appdir path", "Path to an existing AppDir", {"appdir"});
    args::ValueFlagList<std::string> extraPlugins(parser, "plugin", "Extra Qt plugin to deploy (specified by name, filename or path)", {'p', "extra-plugin"});

    args::Flag pluginType(parser, "", "Print plugin type and exit", {"plugin-type"});
    args::Flag pluginApiVersion(parser, "", "Print plugin API version and exit", {"plugin-api-version"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::ParseError&) {
        std::cerr << parser;
        return 1;
    }

    if (pluginType) {
        std::cout << "input" << std::endl;
        return 0;
    }

    if (pluginApiVersion) {
        std::cout << "0" << std::endl;
        return 0;
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
    std::vector<QtModule> extraQtModules;

    auto matchesQtModule = [](std::string libraryName, const QtModule& module) {
        // extract filename if argument is path
        if (bf::is_regular_file(libraryName))
            libraryName = bf::path(libraryName).filename().string();

        // adding the trailing dot makes sure e.g., libQt5WebEngineCore won't be matched as webengine and webenginecore
        const auto& libraryPrefix = module.libraryFilePrefix + ".";

        ldLog() << LD_DEBUG << "Checking library name '" << LD_NO_SPACE << libraryName
                << LD_NO_SPACE << "' against library prefix '" << LD_NO_SPACE << libraryPrefix << LD_NO_SPACE
            << "' and module name '" << LD_NO_SPACE << module.name << LD_NO_SPACE << "'" << std::endl;

        // match plugin filename
        if (strncmp(libraryName.c_str(), libraryPrefix.c_str(), libraryPrefix.size()) == 0) {
            ldLog() << LD_DEBUG << "-> matches library filename, found module:" << module.name << std::endl;
            return true;
        }

        // match plugin name
        if (strcmp(libraryName.c_str(), module.name.c_str()) == 0) {
            ldLog() << LD_DEBUG << "-> matches module name, found module:" << module.name << std::endl;
            return true;
        }

        return false;
    };

    std::copy_if(QtModules.begin(), QtModules.end(), std::back_inserter(foundQtModules), [&matchesQtModule, &libraryNames, &extraPlugins](const QtModule& module) {
        return std::find_if(libraryNames.begin(), libraryNames.end(), [&matchesQtModule, &module](const std::string& libraryName) {
            return matchesQtModule(libraryName, module);
        }) != libraryNames.end();
    });

    std::copy_if(QtModules.begin(), QtModules.end(), std::back_inserter(extraQtModules), [&matchesQtModule, libraryNames, &extraPlugins](const QtModule& module) {
        return std::find_if(extraPlugins.Get().begin(), extraPlugins.Get().end(), [&matchesQtModule, &module](const std::string& libraryName) {
            return matchesQtModule(libraryName, module);
        }) != extraPlugins.Get().end();
    });

    {
        std::set<std::string> moduleNames;
        std::for_each(foundQtModules.begin(), foundQtModules.end(), [&moduleNames](const QtModule& module) {
            moduleNames.insert(module.name);
        });
        ldLog() << "Found Qt modules:" << join(moduleNames) << std::endl;
    }

    {
        std::set<std::string> moduleNames;
        std::for_each(extraQtModules.begin(), extraQtModules.end(), [&moduleNames](const QtModule& module) {
            moduleNames.insert(module.name);
        });
        ldLog() << "Extra Qt modules:" << join(moduleNames) << std::endl;
    }

    if (foundQtModules.empty() && extraQtModules.empty()) {
        ldLog() << LD_ERROR << "Could not find Qt modules to deploy" << std::endl;
        return 1;
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

    if (qmakeVars.empty()) {
        ldLog() << LD_ERROR << "Failed to query Qt paths using qmake -query" << std::endl;
        return 1;
    }

    const bf::path qtPluginsPath = qmakeVars["QT_INSTALL_PLUGINS"];
    const bf::path qtLibexecsPath = qmakeVars["QT_INSTALL_LIBEXECS"];
    const bf::path qtDataPath = qmakeVars["QT_INSTALL_DATA"];
    const bf::path qtTranslationsPath = qmakeVars["QT_INSTALL_TRANSLATIONS"];

    auto qtModulesToDeploy = foundQtModules;
    qtModulesToDeploy.reserve(extraQtModules.size());
    std::copy(extraQtModules.begin(), extraQtModules.end(), std::back_inserter(qtModulesToDeploy));

    for (const auto& module : qtModulesToDeploy) {
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

        if (module.name == "webenginecore") {
            if (!deployWebEnginePlugins(appDir, qtLibexecsPath, qtDataPath, qtTranslationsPath))
                return 1;
        }
    }

    ldLog() << std::endl << "-- Deploying translations --" << std::endl;
    if (!deployTranslations(appDir, qtTranslationsPath, qtModulesToDeploy)) {
        ldLog() << LD_ERROR << "Failed to deploy translations" << std::endl;
        return 1;
    }

    ldLog() << std::endl << "-- Executing deferred operations --" << std::endl;
    if (!appDir.executeDeferredOperations()) {
        ldLog() << LD_ERROR << "Failed to execute deferred operations" << std::endl;
        return 1;
    }

    ldLog() << std::endl << "-- Creating qt.conf in AppDir --" << std::endl;
    if (!createQtConf(appDir))
        return 1;

    ldLog() << std::endl << "Done!" << std::endl;
}
