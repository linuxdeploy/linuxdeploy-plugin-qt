#pragma once

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
#include "deployers/PluginsDeployerFactory.h"

namespace bf = boost::filesystem;

using namespace linuxdeploy::core;
using namespace linuxdeploy::util::misc;
using namespace linuxdeploy::core::log;
using namespace linuxdeploy::plugin::qt;

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
