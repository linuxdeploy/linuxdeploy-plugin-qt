// local headers
#include "PluginsDeployerFactory.h"
#include "basic_plugin_deployer.h"
#include "platform_plugins_deployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::appdir;
namespace bf = boost::filesystem;

PluginsDeployerFactory::PluginsDeployerFactory(AppDir& appDir, bf::path qtPluginsPath) :
    appDir(appDir), qtPluginsPath(std::move(qtPluginsPath)) {}

std::shared_ptr<PluginsDeployer> PluginsDeployerFactory::getInstance(const std::string& moduleName) {
    if (moduleName == "gui") {
        return std::make_shared<PlatformPluginsDeployer>(moduleName, appDir, qtPluginsPath);
    }

    // fallback
    return std::make_shared<BasicPluginDeployer>(moduleName, appDir, qtPluginsPath);
}
