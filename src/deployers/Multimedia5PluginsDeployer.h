#pragma once

#include "BasicPluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class Multimedia5PluginsDeployer : public BasicPluginsDeployer {
            public:
                // we can just use the base class's constructor
                using BasicPluginsDeployer::BasicPluginsDeployer;

                bool deploy() override;
            };
        }
    }
}
