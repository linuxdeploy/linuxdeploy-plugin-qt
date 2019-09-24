// library headers
#include <linuxdeploy/core/log.h>
#include <boost/filesystem.hpp>

// local headers
#include "Qt3DPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace bf = boost::filesystem;

bool Qt3DPluginsDeployer::deploy() {
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying Qt 3D plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / "geometryloaders"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/geometryloaders/"))
            return false;
    }

    for (bf::directory_iterator i(qtPluginsPath / "sceneparsers"); i != bf::directory_iterator(); ++i) {
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/sceneparsers/"))
            return false;
    }

    return true;
}
