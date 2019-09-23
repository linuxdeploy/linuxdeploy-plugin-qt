// system headers
#include <utility>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "BasicPluginsDeployer.h"

using namespace linuxdeploy::core::log;
using namespace linuxdeploy::core::appdir;
using namespace linuxdeploy::plugin::qt;
namespace bf = boost::filesystem;

BasicPluginsDeployer::BasicPluginsDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                           bf::path qtPluginsPath) : moduleName(std::move(moduleName)), appDir(appDir),
                                                                   qtPluginsPath(std::move(qtPluginsPath)) {}

bool BasicPluginsDeployer::deploy() {
    ldLog() << std::endl << "-- Deploying module:" << moduleName << "--" << std::endl;
    return true;
}
