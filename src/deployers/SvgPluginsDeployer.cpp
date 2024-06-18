// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/log/log.h>

// local headers
#include "SvgPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::log;

namespace fs = std::filesystem;

bool SvgPluginsDeployer::doDeploy() {
    ldLog() << "Deploying svg icon engine" << std::endl;

    if (!appDir.deployLibrary(qtPluginsPath / "iconengines/libqsvgicon.so", appDir.path() / "usr/plugins/iconengines/"))
        return false;

    return true;
}
