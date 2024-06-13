// system headers
#include <filesystem>
#include <utility>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "BasicPluginsDeployer.h"

using namespace linuxdeploy::core::log;
using namespace linuxdeploy::core::appdir;
using namespace linuxdeploy::plugin::qt;

namespace fs = std::filesystem;

BasicPluginsDeployer::BasicPluginsDeployer(std::string moduleName,
                                           core::appdir::AppDir& appDir,
                                           fs::path qtPluginsPath,
                                           fs::path qtLibexecsPath,
                                           fs::path installLibsPath,
                                           fs::path qtTranslationsPath,
                                           fs::path qtDataPath) : moduleName(std::move(moduleName)),
                                                                  appDir(appDir),
                                                                  qtPluginsPath(std::move(qtPluginsPath)),
                                                                  qtLibexecsPath(std::move(qtLibexecsPath)),
                                                                  qtInstallQmlPath(std::move(installLibsPath)),
                                                                  qtTranslationsPath(std::move(qtTranslationsPath)),
                                                                  qtDataPath(std::move(qtDataPath)) {}

bool BasicPluginsDeployer::deploy() {
    for (const auto &pluginName : qtPluginsToBeDeployed()) {
        ldLog() << "Deploying" << pluginName << "plugins" << std::endl;
        for (fs::directory_iterator i(qtPluginsPath / pluginName); i != fs::directory_iterator(); ++i) {
            if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins" / pluginName))
                return false;
        }
    }

    return customDeploy();
}

bool BasicPluginsDeployer::customDeploy() {
    return true;
}

std::vector<std::string> BasicPluginsDeployer::qtPluginsToBeDeployed() const {
    return {};
}
