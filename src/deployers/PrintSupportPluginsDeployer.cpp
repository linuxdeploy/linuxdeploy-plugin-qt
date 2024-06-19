// system headers
#include <filesystem>

// local headers
#include "PrintSupportPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool PrintSupportPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"printsupport"});
}
