// system headers
#include <filesystem>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "TlsBackendsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

std::vector<std::string> TlsBackendsDeployer::qtPluginsToBeDeployed() const {
    return {"tls"};
}
