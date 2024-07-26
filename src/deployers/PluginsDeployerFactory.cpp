// local headers
#include "PluginsDeployerFactory.h"
#include "BasicPluginsDeployer.h"
#include "PlatformPluginsDeployer.h"
#include "BearerPluginsDeployer.h"
#include "GamepadPluginsDeployer.h"
#include "LocationPluginsDeployer.h"
#include "Multimedia5PluginsDeployer.h"
#include "Multimedia6PluginsDeployer.h"
#include "PrintSupportPluginsDeployer.h"
#include "PositioningPluginsDeployer.h"
#include "QmlPluginsDeployer.h"
#include "Qt3DPluginsDeployer.h"
#include "SqlPluginsDeployer.h"
#include "SvgPluginsDeployer.h"
#include "TextToSpeechPluginsDeployer.h"
#include "WebEnginePluginsDeployer.h"
#include "XcbglIntegrationPluginsDeployer.h"
#include "TlsBackendsDeployer.h"
#include "WaylandcompositorPluginsDeployer.h"
#include "WaylandShellIntegrationPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::appdir;
namespace fs = std::filesystem;

PluginsDeployerFactory::PluginsDeployerFactory(AppDir& appDir,
                                               fs::path qtPluginsPath,
                                               fs::path qtLibexecsPath,
                                               fs::path qtInstallQmlPath,
                                               fs::path qtTranslationsPath,
                                               fs::path qtDataPath,
                                               int qtMajorVersion,
                                               int qtMinorVersion) : appDir(appDir),
                                                                      qtPluginsPath(std::move(qtPluginsPath)),
                                                                      qtLibexecsPath(std::move(qtLibexecsPath)),
                                                                      qtInstallQmlPath(std::move(qtInstallQmlPath)),
                                                                      qtTranslationsPath(std::move(qtTranslationsPath)),
                                                                      qtDataPath(std::move(qtDataPath)),
                                                                      qtMajorVersion(qtMajorVersion),
                                                                      qtMinorVersion(qtMinorVersion) {}

std::vector<std::shared_ptr<PluginsDeployer>> PluginsDeployerFactory::getDeployers(const std::string& moduleName) {
    if (moduleName == "gui") {
        return {getInstance<PlatformPluginsDeployer>(moduleName), getInstance<XcbglIntegrationPluginsDeployer>(moduleName)};
    }

    if (moduleName == "opengl" || moduleName == "xcbqpa") {
        return {getInstance<XcbglIntegrationPluginsDeployer>(moduleName)};
    }

    if (moduleName == "network") {
        if (qtMajorVersion < 6) {
            return {getInstance<BearerPluginsDeployer>(moduleName)};
        } else if (qtMinorVersion >= 2) {
            return {getInstance<TlsBackendsDeployer>(moduleName)};
        }
    }

    if (moduleName == "svg") {
        return {getInstance<SvgPluginsDeployer>(moduleName)};
    }

    if (moduleName == "sql") {
        return {getInstance<SqlPluginsDeployer>(moduleName)};
    }

    if (moduleName == "location") {
        return {getInstance<LocationPluginsDeployer>(moduleName)};
    }

    if (moduleName == "positioning") {
        return {getInstance<PositioningPluginsDeployer>(moduleName)};
    }

    if (moduleName == "multimedia") {
	if (qtMajorVersion < 6) {
            return {getInstance<Multimedia5PluginsDeployer>(moduleName)};
	} else {
            return {getInstance<Multimedia6PluginsDeployer>(moduleName)};
        }
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

    if (moduleName == "waylandcompositor") {
        return {getInstance<WaylandcompositorPluginsDeployer>(moduleName)};
    }

    if (moduleName == "wayland-shell-integration") {
        return {getInstance<WaylandShellIntegrationPluginsDeployer>(moduleName)};
    }

    // fallback
    return {getInstance<BasicPluginsDeployer>(moduleName)};
}
