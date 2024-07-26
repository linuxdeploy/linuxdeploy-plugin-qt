#pragma once

#include "BasicPluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class WaylandGraphicsIntegrationClientPluginsDeployer : public BasicPluginsDeployer {
            public:
                // we can just use the base class's constructor
                using BasicPluginsDeployer::BasicPluginsDeployer;

                bool deploy() override;
            };
        }
    }
}