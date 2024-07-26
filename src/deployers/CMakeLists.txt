cmake_minimum_required(VERSION 3.7)

set(CLASSES
    PluginsDeployer
    PluginsDeployerFactory
    BasicPluginsDeployer

    PlatformPluginsDeployer
    XcbglIntegrationPluginsDeployer
    BearerPluginsDeployer
    SvgPluginsDeployer
    SqlPluginsDeployer
    PositioningPluginsDeployer
    LocationPluginsDeployer
    Multimedia5PluginsDeployer
    Multimedia6PluginsDeployer
    WebEnginePluginsDeployer
    QmlPluginsDeployer
    Qt3DPluginsDeployer
    GamepadPluginsDeployer
    PrintSupportPluginsDeployer
    TextToSpeechPluginsDeployer
    TlsBackendsDeployer
    WaylandcompositorPluginsDeployer
    WaylandShellIntegrationPluginsDeployer
)

# TODO: CMake <= 3.7 (at least!) doesn't allow for using OBJECT libraries with target_link_libraries
add_library(deployers OBJECT)

target_link_libraries(deployers PUBLIC linuxdeploy_core linuxdeploy-plugin-qt_util)

foreach(class ${CLASSES})
    target_sources(deployers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${class}.h)
    target_sources(deployers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${class}.cpp)
endforeach()
