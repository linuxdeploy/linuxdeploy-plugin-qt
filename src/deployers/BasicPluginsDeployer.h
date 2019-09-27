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

                // Qt data
                const boost::filesystem::path qtPluginsPath;
                const boost::filesystem::path qtLibexecsPath;
                const boost::filesystem::path qtInstallQmlPath;
                const boost::filesystem::path qtTranslationsPath;
                const boost::filesystem::path qtDataPath;

            public:
                /**
                 * Default constructor. Constructs a basic deployer for a plugin with the given name.
                 *
                 * @param moduleName
                 */
                explicit BasicPluginsDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                              boost::filesystem::path qtPluginsPath,
                                              boost::filesystem::path qtLibexecsPath,
                                              boost::filesystem::path installLibsPath,
                                              boost::filesystem::path qtTranslationsPath,
                                              boost::filesystem::path qtDataPath);

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
