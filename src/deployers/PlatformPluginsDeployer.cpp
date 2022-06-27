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

    if (getenv("ENABLE_EGLFS")){
        if (!appDir.deployLibrary(qtPluginsPath / "platforms/libqxcb.so", appDir.path() / "usr/plugins/platforms/"))
            ldLog() << "No XCB platform plugin found, skipping" << std::endl;
        if (!appDir.deployLibrary(qtPluginsPath / "platforms/libqeglfs.so", appDir.path() / "usr/plugins/platforms/"))
            ldLog() << "No EGLFS platform plugin found, skipping" << std::endl;
        if (!appDir.deployLibrary(qtPluginsPath / "egldeviceintegrations/libqeglfs-kms-integration.so", appDir.path() / "usr/plugins/egldeviceintegrations/"))
            ldLog() << "No EGLFS KMS Integration plugin found, skipping" << std::endl;
        if (!appDir.deployLibrary(qtPluginsPath / "egldeviceintegrations/libqeglfs-x11-integration.so", appDir.path() / "usr/plugins/egldeviceintegrations/"))
            ldLog() << "No EGLFS X11 Integration plugin found, skipping" << std::endl;
        if (!appDir.deployLibrary(qtPluginsPath / "egldeviceintegrations/libqeglfs-kms-egldevice-integration.so", appDir.path() / "usr/plugins/egldeviceintegrations/"))
            ldLog() << "No EGLFS KMS EGLDEVICE Integration plugin found, skipping" << std::endl;
        if (!appDir.deployLibrary(qtPluginsPath / "egldeviceintegrations/libqeglfs-kms-emu-integration.so", appDir.path() / "usr/plugins/egldeviceintegrations/"))
            ldLog() << "No EGLFS EMU Integration plugin found, skipping" << std::endl;
    } else {
        if (!appDir.deployLibrary(qtPluginsPath / "platforms/libqxcb.so", appDir.path() / "usr/plugins/platforms/"))
            return false;        
    }

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
        ldLog() << "Trying to deploy platform themes and style" << std::endl;

        const auto libqxdgPath = platformThemesPath / "libqxdgdesktopportal.so";

        for (const auto &file : {libqxdgPath}) {
            // we need to check whether the files exist at least, otherwise the deferred deployment operation fails
            if (bf::is_regular_file(file)) {
                ldLog() << "Attempting to deploy" << file.filename() << "found at path" << file.parent_path() << std::endl;
                appDir.deployFile(file, platformThemesDestination);
            } else {
                ldLog() << "Could not find" << file.filename() << "on system, skipping deployment" << std::endl;
            }
        }
    }

    return true;
}
