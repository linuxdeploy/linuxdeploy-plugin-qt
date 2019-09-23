cmake_minimum_required(VERSION 3.1)

set(CLASSES
    PluginsDeployer
    PluginsDeployerFactory
    basic_plugin_deployer
    platform_plugins_deployer
)

add_library(deployers OBJECT)

target_link_libraries(deployers PUBLIC linuxdeploy_core)

foreach(class ${CLASSES})
    target_sources(deployers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${class}.h)
    target_sources(deployers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${class}.cpp)
endforeach()
