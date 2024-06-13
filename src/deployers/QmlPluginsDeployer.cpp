// library headers
#include <qml.h>

// local headers
#include "QmlPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

namespace fs = std::filesystem;

bool QmlPluginsDeployer::customDeploy() {
    try {
        deployQml(appDir, qtInstallQmlPath);
    } catch (const QmlImportScannerError &) {
        return false;
    }

    return true;
}
