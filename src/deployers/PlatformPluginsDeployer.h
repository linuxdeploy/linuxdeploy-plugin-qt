#pragma once

#include "BasicPluginsDeployer.h"

namespace linuxdeploy {
    namespace plugin {
        namespace qt {
            class PlatformPluginsDeployer : public BasicPluginsDeployer {
            private:
                std::vector<std::string> platformToDeploy;

            public:
                /**
                 * Default constructor. Constructs a PlatformPluginsDeployer.
                 *
                 * @param moduleName
                 */
                explicit PlatformPluginsDeployer(std::string moduleName, core::appdir::AppDir& appDir,
                                        boost::filesystem::path qtPluginsPath,
                                        boost::filesystem::path qtLibexecsPath,
                                        boost::filesystem::path installLibsPath,
                                        boost::filesystem::path qtTranslationsPath,
                                        boost::filesystem::path qtDataPath);

                bool deploy() override;
            };
        }
    }
}
