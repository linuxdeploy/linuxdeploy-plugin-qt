// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "PrintSupportPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> PrintSupportPluginsDeployer::qtPluginsToBeDeployed() const {
    return {"printsupport"};
}
