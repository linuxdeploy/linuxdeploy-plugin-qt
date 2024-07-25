#pragma once

// system headers
#include <filesystem>
#include <fstream>

// library includes
#include <linuxdeploy/core/appdir.h>
#include <linuxdeploy/core/elf_file.h>
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/util/util.h>

// local includes
#include "qt-modules.h"
#include "qml.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace linuxdeploy::core;
using namespace linuxdeploy::util::misc;
using namespace linuxdeploy::core::log;

// little helper called by other integration plugins
inline bool deployIntegrationPlugins(appdir::AppDir& appDir, const fs::path& qtPluginsPath, const std::initializer_list<fs::path>& subDirs) {
    for (const fs::path& subDir : subDirs) {
        // make sure the path ends with a / so that liblinuxdeploy recognize the destination as a directory
        auto dir = qtPluginsPath / subDir / "";

        if (!fs::is_directory(dir)) {
            ldLog() << "Directory" << dir << "doesn't exist, skipping deployment" << std::endl;
            continue;
        }

        for (fs::directory_iterator i(dir); i != fs::directory_iterator(); ++i) {
            // append a trailing slash to make linuxdeploy aware of the destination being a directory
            // otherwise, when the directory doesn't exist, it might just copy all files to files called like
            // destinationDir
            auto destinationDir = appDir.path() / "usr/plugins" / subDir / "";
            if (i->path().extension() == ".debug") {
                ldLog() << LD_DEBUG << "skipping .debug file:" << i->path() << std::endl;
                continue;
            }

            if (!appDir.deployLibrary(*i, destinationDir))
                return false;
        }
    }

    return true;
}

inline bool createQtConf(appdir::AppDir &appDir) {
    auto qtConfPath = appDir.path() / "usr" / "bin" / "qt.conf";

    if (fs::is_regular_file(qtConfPath)) {
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

inline bool createAppRunHook(appdir::AppDir &appDir) {
    auto hookPath = appDir.path() / "apprun-hooks" / "linuxdeploy-plugin-qt-hook.sh";

    try {
        fs::create_directories(hookPath.parent_path());
    } catch (const fs::filesystem_error& e) {
        ldLog() << LD_ERROR << "Failed to create hooks directory:" << e.what() << std::endl;
        return false;
    }

    if (fs::is_regular_file(hookPath)) {
        ldLog() << LD_WARNING << "Overwriting existing AppRun hook file:" << hookPath << std::endl;
    } else {
        ldLog() << "Creating AppRun hook file:" << hookPath << std::endl;
    }

    std::ofstream ofs(hookPath.string());

    if (!ofs) {
        ldLog() << LD_ERROR << "Failed to open AppRun hook for writing" << std::endl;
        return false;
    }

    // Old Qt versions only ship gtk2, new versions only gtk3. No Qt version had both.
    const bool haveGtk3 = fs::exists(appDir.path() / "usr/plugins/platformthemes" / "libqgtk3.so");

    ofs << "# generated by linuxdeploy-plugin-qt" << std::endl
        << std::endl
        << "# try to make Qt apps more \"native looking\" on Gtk-based desktops, if possible" << std::endl
        << "# see https://github.com/AppImage/AppImageKit/issues/977#issue-462374883" << std::endl
        << "case \"${XDG_CURRENT_DESKTOP}\" in" << std::endl
        << "    *GNOME*|*gnome*|*XFCE*)" << std::endl
        << "        export QT_QPA_PLATFORMTHEME=" << (haveGtk3 ? "gtk3" : "gtk2") << std::endl
        << "        ;;" << std::endl
        << "esac" << std::endl;

    return true;
}

inline bool
deployTranslations(appdir::AppDir &appDir, const fs::path &qtTranslationsPath, const std::vector<QtModule> &modules) {
    if (qtTranslationsPath.empty() || !fs::is_directory(qtTranslationsPath)) {
        ldLog() << LD_WARNING << "Translation directory does not exist, skipping deployment";
        return true;
    }

    ldLog() << "Qt translations directory:" << qtTranslationsPath << std::endl;

    auto checkName = [&appDir, &modules](const fs::path &fileName) {
        if (!strEndsWith(fileName.string(), ".qm"))
            return false;

        // always deploy basic Qt translations
        if (strStartsWith(fileName.string(), "qt_") && fileName.filename().string().size() >= 5 &&
            fileName.filename().string().size() <= 6)
            return true;

        for (const auto &module : modules) {
            if (!module.translationFilePrefix.empty() && strStartsWith(fileName.string(), module.translationFilePrefix))
                return true;
        }

        return false;
    };

    for (fs::directory_iterator i(qtTranslationsPath); i != fs::directory_iterator(); ++i) {
        if (!fs::is_regular_file(*i))
            continue;

        const auto fileName = (*i).path().filename();

        if (checkName(fileName))
            appDir.deployFile(*i, appDir.path() / "usr/translations/");
    }

    const auto& appDirTranslationsPath = appDir.path() / "usr/translations";
    for (auto& i: fs::recursive_directory_iterator(appDir.path())) {
        if (!fs::is_regular_file(i) || pathContainsFile(appDirTranslationsPath, i))
            continue;

        const auto fileName = i.path().filename();

        if (strEndsWith(fileName.string(), ".qm"))
            appDir.createRelativeSymlink(i, appDir.path() / "usr/translations" / fileName);
    }

    return true;
}
