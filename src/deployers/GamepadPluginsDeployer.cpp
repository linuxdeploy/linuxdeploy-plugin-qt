// system headers
#include <filesystem>

// local headers
#include "GamepadPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool GamepadPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"gamepads"});
}
