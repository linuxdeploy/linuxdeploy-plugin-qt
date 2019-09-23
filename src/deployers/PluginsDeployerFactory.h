#pragma once

// system headers
#include <memory>
#include <string>

// library headers
#include <linuxdeploy/core/appdir.h>
#include <boost/filesystem.hpp>

// local headers
#include "PluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PluginsDeployerFactory {
            private:
                core::appdir::AppDir& appDir;
                const boost::filesystem::path qtPluginsPath;

            public:
                explicit PluginsDeployerFactory(core::appdir::AppDir& appDir, boost::filesystem::path qtPluginsPath);

                std::shared_ptr<PluginsDeployer> getInstance(const std::string& moduleName);
            };
        }
    }
}
