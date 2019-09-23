// local headers
#include "plugin_deployer_factory.h"
#include "basic_plugin_deployer.h"
#include "platform_plugins_deployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::appdir;
namespace bf = boost::filesystem;

std::shared_ptr<PluginDeployer>
PluginDeployerFactory::getInstance(const std::string& moduleName, AppDir& appDir, const bf::path& qtPluginsPath) {
    if (moduleName == "gui") {
        return std::make_shared<PlatformPluginsDeployer>(moduleName, appDir, qtPluginsPath);
    }

    // fallback
    return std::make_shared<BasicPluginDeployer>(moduleName, appDir, qtPluginsPath);
}
