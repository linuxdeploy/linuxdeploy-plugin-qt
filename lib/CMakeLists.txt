add_subdirectory(linuxdeploy EXCLUDE_FROM_ALL)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

add_library(json INTERFACE)
set_property(TARGET json PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})