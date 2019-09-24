// local headers
#include "PluginsDeployerFactory.h"
#include "BasicPluginsDeployer.h"
#include "PlatformPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::appdir;
namespace bf = boost::filesystem;

PluginsDeployerFactory::PluginsDeployerFactory(AppDir& appDir,
                                               bf::path qtPluginsPath,
                                               bf::path qtLibexecsPath,
                                               bf::path qtInstallQmlPath,
                                               bf::path qtTranslationsPath,
                                               bf::path qtDataPath) : appDir(appDir),
                                                                      qtPluginsPath(std::move(qtPluginsPath)),
                                                                      qtLibexecsPath(std::move(qtLibexecsPath)),
                                                                      qtInstallQmlPath(std::move(qtInstallQmlPath)),
                                                                      qtTranslationsPath(std::move(qtTranslationsPath)),
                                                                      qtDataPath(std::move(qtDataPath)) {}

std::shared_ptr<PluginsDeployer> PluginsDeployerFactory::getInstance(const std::string& moduleName) {
    if (moduleName == "gui") {
        return getInstance<PlatformPluginsDeployer>(moduleName);
    }
    
    if (moduleName == "opengl" || moduleName == "gui" || moduleName == "xcbqpa") {
        return getInstance<PlatformPluginsDeployer>(moduleName);
    }

    // fallback
    return getInstance<BasicPluginsDeployer>(moduleName);
}
