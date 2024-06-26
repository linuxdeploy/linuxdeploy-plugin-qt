#pragma once

// system headers
#include <filesystem>
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

                // Qt data
                const std::filesystem::path qtPluginsPath;
                const std::filesystem::path qtLibexecsPath;
                const std::filesystem::path qtInstallQmlPath;
                const std::filesystem::path qtTranslationsPath;
                const std::filesystem::path qtDataPath;

            public:
                /**
                 * Default constructor. Constructs a basic deployer for a plugin with the given name.
                 *
                 * @param moduleName
                 */
                explicit BasicPluginsDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                              std::filesystem::path qtPluginsPath,
                                              std::filesystem::path qtLibexecsPath,
                                              std::filesystem::path installLibsPath,
                                              std::filesystem::path qtTranslationsPath,
                                              std::filesystem::path qtDataPath);

                /**
                 * Default destroyer is good enough for this class for now, but in case we need to change this we declare a virtual
                 * one.
                 */
                virtual ~BasicPluginsDeployer() = default;

            public:
                /**
                 * This method might make some deployment preparation and calls \sa doDeploy() to finalize the deployment.
                 */
                bool deploy() override final;

            protected:
                /**
                 * This method does the actual moduleName deployment, where any special case should be handled and
                 * \sa deployStandardQtPlugins () method should be called to deploy Qt plugins that follow the default
                 * name and path scheme.
                 */
                virtual bool doDeploy();

                /**
                 * Deploys a list of Qt plugin that should be deployed and
                 * follow the default name and path scheme.
                 */
                bool deployStandardQtPlugins(const std::vector<std::string>& plugins);
            };
        }
    }
}
