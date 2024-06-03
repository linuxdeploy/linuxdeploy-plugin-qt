// system headers
#include <filesystem>
#include <set> // Include set for handling exclusions

// library headers
#include <linuxdeploy/core/log.h>

// local headers
#include "SqlPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace fs = std::filesystem;

bool SqlPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    ldLog() << "Deploying SQL plugins" << std::endl;

    // Retrieve the list of plugins to exclude from the environment
    std::set<std::string> excludedPlugins;
    if (const char* env_p = std::getenv("LINUXDEPLOY_EXCLUDE_SQL_PLUGINS")) {
        std::string excluded(env_p);
        std::istringstream iss(excluded);
        std::string plugin;
        while (std::getline(iss, plugin, ';')) {
            excludedPlugins.insert(plugin);
        }
    }

    for (fs::directory_iterator i(qtPluginsPath / "sqldrivers"); i != fs::directory_iterator(); ++i) {
        // Check if the current plugin is in the list of excluded plugins
        if (excludedPlugins.find(i->path().filename().string()) != excludedPlugins.end()) {
            ldLog() << "Excluding SQL plugin:" << i->path().filename() << std::endl;
            continue; // Skip deploying this plugin
        }

        // Attempt to deploy the plugin, emitting a warning instead of failing on missing dependencies
        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/sqldrivers/", true)) {
            ldLog() << LD_WARNING << "Could not deploy SQL plugin due to missing dependencies:" << i->path().filename() << std::endl;
            continue; // Proceed with the next plugin instead of returning false
        }
    }

    return true;
}
