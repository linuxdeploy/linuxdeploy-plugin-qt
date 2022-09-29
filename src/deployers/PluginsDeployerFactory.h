#pragma once

// system headers
#include <filesystem>
#include <memory>
#include <string>

// library headers
#include <linuxdeploy/core/appdir.h>

// local headers
#include "PluginsDeployer.h"
#include "BasicPluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PluginsDeployerFactory {
            private:
                core::appdir::AppDir& appDir;
                const std::filesystem::path qtPluginsPath;
                const std::filesystem::path qtLibexecsPath;
                const std::filesystem::path qtInstallQmlPath;
                const std::filesystem::path qtTranslationsPath;
                const std::filesystem::path qtDataPath;
                const int qtMajorVersion;
                const int qtMinorVersion;

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
                                                std::filesystem::path qtPluginsPath,
                                                std::filesystem::path qtLibexecsPath,
                                                std::filesystem::path qtInstallQmlPath,
                                                std::filesystem::path qtTranslationsPath,
                                                std::filesystem::path qtDataPath,
                                                int qtMajorVersion,
                                                int qtMinorVersion);

                std::vector<std::shared_ptr<PluginsDeployer>> getDeployers(const std::string& moduleName);
            };
        }
    }
}
