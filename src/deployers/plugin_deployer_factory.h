#pragma once

// system headers
#include <memory>
#include <string>

// local headers
#include "plugin_deployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PluginDeployerFactory {
            public:
                static std::shared_ptr<PluginDeployer> getInstance(const std::string& moduleName);
            };
        }
    }
}
