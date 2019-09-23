
// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "basic_plugin_deployer.h"

using namespace linuxdeploy::core::log;

class BasicPluginDeployer::Private {
public:
    std::string moduleName;
};

BasicPluginDeployer::BasicPluginDeployer(const std::string& pluginName) : d(new Private) {
    d->moduleName = pluginName;
}

void BasicPluginDeployer::deploy() {
    ldLog() << std::endl << "-- Deploying module:" << d->moduleName << "--" << std::endl;
}
