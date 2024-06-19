// system headers
#include <filesystem>

// local headers
#include "SqlPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool SqlPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"sqldrivers"});
}
