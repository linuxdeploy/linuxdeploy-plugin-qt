// system headers
#include <filesystem>

// local headers
#include "LocationPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool LocationPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"geoservices"});
}
