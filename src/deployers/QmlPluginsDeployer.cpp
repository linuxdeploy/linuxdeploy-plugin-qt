// library headers
#include <qml.h>

// local headers
#include "QmlPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

namespace bf = boost::filesystem;

bool QmlPluginsDeployer::deploy() {
    if (!BasicPluginsDeployer::deploy())
        return false;

    try {
        deployQml(appDir, qtInstallQmlPath);
    } catch (const QmlImportScannerError &) {
        return false;
    }

    return true;
}
