// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "Qt3DPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> Qt3DPluginsDeployer::qtPluginsToBeDeployed() const {
    return {"geometryloaders", "sceneparsers"};
}
