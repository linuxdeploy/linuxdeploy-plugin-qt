// system includes
#include <iostream>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

// library includes
#include <boost/filesystem.hpp>
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/elf.h>
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/util/util.h>

// local includes
#include "qt-modules.h"
#include "qml.h"
#include "util.h"

namespace bf = boost::filesystem;

using namespace linuxdeploy::core;
using namespace linuxdeploy::util::misc;
using namespace linuxdeploy::core::log;


std::string join(const std::vector<std::string> &list) {
    return join(list.begin(), list.end());
}

std::string join(const std::set<std::string> &list) {
    return join(list.begin(), list.end());
}

bool strStartsWith(const std::string &str, const std::string &prefix) {
    if (str.size() < prefix.size())
        return false;

    return strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}

bool strEndsWith(const std::string &str, const std::string &suffix) {
    if (str.size() < suffix.size())
        return false;

    return strncmp(str.c_str() + (str.size() - suffix.size()), suffix.c_str(), suffix.size()) == 0;
}

bool deployPlatformPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
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

    const bf::path platformThemesPath = qtPluginsPath / "platformthemes";
    const bf::path stylesPath = qtPluginsPath / "styles";

    const bf::path platformThemesDestination = appDir.path() / "usr/plugins/platformthemes/";
    const bf::path stylesDestination = appDir.path() / "usr/plugins/styles/";

    if (getenv("DEPLOY_PLATFORM_THEMES") != nullptr) {
        ldLog() << LD_WARNING << "Deploying all platform themes and styles [experimental feature]" << std::endl;

        if (bf::is_directory(platformThemesPath))
            for (bf::directory_iterator i(platformThemesPath); i != bf::directory_iterator(); ++i)
                if (!appDir.deployLibrary(*i, platformThemesDestination))
                    return false;

        if (bf::is_directory(stylesPath))
            for (bf::directory_iterator i(stylesPath); i != bf::directory_iterator(); ++i)
                if (!appDir.deployLibrary(*i, stylesDestination))
                    return false;
    } else {
        ldLog() << "Trying to deploy Gtk 2 platform theme and/or style" << std::endl;

        // according to probono, only the files shall be deployed, not their dependencies
        // either loading succeeds, then the system Gtk shall be used anyway, otherwise loading fails and Qt will fall
        // back to the default UI theme
        // we don't care whether this works (it's an experimental feature), therefore we ignore the return values
        const auto libqgtk2Filename = "libqgtk2.so";
        const auto libqgtk2styleFilename = "libqgtk2style.so";

        const auto libqgtk2Path = platformThemesPath / libqgtk2Filename;
        const auto libqgtk2stylePath = stylesPath / libqgtk2styleFilename;

        // we need to check whether the files exist at least, otherwise the deferred deployment operation fails
        if (bf::is_regular_file(libqgtk2Path)) {
            ldLog() << "Attempting to deploy" << libqgtk2Filename << "found at path" << libqgtk2Path << std::endl;
            appDir.deployFile(libqgtk2Path, platformThemesDestination);
        } else {
            ldLog() << "Could not find" << libqgtk2Filename << "on system, skipping deployment" << std::endl;
        }

        if (bf::is_regular_file(libqgtk2stylePath)) {
            ldLog() << "Attempting to deploy" << libqgtk2styleFilename << "found at path" << libqgtk2stylePath << std::endl;
            appDir.deployFile(libqgtk2stylePath, stylesDestination);
        } else {
            ldLog() << "Could not find" << libqgtk2styleFilename << "on system, skipping deployment" << std::endl;
        }
    }

    return true;
}

// little helper called by other integration plugins
bool deployIntegrationPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath, const std::initializer_list<bf::path>& subDirs) {
    for (const bf::path& subDir : subDirs) {
        // make sure the path ends with a / so that liblinuxdeploy recognize the destination as a directory
        auto dir = qtPluginsPath / subDir / "/";

        if (!bf::is_directory(dir)) {
            ldLog() << "Directory" << dir << "doesn't exist, skipping deployment" << std::endl;
            continue;
        }

        for (bf::directory_iterator i(dir); i != bf::directory_iterator(); ++i) {
            // append a trailing slash to make linuxdeploy aware of the destination being a directory
            // otherwise, when the directory doesn't exist, it might just copy all files to files called like
            // destinationDir
            auto destinationDir = appDir.path() / "usr/plugins" / subDir / "";

            if (!appDir.deployLibrary(*i, destinationDir))
                return false;
        }
    }

    return true;
}

bool deployXcbglIntegrationPlugins(appdir::AppDir& appDir, const bf::path& qtPluginsPath) {
    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    return deployIntegrationPlugins(appDir, qtPluginsPath, {"xcbglintegrations/"});
}

bool deploySvgPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying svg icon engine" << std::endl;

    if (!appDir.deployLibrary(qtPluginsPath / "iconengines/libqsvgicon.so", appDir.path() / "usr/plugins/iconengines/"))
        return false;

    return true;
}

bool deployBearerPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying bearer plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "bearer"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/bearer/"))
            return false;
    }

    return true;
}

bool deploySqlPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying SQL plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "sqldrivers"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/sqldrivers/"))
            return false;
    }

    return true;
}

bool deployPositioningPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying positioning plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "position"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/position/"))
            return false;
    }

    return true;
}

bool deployMultimediaPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
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

bool deployWebEnginePlugins(appdir::AppDir &appDir, const bf::path &qtLibexecsPath, const bf::path &qtDataPath,
                            const bf::path &qtTranslationsPath) {
    ldLog() << "Deploying web engine plugins" << std::endl;

    const auto newLibexecPath = appDir.path() / "usr/libexec/";

    // make sure directory is there before trying to write a qt.conf file
    bf::create_directories(newLibexecPath);

    for (bf::directory_iterator i(qtLibexecsPath); i != bf::directory_iterator(); ++i) {
        auto &entry = *i;
        const std::string prefix = "QtWeb";

        auto fileName = entry.path().filename();

        // skip files which don't start with prefix
        if (!strStartsWith(fileName.string(), prefix))
            continue;

        if (!appDir.deployExecutable(*i, newLibexecPath))
            return false;
    }

    for (const auto &fileName : {"qtwebengine_resources.pak",
                                 "qtwebengine_devtools_resources.pak",
                                 "qtwebengine_resources_100p.pak",
                                 "qtwebengine_resources_200p.pak", "icudtl.dat"}) {
        auto path = qtDataPath / "resources" / fileName;

        if (bf::is_regular_file(path))
            appDir.deployFile(path, appDir.path() / "usr/resources/");
    }

    if (bf::is_directory(qtTranslationsPath / "qtwebengine_locales")) {
        for (bf::directory_iterator i(qtTranslationsPath / "qtwebengine_locales"); i != bf::directory_iterator(); ++i) {
            appDir.deployFile(*i, appDir.path() / "usr/translations/qtwebengine_locales/");
        }
    }

    const auto qtConfPath = newLibexecPath / "qt.conf";

    std::ofstream ofs(qtConfPath.string());

    if (!ofs) {
        ldLog() << LD_ERROR << "Failed to open" << qtConfPath << "for writing" << std::endl;
        return false;
    }

    ofs << "# generated by linuxdeploy" << std::endl
        << "[Paths]" << std::endl
        << "Prefix = ../" << std::endl;

    return true;
}

bool deploy3DPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying Qt 3D plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "geometryloaders"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/geometryloaders/"))
            return false;
    }

    for (bf::directory_iterator i(qtPluginsPath / "sceneparsers"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/sceneparsers/"))
            return false;
    }

    return true;
}

bool deployGamepadPlugins(appdir::AppDir &appDir, const bf::path &qtPluginsPath) {
    ldLog() << "Deploying Gamepad plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "gamepads"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/gamepads/"))
            return false;
    }

    return true;
}

bool createQtConf(appdir::AppDir &appDir) {
    auto qtConfPath = appDir.path() / "usr" / "bin" / "qt.conf";

    if (bf::is_regular_file(qtConfPath)) {
        ldLog() << LD_WARNING << "Overwriting existing qt.conf file:" << qtConfPath << std::endl;
    } else {
        ldLog() << "Creating Qt conf file:" << qtConfPath << std::endl;
    }

    std::ofstream ofs(qtConfPath.string());

    if (!ofs) {
        ldLog() << LD_ERROR << "Failed to open qt.conf for writing" << std::endl;
        return false;
    }

    ofs << "# generated by linuxdeploy-plugin-qt" << std::endl
        << "[Paths]" << std::endl
        << "Prefix = ../" << std::endl
        << "Plugins = plugins" << std::endl
        << "Imports = qml" << std::endl
        << "Qml2Imports = qml" << std::endl;

    return true;
}

bool createAppRunHook(appdir::AppDir &appDir) {
    auto hookPath = appDir.path() / "apprun-hooks" / "linuxdeploy-plugin-qt-hook.sh";

    try {
        bf::create_directories(hookPath.parent_path());
    } catch (const bf::filesystem_error& e) {
        ldLog() << LD_ERROR << "Failed to create hooks directory:" << e.what() << std::endl;
        return false;
    }

    if (bf::is_regular_file(hookPath)) {
        ldLog() << LD_WARNING << "Overwriting existing AppRun hook file:" << hookPath << std::endl;
    } else {
        ldLog() << "Creating AppRun hook file:" << hookPath << std::endl;
    }

    std::ofstream ofs(hookPath.string());

    if (!ofs) {
        ldLog() << LD_ERROR << "Failed to open AppRun hook for writing" << std::endl;
        return false;
    }

    ofs << "# generated by linuxdeploy-plugin-qt" << std::endl
        << std::endl
        << "# try to make Qt apps more \"native looking\", if possible" << std::endl
        << "# see also https://github.com/AppImage/AppImageKit/issues/977#issue-462374883" << std::endl
        << "# and https://github.com/AppImage/AppImageKit/issues/977#issue-462374883" << std::endl
        << "case \"${XDG_CURRENT_DESKTOP}\" in" << std::endl
        << "    *GNOME*|*gnome*|*XFCE*)" << std::endl
        << "        export QT_QPA_PLATFORMTHEME=gtk2" << std::endl
        << "        ;;" << std::endl
        << "esac" << std::endl;

    return true;
}

bool
deployTranslations(appdir::AppDir &appDir, const bf::path &qtTranslationsPath, const std::vector<QtModule> &modules) {
    if (qtTranslationsPath.empty() || !bf::is_directory(qtTranslationsPath)) {
        ldLog() << LD_WARNING << "Translation directory does not exist, skipping deployment";
        return true;
    }

    ldLog() << "Qt translations directory:" << qtTranslationsPath << std::endl;

    auto checkName = [&appDir, &modules](const bf::path &fileName) {
        if (!strEndsWith(fileName.string(), ".qm"))
            return false;

        // always deploy basic Qt translations
        if (strStartsWith(fileName.string(), "qt_") && bf::basename(fileName).size() >= 5 &&
            bf::basename(fileName).size() <= 6)
            return true;

        for (const auto &module : modules) {
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

    const auto& appDirTranslationsPath = appDir.path() / "usr/translations";
    for (auto& i: bf::recursive_directory_iterator(appDir.path())) {
        if (!bf::is_regular_file(i) || pathContainsFile(appDirTranslationsPath, i))
            continue;

        const auto fileName = i.path().filename();

        if (strEndsWith(fileName.string(), ".qm"))
            appDir.createRelativeSymlink(i, appDir.path() / "usr/translations" / fileName);
    }

    return true;
}

bool deployQmlFiles(appdir::AppDir &appDir, const bf::path &installLibsPath) {
    try {
        deployQml(appDir, installLibsPath);
    } catch (const QmlImportScannerError &) {
        return false;
    }

    return true;
}

int main(const int argc, const char *const *const argv) {
    // set up verbose logging if $DEBUG is set
    if (getenv("DEBUG"))
        ldLog::setVerbosity(LD_DEBUG);

    args::ArgumentParser parser("linuxdeploy Qt plugin",
                                "Bundles Qt resources. For use with an existing AppDir, created by linuxdeploy.");

    args::ValueFlag<bf::path> appDirPath(parser, "appdir path", "Path to an existing AppDir", {"appdir"});
    args::ValueFlagList<std::string> extraPlugins(parser, "plugin",
                                                  "Extra Qt plugin to deploy (specified by name, filename or path)",
                                                  {'p', "extra-plugin"});

    args::Flag pluginType(parser, "", "Print plugin type and exit", {"plugin-type"});
    args::Flag pluginApiVersion(parser, "", "Print plugin API version and exit", {"plugin-api-version"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::ParseError &) {
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

    // allow disabling copyright files deployment via environment variable
    if (getenv("DISABLE_COPYRIGHT_FILES_DEPLOYMENT") != nullptr) {
        ldLog() << std::endl << LD_WARNING << "Copyright files deployment disabled" << std::endl;
        appDir.setDisableCopyrightFilesDeployment(true);
    }

    // check which libraries and plugins the binaries and libraries depend on
    std::set<std::string> libraryNames;
    for (const auto &path : appDir.listSharedLibraries()) {
        libraryNames.insert(path.filename().string());
        try {
            for (const auto &dependency : elf::ElfFile(path).traceDynamicDependencies()) {
                libraryNames.insert(dependency.filename().string());
            }
        } catch (const elf::ElfFileParseError &e) {
            ldLog() << LD_DEBUG << "Failed to parse file as ELF file:" << path << std::endl;
        }
    }

    {
        ldLog() << LD_DEBUG << "Libraries to consider: ";
        for (const auto &libraryName : libraryNames)
            ldLog() << " " << libraryName;
        ldLog() << std::endl;
    }

    // check for Qt modules
    std::vector<QtModule> foundQtModules;
    std::vector<QtModule> extraQtModules;

    auto matchesQtModule = [](std::string libraryName, const QtModule &module) {
        // extract filename if argument is path
        if (bf::is_regular_file(libraryName))
            libraryName = bf::path(libraryName).filename().string();

        // adding the trailing dot makes sure e.g., libQt5WebEngineCore won't be matched as webengine and webenginecore
        const auto &libraryPrefix = module.libraryFilePrefix + ".";

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

    std::copy_if(QtModules.begin(), QtModules.end(), std::back_inserter(foundQtModules),
                 [&matchesQtModule, &libraryNames](const QtModule &module) {
                     return std::find_if(libraryNames.begin(), libraryNames.end(),
                                         [&matchesQtModule, &module](const std::string &libraryName) {
                                             return matchesQtModule(libraryName, module);
                                         }) != libraryNames.end();
                 });

    std::vector<std::string> extraPluginsFromEnv;
    const auto* const extraPluginsFromEnvData = getenv("EXTRA_QT_PLUGINS");
    if (extraPluginsFromEnvData != nullptr)
        extraPluginsFromEnv = linuxdeploy::util::split(std::string(extraPluginsFromEnvData, ';'));

    for (const auto& pluginsList : {static_cast<std::vector<std::string>>(extraPlugins.Get()), extraPluginsFromEnv}) {
        std::copy_if(QtModules.begin(), QtModules.end(), std::back_inserter(extraQtModules),
            [&matchesQtModule, &libraryNames, &pluginsList](const QtModule &module) {
                return std::find_if(pluginsList.begin(), pluginsList.end(),
                    [&matchesQtModule, &module](const std::string &libraryName) {
                        return matchesQtModule(libraryName, module);
                    }) != pluginsList.end();
            }
        );
    }

    {
        std::set<std::string> moduleNames;
        std::for_each(foundQtModules.begin(), foundQtModules.end(), [&moduleNames](const QtModule &module) {
            moduleNames.insert(module.name);
        });
        ldLog() << "Found Qt modules:" << join(moduleNames) << std::endl;
    }

    {
        std::set<std::string> moduleNames;
        std::for_each(extraQtModules.begin(), extraQtModules.end(), [&moduleNames](const QtModule &module) {
            moduleNames.insert(module.name);
        });
        ldLog() << "Extra Qt modules:" << join(moduleNames) << std::endl;
    }

    if (foundQtModules.empty() && extraQtModules.empty()) {
        ldLog() << LD_ERROR << "Could not find Qt modules to deploy" << std::endl;
        return 1;
    }

    auto qmakePath = findQmake();

    if (qmakePath.empty()) {
        ldLog() << LD_ERROR << "Could not find qmake, please install or provide path using $QMAKE" << std::endl;
        return 1;
    }

    if (!bf::exists(qmakePath)) {
        ldLog() << LD_ERROR << "No such file or directory:" << qmakePath << std::endl;
        return 1;
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
    const bf::path qtBinsPath = qmakeVars["QT_INSTALL_BINS"];
    const bf::path qtLibsPath = qmakeVars["QT_INSTALL_LIBS"];
    const bf::path qtInstallQmlPath = qmakeVars["QT_INSTALL_QML"];

    ldLog() << std::endl;
    ldLog() << "QT_INSTALL_LIBS:" << qtLibsPath << std::endl;
    std::ostringstream newLibraryPath;
    newLibraryPath << qtLibsPath.string() << ":" << getenv("LD_LIBRARY_PATH");
    setenv("LD_LIBRARY_PATH", newLibraryPath.str().c_str(), true);
    ldLog() << "Prepending QT_INSTALL_LIBS path to $LD_LIBRARY_PATH, new $LD_LIBRARY_PATH:" << newLibraryPath.str()
            << std::endl;

    std::ostringstream newPath;
    newPath << qtBinsPath.string() << ":" << getenv("PATH");
    setenv("PATH", newPath.str().c_str(), true);
    ldLog() << "Prepending QT_INSTALL_BINS path to $PATH, new $PATH:" << newPath.str() << std::endl;


    auto qtModulesToDeploy = foundQtModules;
    qtModulesToDeploy.reserve(extraQtModules.size());
    std::copy(extraQtModules.begin(), extraQtModules.end(), std::back_inserter(qtModulesToDeploy));

    for (const auto &module : qtModulesToDeploy) {
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

        if (module.name == "qml") {
            if (!deployQmlFiles(appDir, qtInstallQmlPath))
                return 1;
        }

        if (module.name == "3dquickrender") {
            if (!deploy3DPlugins(appDir, qtPluginsPath))
                return 1;
        }

        if (module.name == "gamepad") {
            if (!deployGamepadPlugins(appDir, qtPluginsPath))
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
    if (!createQtConf(appDir)) {
        ldLog() << LD_ERROR << "Failed to create qt.conf in AppDir" << std::endl;
        return 1;
    }

    ldLog() << std::endl << "-- Creating AppRun hook --" << std::endl;
    if (!createAppRunHook(appDir)) {
        ldLog() << LD_ERROR << "Failed to create AppRun hook in AppDir" << std::endl;
        return 1;
    }

    ldLog() << std::endl << "Done!" << std::endl;
    return 0;
}
