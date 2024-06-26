// system headers
#include <filesystem>

// local headers
#include "Qt3DPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool Qt3DPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"geometryloaders", "sceneparsers"});
}
