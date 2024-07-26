// system includes
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>

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
static const std::vector<QtModule> Qt5Modules = {
    {"3danimation", "libQt53DAnimation", ""},
    {"3dcore", "libQt53DCore", ""},
    {"3dextras", "libQt53DExtras", ""},
    {"3dinput", "libQt53DInput", ""},
    {"3dlogic", "libQt53DLogic", ""},
    {"3drender", "libQt53DRender", ""},
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
    {"waylandclient", "libQt5WaylandClient", ""},
    {"waylandcompositor", "libQt5WaylandCompositor", ""},
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

static const std::vector<QtModule> Qt6Modules = {
    {"concurrent", "libQt6Concurrent", "qtbase"},
    {"core5compat", "libQt6Core5Compat", "qtbase"},
    {"core", "libQt6Core", "qtbase"},
    {"dbus", "libQt6DBus", ""},
    {"designercomponents", "libQt6DesignerComponents", ""},
    {"designer", "libQt6Designer", ""},
    {"eglfsdeviceintegration", "libQt6EglFSDeviceIntegration", ""},
    {"eglfskmssupport", "libQt6EglFsKmsSupport", ""},
    {"gui", "libQt6Gui", "qtbase"},
    {"help", "libQt6Help", "qt_help"},
    {"network", "libQt6Network", "qtbase"},
    {"opengl", "libQt6OpenGL", ""},
    {"openglwidgets", "libQt6OpenGLWidgets", ""},
    {"printsupport", "libQt6PrintSupport", ""},
    {"qmlmodels", "libQt6QmlModels", ""},
    {"qml", "libQt6Qml", "qtdeclarative"},
    {"qmlworkerscript", "libQt6QmlWorkerScript", ""},
    {"quick3dassetimport", "libQt6Quick3DAssetImport", ""},
    {"quick3druntimerender", "libQt6Quick3DRuntimeRender", ""},
    {"quick3d", "libQt6Quick3D", ""},
    {"quick3dutils", "libQt6Quick3DUtils", ""},
    {"quickcontrols2impl", "libQt6QuickControls2Impl", ""},
    {"quickcontrols2", "libQt6QuickControls2", ""},
    {"quickparticles", "libQt6QuickParticles", ""},
    {"quickshapes", "libQt6QuickShapes", ""},
    {"quick", "libQt6Quick", "qtdeclarative"},
    {"quicktemplates2", "libQt6QuickTemplates2", ""},
    {"quicktest", "libQt6QuickTest", ""},
    {"quickwidgets", "libQt6QuickWidgets", ""},
    {"shadertools", "libQt6ShaderTools", ""},
    {"sql", "libQt6Sql", "qtbase"},
    {"svg", "libQt6Svg", ""},
    {"svgwidgets", "libQt6SvgWidgets", ""},
    {"test", "libQt6Test", "qtbase"},
    {"uitools", "libQt6UiTools", ""},
    {"waylandclient", "libQt6WaylandClient", ""},
    {"wayland-shell-integration", "libQt6WlShellIntegration", ""},
    {"wayland-graphics-integration-client", "", ""},
    {"waylandcompositor", "libQt6WaylandCompositor", ""},
    {"webenginecore", "libQt6WebEngineCore", ""},
    {"webengine", "libQt6WebEngine", "qtwebengine"},
    {"webenginewidgets", "libQt6WebEngineWidgets", ""},
    {"widgets", "libQt6Widgets", "qtbase"},
    {"xcbqpa", "libQt6XcbQpa", ""},
    {"xml", "libQt6Xml", "qtbase"},

    /* Re-introduce for testing in 6.2+ */

    {"3danimation", "libQt63DAnimation", ""},
    {"3dcore", "libQt63DCore", ""},
    {"3dextras", "libQt63DExtras", ""},
    {"3dinput", "libQt63DInput", ""},
    {"3dlogic", "libQt63DLogic", ""},
    {"3drender", "libQt63DRender", ""},
    {"3dquickanimation", "libQt63DQuickAnimation", ""},
    {"3dquickextras", "libQt63DQuickExtras", ""},
    {"3dquickinput", "libQt63DQuickInput", ""},
    {"3dquickrender", "libQt63DQuickRender", ""},
    {"3dquickscene2d", "libQt63DQuickScene2D", ""},
    {"3dquick", "libQt63DQuick", ""},
    {"bluetooth", "libQt6Bluetooth", ""},
    {"clucene", "libQt6CLucene", "qt_help"},
    {"declarative", "libQt6Declarative", "qtquick2"},
    {"gamepad", "libQt6Gamepad", ""},
    {"location", "libQt6Location", ""},
    {"multimediagsttools", "libQt6MultimediaGstTools", "qtmultimedia"},
    {"multimediaquick", "libQt6MultimediaQuick", "qtmultimedia"},
    {"multimedia", "libQt6Multimedia", "qtmultimedia"},
    {"multimediawidgets", "libQt6MultimediaWidgets", "qtmultimedia"},
    {"nfc", "libQt6Nfc", ""},
    {"positioning", "libQt6Positioning", ""},
    {"remoteobjects", "libQt6RemoteObjects", ""},
    {"script", "libQt6Script", "qtscript"},
    {"scripttools", "libQt6ScriptTools", "qtscript"},
    {"scxml", "libQt6Scxml", ""},
    {"sensors", "libQt6Sensors", ""},
    {"serialbus", "libQt6SerialBus", ""},
    {"serialport", "libQt6SerialPort", "qtserialport"},
    {"texttospeech", "libQt6TextToSpeech", ""},
    {"webchannel", "libQt6WebChannel", ""},
    {"websockets", "libQt6WebSockets", "qtwebsockets"},
    {"x11extras", "libQt6X11Extras", ""},
    {"xmlpatterns", "libQt6XmlPatterns", "qtxmlpatterns"},
   
};

inline const std::vector<QtModule>& getQtModules(const int version) {
    if (version == 5) {
        return Qt5Modules;
    }
    else if (version == 6) {
        return Qt6Modules;
    }
    throw std::runtime_error("Unknown Qt version: " + std::to_string(version));
}
