// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/log/log.h>

// local headers
#include "Multimedia6PluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::log;

namespace fs = std::filesystem;

bool Multimedia6PluginsDeployer::doDeploy() {
    if (fs::exists(qtPluginsPath / "multimedia")) {
        return deployStandardQtPlugins({"multimedia"});
    } else {
        ldLog() << LD_WARNING << "Missing Qt 6 multimedia plugins, skipping." << std::endl;
        return true;
    }
}
