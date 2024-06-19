// system headers
#include <filesystem>

// local headers
#include "Multimedia5PluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool Multimedia5PluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"mediaservice", "audio"});
}
