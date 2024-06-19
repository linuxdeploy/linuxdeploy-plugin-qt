// system headers
#include <filesystem>

// local headers
#include "BearerPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool BearerPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"bearer"});
}
