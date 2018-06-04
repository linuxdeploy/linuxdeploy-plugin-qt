// system includes
#include <string>
#include <tuple>
#include <vector>

#pragma once

class QtModule {
    public:
        const std::string name;
        const std::string libraryFilePrefix;
        const std::string pluginGroup;

    public:
        QtModule(std::string name, std::string libraryFilePrefix, std::string pluginGroup) :
            name(std::move(name)), libraryFilePrefix(std::move(libraryFilePrefix)), pluginGroup(std::move(pluginGroup)) {}
};

static const std::vector<QtModule> QtModules = {
    {"3danimation", "libQt53DAnimation", ""},
    {"3dcore", "libQt53DCore", ""},
    {"3dextras", "libQt53DExtras", ""},
    {"3dinput", "libQt53DInput", ""},
    {"3dlogic", "libQt53DLogic", ""},
    {"3dquickanimation", "libQt53DQuickAnimation", ""},
    {"3dquickextras", "libQt53DQuickExtras", ""},
    {"3dquickinput", "libQt53DQuickInput", ""},
    {"3dquickrender", "libQt53DQuickRender", ""},
    {"3dquickscene2d", "libQt53DQuickScene2D", ""},
    {"3dquick", "libQt53DQuick", ""},
    {"bluetooth", "libQt5Bluetooth", ""},
    {"concurrent", "libQt5Concurrent", ""},
    {"core", "libQt5Core", ""},
    {"dbus", "libQt5DBus", ""},
    {"designercomponents", "libQt5DesignerComponents", ""},
    {"designer", "libQt5Designer", ""},
    {"gamepad", "libQt5Gamepad", ""},
    {"gui", "libQt5Gui", ""},
    {"help", "libQt5Help", ""},
    {"location", "libQt5Location", ""},
    {"multimediagsttools", "libQt5MultimediaGstTools", ""},
    {"multimediaquick", "libQt5MultimediaQuick", ""},
    {"multimedia", "libQt5Multimedia", ""},
    {"multimediawidgets", "libQt5MultimediaWidgets", ""},
    {"network", "libQt5Network", ""},
    {"nfc", "libQt5Nfc", ""},
    {"opengl", "libQt5OpenGL", ""},
    {"positioning", "libQt5Positioning", ""},
    {"printsupport", "libQt5PrintSupport", ""},
    {"qml", "libQt5Qml", ""},
    {"quickcontrols2", "libQt5QuickControls2", ""},
    {"quickparticles", "libQt5QuickParticles", ""},
    {"quick", "libQt5Quick", ""},
    {"quicktemplates2", "libQt5QuickTemplates2", ""},
    {"quicktest", "libQt5QuickTest", ""},
    {"quickwidgets", "libQt5QuickWidgets", ""},
    {"remoteobjects", "libQt5RemoteObjects", ""},
    {"script", "libQt5Script", ""},
    {"scripttools", "libQt5ScriptTools", ""},
    {"scxml", "libQt5Scxml", ""},
    {"sensors", "libQt5Sensors", ""},
    {"serialbus", "libQt5SerialBus", ""},
    {"serialport", "libQt5SerialPort", ""},
    {"sql", "libQt5Sql", ""},
    {"svg", "libQt5Svg", ""},
    {"test", "libQt5Test", ""},
    {"texttospeech", "libQt5TextToSpeech", ""},
    {"webchannel", "libQt5WebChannel", ""},
    {"webenginecore", "libQt5WebEngineCore", ""},
    {"webengine", "libQt5WebEngine", ""},
    {"webenginewidgets", "libQt5WebEngineWidgets", ""},
    {"websockets", "libQt5WebSockets", ""},
    {"widgets", "libQt5Widgets", ""},
    {"x11extras", "libQt5X11Extras", ""},
    {"xcbqpa", "libQt5XcbQpa", ""},
    {"xmlpatterns", "libQt5XmlPatterns", ""},
    {"xml", "libQt5Xml", ""},
};
