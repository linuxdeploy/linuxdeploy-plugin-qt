#pragma once

// system headers
#include <memory>
#include <string>

// library headers
#include <linuxdeploy/core/appdir.h>
#include <boost/filesystem.hpp>

// local headers
#include "plugin_deployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PluginDeployerFactory {
            public:
                static std::shared_ptr<PluginDeployer>
                getInstance(const std::string& moduleName, core::appdir::AppDir& appDir,
                            const boost::filesystem::path& qtPluginsPath);
            };
        }
    }
}
