#include "NetworkInformationPluginsDeployer.h"

namespace linuxdeploy {
namespace plugin {
namespace qt {
bool NetworkInformationPluginsDeployer::doDeploy() {
  return deployStandardQtPlugins({"networkinformation"});
}
} // namespace qt
} // namespace plugin
} // namespace linuxdeploy
