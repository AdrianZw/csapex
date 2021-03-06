#ifndef BOOTSTRAP_PLUGIN_H
#define BOOTSTRAP_PLUGIN_H

/// PROJECT
#include <csapex/plugin/plugin_fwd.h>
#include <csapex_core/csapex_core_export.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/utility/export_plugin.h>

#define CSAPEX_REGISTER_BOOT(name) CLASS_LOADER_REGISTER_CLASS_WITH_MESSAGE(name, BootstrapPlugin, "INSTALLING BOOTSTRAP PLUGIN")

namespace csapex
{
class CSAPEX_CORE_EXPORT BootstrapPlugin
{
public:
    virtual ~BootstrapPlugin();

    virtual void boot(csapex::PluginLocator* locator) = 0;
};
}  // namespace csapex

#endif  // BOOTSTRAP_PLUGIN_H
