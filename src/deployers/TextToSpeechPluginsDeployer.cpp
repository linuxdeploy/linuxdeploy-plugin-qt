// local headers
#include "TextToSpeechPluginsDeployer.h"

using namespace linuxdeploy::plugin::qt;

bool TextToSpeechPluginsDeployer::doDeploy() {
    return deployStandardQtPlugins({"texttospeech"});
}
