// library headers
#include <linuxdeploy/log/log.h>

// local headers
#include "XcbglIntegrationPluginsDeployer.h"
#include "deployment.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::log;

bool XcbglIntegrationPluginsDeployer::doDeploy() {
    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    return deployIntegrationPlugins(appDir, qtPluginsPath, {"xcbglintegrations/"});
}
