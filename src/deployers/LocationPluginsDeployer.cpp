// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "LocationPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> LocationPluginsDeployer::qtPluginsToBeDeployed() const {
    return {"geoservices"};
}
