// system headers
#include <filesystem>

// local headers
#include "WaylandcompositorPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool WaylandcompositorPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"wayland-decoration-client",
        "wayland-graphics-integration-client",
        "wayland-shell-integration",});
}
