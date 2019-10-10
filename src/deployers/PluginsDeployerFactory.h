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
                const boost::filesystem::path qtLibexecsPath;
                const boost::filesystem::path qtInstallQmlPath;
                const boost::filesystem::path qtTranslationsPath;
                const boost::filesystem::path qtDataPath;

                template<typename T>
                std::shared_ptr<PluginsDeployer> getInstance(const std::string& moduleName) {
                    static_assert(std::is_convertible<T*, PluginsDeployer*>::value, "T must inherit PluginsDeployer");

                    return std::make_shared<T>(
                        moduleName,
                        appDir,
                        qtPluginsPath,
                        qtLibexecsPath,
                        qtInstallQmlPath,
                        qtTranslationsPath,
                        qtDataPath
                    );
                }

            public:
                explicit PluginsDeployerFactory(core::appdir::AppDir& appDir,
                                                boost::filesystem::path qtPluginsPath,
                                                boost::filesystem::path qtLibexecsPath,
                                                boost::filesystem::path qtInstallQmlPath,
                                                boost::filesystem::path qtTranslationsPath,
                                                boost::filesystem::path qtDataPath);

                std::vector<std::shared_ptr<PluginsDeployer>> getDeployers(const std::string& moduleName);
            };
        }
    }
}
