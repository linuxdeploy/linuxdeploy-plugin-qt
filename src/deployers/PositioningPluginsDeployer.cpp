// system headers
#include <filesystem>

// local headers
#include "PositioningPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool PositioningPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"position"});
}
