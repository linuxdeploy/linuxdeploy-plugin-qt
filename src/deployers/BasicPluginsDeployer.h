#pragma once

// system headers
#include <memory>

// library headers
#include <linuxdeploy/core/appdir.h>

// local headers
#include "PluginsDeployer.h"


namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            /**
             *
             */
            class BasicPluginsDeployer : public PluginsDeployer {
            protected:
                std::string moduleName;
                core::appdir::AppDir& appDir;
                boost::filesystem::path qtPluginsPath;

            public:
                /**
                 * Default constructor. Constructs a basic deployer for a plugin with the given name.
                 *
                 * @param moduleName
                 */
                explicit BasicPluginsDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                              boost::filesystem::path qtPluginsPath);

                /**
                 * Default destroyer is good enough for this class for now, but in case we need to change this we declare a virtual
                 * one.
                 */
                virtual ~BasicPluginsDeployer() = default;

            public:
                bool deploy() override;
            };
        }
    }
}
