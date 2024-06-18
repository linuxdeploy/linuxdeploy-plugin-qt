// library headers
#include <linuxdeploy/log/log.h>

// local headers
#include "XcbglIntegrationPluginsDeployer.h"
#include "deployment.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::log;

bool XcbglIntegrationPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    return deployIntegrationPlugins(appDir, qtPluginsPath, {"xcbglintegrations/"});
}
