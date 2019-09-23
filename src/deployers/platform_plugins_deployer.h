#pragma once

#include "basic_plugin_deployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PlatformPluginsDeployer : public BasicPluginDeployer {
            public:
                // we can just use the base class's constructor
                using BasicPluginDeployer::BasicPluginDeployer;

                bool deploy() override;
            };
        }
    }
}
