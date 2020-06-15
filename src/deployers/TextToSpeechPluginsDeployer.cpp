// library headers
#include <linuxdeploy/core/log.h>
#include <boost/filesystem.hpp>

// local headers
#include "TextToSpeechPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;
using namespace linuxdeploy::core::log;

namespace bf = boost::filesystem;

bool TextToSpeechPluginsDeployer::deploy() {
    // calling the default code is optional, but it won't hurt for now
    if (!BasicPluginsDeployer::deploy())
        return false;

    const std::string pluginsName = "texttospeech";

    ldLog() << "Deploying" << pluginsName << "plugins" << std::endl;

    for (bf::directory_iterator i(qtPluginsPath / pluginsName); i != bf::directory_iterator(); ++i) {
        if (i->path().extension() == ".debug") {
            ldLog() << LD_DEBUG << "skipping .debug file:" << i->path() << std::endl;
            continue;
        }

        if (!appDir.deployLibrary(*i, appDir.path() / "usr/plugins/" / pluginsName))
            return false;
    }

    return true;
}
