// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "Multimedia6PluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace fs = std::filesystem;

std::vector<std::string> Multimedia6PluginsDeployer::qtPluginsToBeDeployed() const {
    if (fs::exists(qtPluginsPath / "multimedia")) {
        return {"multimedia"};
    } else {
        ldLog() << LD_WARNING << "Missing Qt 6 multimedia plugins, skipping." << std::endl;
        return {};
    }
}
