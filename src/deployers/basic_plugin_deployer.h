#pragma once

// system headers
#include <memory>

// local headers
#include "plugin_deployer.h"

using namespace linuxdeploy::plugin::qt;

/**
 *
 */
class BasicPluginDeployer : public PluginDeployer {
private:
    class Private;
    std::shared_ptr<Private> d;

public:
    /**
     * Default constructor. Constructs a basic deployer for a plugin with the given name.
     *
     * @param pluginName
     */
    explicit BasicPluginDeployer(const std::string& pluginName);

    /**
     * Default destroyer is good enough for this class for now, but in case we need to change this we declare a virtual
     * one.
     */
    virtual ~BasicPluginDeployer() = default;

public:
    void deploy() override;
};
