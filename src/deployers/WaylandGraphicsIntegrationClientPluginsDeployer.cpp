// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/util/util.h>

// local headers
#include "WaylandGraphicsIntegrationClientPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace fs = std::filesystem;

bool WaylandGraphicsIntegrationClientPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying Wayland Shell Integration plugins" << std::endl;

    for (fs::directory_iterator i(qtPluginsPath / "wayland-graphics-integration-client"); i != fs::directory_iterator(); ++i) {
        if (i->path().extension() == ".debug") {
            ldLog() << LD_DEBUG << "skipping .debug file:" << i->path() << std::endl;
            continue;
        }

        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/wayland-graphics-integration-client/"))
            return false;
    }

    return true;
}
