// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/log/log.h>
#include <linuxdeploy/util/util.h>

// local headers
#include "PlatformPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::log;

namespace fs = std::filesystem;

bool PlatformPluginsDeployer::doDeploy() {
    ldLog() << "Deploying platform plugins" << std::endl;

    // always deploy default platform
    if (!appDir.deployLibrary(qtPluginsPath / "platforms/libqxcb.so", appDir.path() / "usr/plugins/platforms/"))
        return false;

    // deploy extra platform plugins, if any
    const auto* const platformPluginsFromEnvData = getenv("EXTRA_PLATFORM_PLUGINS");
    if (platformPluginsFromEnvData != nullptr) {
        for (const auto& platformToDeploy : linuxdeploy::util::split(std::string(platformPluginsFromEnvData), ';')) {
            ldLog() << "Deploying extra platform plugin: " << platformToDeploy << std::endl;
            if (!appDir.deployLibrary(qtPluginsPath / "platforms" / platformToDeploy, appDir.path() / "usr/plugins/platforms/"))
                return false;

            using namespace linuxdeploy::util::misc;
            if (stringStartsWith(platformToDeploy, "libqwayland")) {
                if (!deployStandardQtPlugins({"wayland-decoration-client", "wayland-shell-integration"})) {
                    return false;
                }
            }
        }
    }

    if (!deployStandardQtPlugins({"platforminputcontexts", "imageformats"})) {
        return false;
    }

    // TODO: platform themes -- https://github.com/probonopd/linuxdeployqt/issues/236

    const fs::path platformThemesPath = qtPluginsPath / "platformthemes";
    const fs::path stylesPath = qtPluginsPath / "styles";

    const fs::path platformThemesDestination = appDir.path() / "usr/plugins/platformthemes/";
    const fs::path stylesDestination = appDir.path() / "usr/plugins/styles/";

    if (getenv("DEPLOY_PLATFORM_THEMES") != nullptr) {
        ldLog() << LD_WARNING << "Deploying all platform themes and styles [experimental feature]" << std::endl;

        if (fs::is_directory(platformThemesPath))
            for (fs::directory_iterator i(platformThemesPath); i != fs::directory_iterator(); ++i)
                if (!appDir.deployLibrary(*i, platformThemesDestination))
                    return false;

        if (fs::is_directory(stylesPath))
            for (fs::directory_iterator i(stylesPath); i != fs::directory_iterator(); ++i)
                if (!appDir.deployLibrary(*i, stylesDestination))
                    return false;
    } else {
        ldLog() << "Trying to deploy platform themes and style" << std::endl;

        const auto libqxdgPath = platformThemesPath / "libqxdgdesktopportal.so";

        for (const auto &file : {libqxdgPath}) {
            // we need to check whether the files exist at least, otherwise the deferred deployment operation fails
            if (fs::is_regular_file(file)) {
                ldLog() << "Attempting to deploy" << file.filename() << "found at path" << file.parent_path() << std::endl;
                appDir.deployFile(file, platformThemesDestination);
            } else {
                ldLog() << "Could not find" << file.filename() << "on system, skipping deployment" << std::endl;
            }
        }
    }

    return true;
}
