// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "XcbglIntegrationPluginsDeployer.h"
#include "deployment.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

bool XcbglIntegrationPluginsDeployer::customDeploy() {
    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    return deployIntegrationPlugins(appDir, qtPluginsPath, {"xcbglintegrations/"});
}
