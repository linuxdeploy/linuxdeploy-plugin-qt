// system headers
#include <utility>

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "BasicPluginsDeployer.h"

using namespace linuxdeploy::core::log;
using namespace linuxdeploy::core::appdir;
using namespace linuxdeploy::plugin::qt;
namespace bf = boost::filesystem;

BasicPluginsDeployer::BasicPluginsDeployer(std::string moduleName,
                                           core::appdir::AppDir& appDir,
                                           bf::path qtPluginsPath,
                                           bf::path qtLibexecsPath,
                                           bf::path installLibsPath,
                                           bf::path qtTranslationsPath,
                                           bf::path qtDataPath) : moduleName(std::move(moduleName)),
                                                                  appDir(appDir),
                                                                  qtPluginsPath(std::move(qtPluginsPath)),
                                                                  qtLibexecsPath(std::move(qtLibexecsPath)),
                                                                  qtInstallQmlPath(std::move(installLibsPath)),
                                                                  qtTranslationsPath(std::move(qtTranslationsPath)),
                                                                  qtDataPath(std::move(qtDataPath)) {}

bool BasicPluginsDeployer::deploy() {
    ldLog() << std::endl << "-- Deploying module:" << moduleName << "--" << std::endl;
    return true;
}
