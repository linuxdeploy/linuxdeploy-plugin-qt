// system includes
#include <string>
#include <tuple>
#include <vector>

#pragma once

class QtModule {
    public:
        const std::string name;
        const std::string libraryFilePrefix;
        const std::string translationFilePrefix;

    public:
        QtModule(std::string name, std::string libraryFilePrefix, std::string pluginGroup) :
            name(std::move(name)), libraryFilePrefix(std::move(libraryFilePrefix)), translationFilePrefix(std::move(pluginGroup)) {}
};

// TODO: the list of translation file prefixes is probably incomplete
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
    {"clucene", "libQt5CLucene", "qt_help"},
    {"concurrent", "libQt5Concurrent", "qtbase"},
    {"core", "libQt5Core", "qtbase"},
    {"dbus", "libQt5DBus", ""},
    {"declarative", "libQt5Declarative", "qtquick1"},
    {"designercomponents", "libQt5DesignerComponents", ""},
    {"designer", "libQt5Designer", ""},
    {"gamepad", "libQt5Gamepad", ""},
    {"gui", "libQt5Gui", "qtbase"},
    {"help", "libQt5Help", "qt_help"},
    {"location", "libQt5Location", ""},
    {"multimediagsttools", "libQt5MultimediaGstTools", "qtmultimedia"},
    {"multimediaquick", "libQt5MultimediaQuick", "qtmultimedia"},
    {"multimedia", "libQt5Multimedia", "qtmultimedia"},
    {"multimediawidgets", "libQt5MultimediaWidgets", "qtmultimedia"},
    {"network", "libQt5Network", "qtbase"},
    {"nfc", "libQt5Nfc", ""},
    {"opengl", "libQt5OpenGL", ""},
    {"positioning", "libQt5Positioning", ""},
    {"printsupport", "libQt5PrintSupport", ""},
    {"qml", "libQt5Qml", "qtdeclarative"},
    {"quickcontrols2", "libQt5QuickControls2", ""},
    {"quickparticles", "libQt5QuickParticles", ""},
    {"quick", "libQt5Quick", "qtdeclarative"},
    {"quicktemplates2", "libQt5QuickTemplates2", ""},
    {"quicktest", "libQt5QuickTest", ""},
    {"quickwidgets", "libQt5QuickWidgets", ""},
    {"remoteobjects", "libQt5RemoteObjects", ""},
    {"script", "libQt5Script", "qtscript"},
    {"scripttools", "libQt5ScriptTools", "qtscript"},
    {"scxml", "libQt5Scxml", ""},
    {"sensors", "libQt5Sensors", ""},
    {"serialbus", "libQt5SerialBus", ""},
    {"serialport", "libQt5SerialPort", "qtserialport"},
    {"sql", "libQt5Sql", "qtbase"},
    {"svg", "libQt5Svg", ""},
    {"test", "libQt5Test", "qtbase"},
    {"texttospeech", "libQt5TextToSpeech", ""},
    {"webchannel", "libQt5WebChannel", ""},
    {"webenginecore", "libQt5WebEngineCore", ""},
    {"webengine", "libQt5WebEngine", "qtwebengine"},
    {"webenginewidgets", "libQt5WebEngineWidgets", ""},
    {"websockets", "libQt5WebSockets", "qtwebsockets"},
    {"widgets", "libQt5Widgets", "qtbase"},
    {"x11extras", "libQt5X11Extras", ""},
    {"xcbqpa", "libQt5XcbQpa", ""},
    {"xmlpatterns", "libQt5XmlPatterns", "qtxmlpatterns"},
    {"xml", "libQt5Xml", "qtbase"},
};
