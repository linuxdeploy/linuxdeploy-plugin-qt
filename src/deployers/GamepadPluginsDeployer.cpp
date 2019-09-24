// library headers
#include <linuxdeploy/core/log.h>
#include <boost/filesystem.hpp>

// local headers
#include "GamepadPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace bf = boost::filesystem;

bool GamepadPluginsDeployer::deploy() {
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying Gamepad plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "gamepads"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/gamepads/"))
            return false;
    }

    return true;
}
