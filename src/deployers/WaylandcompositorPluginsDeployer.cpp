// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "WaylandcompositorPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> WaylandcompositorPluginsDeployer::qtPluginsToBeDeployed() const {
    return {"wayland-decoration-client",
            "wayland-graphics-integration-client",
            "wayland-shell-integration",};
}
