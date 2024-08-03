// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "Multimedia6PluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace fs = std::filesystem;

bool Multimedia6PluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    if (fs::exists(qtPluginsPath / "multimedia")) {
        ldLog() << "Deploying multimedia plugins" << std::endl;

        for (fs::directory_iterator i(qtPluginsPath / "multimedia"); i != fs::directory_iterator(); ++i) {
            if (i->path().extension() == ".debug") {
                ldLog() << LD_DEBUG << "skipping .debug file:" << i->path() << std::endl;
                continue;
            }

            if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/multimedia/"))
                return false;
        }
    } else {
        ldLog() << LD_WARNING << "Missing Qt 6 multimedia plugins, skipping." << std::endl;
    }

    return true;
}
