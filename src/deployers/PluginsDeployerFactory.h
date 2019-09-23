#pragma once

// system headers
#include <memory>
#include <string>

// library headers
#include <linuxdeploy/core/appdir.h>
#include <boost/filesystem.hpp>

// local headers
#include "PluginsDeployer.h"
#include "BasicPluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PluginsDeployerFactory {
            private:
                core::appdir::AppDir& appDir;
                const boost::filesystem::path qtPluginsPath;

                template<typename T>
                std::shared_ptr<PluginsDeployer> getInstance(const std::string& moduleName) {
                    static_assert(std::is_convertible<T*, PluginsDeployer*>::value, "T must inherit PluginsDeployer");

                    return std::make_shared<T>(moduleName, appDir, qtPluginsPath);
                }

            public:
                explicit PluginsDeployerFactory(core::appdir::AppDir& appDir, boost::filesystem::path qtPluginsPath);

                std::shared_ptr<PluginsDeployer> getInstance(const std::string& moduleName);
            };
        }
    }
}
