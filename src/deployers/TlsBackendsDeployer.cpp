// system headers
#include <filesystem>

// local headers
#include "TlsBackendsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool TlsBackendsDeployer::doDeploy() {
    return deployStandardQtPlugins({"tls"});
}
