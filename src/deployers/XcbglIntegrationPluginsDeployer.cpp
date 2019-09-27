// library headers
#include <linuxdeploy/core/log.h>
#include <boost/filesystem.hpp>

// local headers
#include "XcbglIntegrationPluginsDeployer.h"
#include "deployment.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace bf = boost::filesystem;

bool XcbglIntegrationPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying xcb-gl integrations" << std::endl;

    return deployIntegrationPlugins(appDir, qtPluginsPath, {"xcbglintegrations/"});
}
