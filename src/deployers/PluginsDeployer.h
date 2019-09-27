#pragma once

#include <string>

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            /**
             * Interface for deployer classes.
             */
            class PluginsDeployer {
            public:
                virtual bool deploy() = 0;
            };
        }
    }
}
