// local headers
#include "plugin_deployer_factory.h"
#include "basic_plugin_deployer.h"

using namespace linuxdeploy::plugin::qt;

std::shared_ptr <PluginDeployer> PluginDeployerFactory::getInstance(const std::string& moduleName) {
    std::shared_ptr<PluginDeployer> rv;

    // fallback
    if (rv == nullptr) {
        rv.reset(new BasicPluginDeployer(moduleName));
    }

    return rv;
}
