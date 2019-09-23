#pragma once

#include <string>

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            /**
             * Interface for deployer classes.
             */
            class PluginDeployer {
            public:
                virtual void deploy() = 0;
            };
        }
    }
}
