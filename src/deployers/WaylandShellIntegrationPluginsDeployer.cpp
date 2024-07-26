// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>
#include <linuxdeploy/util/util.h>

// local headers
#include "WaylandShellIntegrationPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace fs = std::filesystem;

bool WaylandShellIntegrationPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying Wayland Shell Integration plugins" << std::endl;

    // always deploy default platform
    if (!appDir.deployLibrary(qtPluginsPath / "wayland-shell-integration/libxdg-shell.so", appDir.path() / "usr/plugins/wayland-shell-integration/"))
        return false;

    // deploy Wayland Shell Integration platform plugins, if any
    const auto* const platformPluginsFromEnvData = getenv("EXTRA_WAYLAND_SHELL_INTEGRATION_PLUGINS");
    if (platformPluginsFromEnvData != nullptr) {
        for (const auto& platformToDeploy : linuxdeploy::util::split(std::string(platformPluginsFromEnvData), ';')) {
            ldLog() << "Deploying extra Wayland Shell Integration plugin: " << platformToDeploy << std::endl;
            if (!appDir.deployLibrary(qtPluginsPath / "wayland-shell-integration" / platformToDeploy, appDir.path() / "usr/plugins/wayland-shell-integration/"))
                return false;
        }
     }

    return true;
}
