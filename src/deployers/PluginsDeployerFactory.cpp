// local headers
#include "PluginsDeployerFactory.h"
#include "BasicPluginsDeployer.h"
#include "PlatformPluginsDeployer.h"
#include "BearerPluginsDeployer.h"
#include "GamepadPluginsDeployer.h"
#include "MultimediaPluginsDeployer.h"
#include "PrintSupportPluginsDeployer.h"
#include "PositioningPluginsDeployer.h"
#include "QmlPluginsDeployer.h"
#include "Qt3DPluginsDeployer.h"
#include "SqlPluginsDeployer.h"
#include "SvgPluginsDeployer.h"
#include "TextToSpeechPluginsDeployer.h"
#include "WebEnginePluginsDeployer.h"
#include "XcbglIntegrationPluginsDeployer.h"

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

std::vector<std::shared_ptr<PluginsDeployer>> PluginsDeployerFactory::getDeployers(const std::string& moduleName) {
    if (moduleName == "gui") {
        return {getInstance<PlatformPluginsDeployer>(moduleName), getInstance<XcbglIntegrationPluginsDeployer>(moduleName)};
    }

    if (moduleName == "opengl" || moduleName == "xcbqpa") {
        return {getInstance<XcbglIntegrationPluginsDeployer>(moduleName)};
    }

    if (moduleName == "network") {
        return {getInstance<BearerPluginsDeployer>(moduleName)};
    }

    if (moduleName == "svg") {
        return {getInstance<SvgPluginsDeployer>(moduleName)};
    }

    if (moduleName == "sql") {
        return {getInstance<SqlPluginsDeployer>(moduleName)};
    }

    if (moduleName == "positioning") {
        return {getInstance<PositioningPluginsDeployer>(moduleName)};
    }

    if (moduleName == "multimedia") {
        return {getInstance<MultimediaPluginsDeployer>(moduleName)};
    }

    if (moduleName == "webenginecore") {
        return {getInstance<WebEnginePluginsDeployer>(moduleName)};
    }

    if (moduleName == "qml") {
        return {getInstance<QmlPluginsDeployer>(moduleName)};
    }

    if (moduleName == "3dquickrender" || moduleName == "3drender") {
        return {getInstance<Qt3DPluginsDeployer>(moduleName)};
    }

    if (moduleName == "gamepad") {
        return {getInstance<GamepadPluginsDeployer>(moduleName)};
    }

    if (moduleName == "printsupport") {
        return {getInstance<PrintSupportPluginsDeployer>(moduleName)};
    }

    if (moduleName == "texttospeech") {
        return {getInstance<TextToSpeechPluginsDeployer>(moduleName)};
    }

    // fallback
    return {getInstance<BasicPluginsDeployer>(moduleName)};
}
