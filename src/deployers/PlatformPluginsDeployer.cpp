// library headers
#include <linuxdeploy/core/log.h>
#include <boost/filesystem.hpp>

// local headers
#include "PlatformPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace bf = boost::filesystem;

bool PlatformPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

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
        const auto libqgtk2Path = platformThemesPath / "libqgtk2.so";
        const auto libqgtk3Path = platformThemesPath / "libqgtk3.so";
        const auto libqxdgPath = platformThemesPath / "libqxdgdesktopportal.so";

        for (const auto &file : {libqgtk2Path, libqgtk3Path, libqxdgPath}) {
            // we need to check whether the files exist at least, otherwise the deferred deployment operation fails
            if (bf::is_regular_file(file)) {
                ldLog() << "Attempting to deploy" << file.filename() << "found at path" << file.parent_path() << std::endl;
                appDir.deployFile(file, platformThemesDestination);
            } else {
                ldLog() << "Could not find" << file.filename() << "on system, skipping deployment" << std::endl;
            }
        }

        const auto libqgtk2stylePath = stylesPath / "libqgtk2style.so";
        if (bf::is_regular_file(libqgtk2stylePath)) {
            ldLog() << "Attempting to deploy" << libqgtk2stylePath.filename() << "found at path" << libqgtk2stylePath << std::endl;
            appDir.deployFile(libqgtk2stylePath, stylesDestination);
        } else {
            ldLog() << "Could not find" << libqgtk2stylePath.filename() << "on system, skipping deployment" << std::endl;
        }
    }

    return true;
}
