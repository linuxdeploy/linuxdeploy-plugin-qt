// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "GamepadPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> GamepadPluginsDeployer::qtPluginsToBeDeployed() const {
    return {"gamepads"};
}
