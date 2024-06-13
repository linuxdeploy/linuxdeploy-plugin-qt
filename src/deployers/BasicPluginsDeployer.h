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
                 * This method deploys the plugins returned by \sa qtPluginsToBeDeployed()
                 * and call \sa customDeploy() to finalize the deployment.
                 */
                bool deploy() override final;

            protected:
                /**
                 * The \sa deploy() method can deploy Qt plugins that follow the default
                 * name and path scheme, but some modules are special so
                 * they should write custom deployment code.
                 */
                virtual bool customDeploy();

                /**
                 * Returns a list of Qt plugin names that should be deployed and
                 * follow the default name and path scheme.
                 */
                virtual std::vector<std::string> qtPluginsToBeDeployed() const;
            };
        }
    }
}
