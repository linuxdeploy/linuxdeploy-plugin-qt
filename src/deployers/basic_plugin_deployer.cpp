// system headers
#include <utility>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "basic_plugin_deployer.h"

using namespace linuxdeploy::core::log;
using namespace linuxdeploy::core::appdir;
using namespace linuxdeploy::plugin::qt;
namespace bf = boost::filesystem;

BasicPluginDeployer::BasicPluginDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                         bf::path qtPluginsPath) : moduleName(std::move(moduleName)), appDir(appDir),
                                                                   qtPluginsPath(std::move(qtPluginsPath)) {}

bool BasicPluginDeployer::deploy() {
    ldLog() << std::endl << "-- Deploying module:" << moduleName << "--" << std::endl;
    return true;
}
