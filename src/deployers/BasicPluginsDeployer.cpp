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
    // currently this is a no-op, but we might add more functionality later on, such as some kinds of default
    // attempts to copy data based on the moduleName
    return doDeploy();
}

bool BasicPluginsDeployer::deployStandardQtPlugins(const std::vector<std::string>& plugins)
{
    for (const auto &pluginName : plugins) {
        ldLog() << "Deploying Qt" << pluginName << "plugins" << std::endl;
        for (fs::directory_iterator i(qtPluginsPath / pluginName); i != fs::directory_iterator(); ++i) {
            if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins" / pluginName))
                return false;
        }
    }
    return true;
}

bool BasicPluginsDeployer::doDeploy() {
    return true;
}
