#ifndef CSAPEX_CORE_H
#define CSAPEX_CORE_H

/// COMPONENT
#include <csapex/csapex_fwd.h>
#include <csapex/command/dispatcher.h>
#include <csapex/core/settings.h>
#include <csapex/command/meta.h>
#include <csapex/utility/uuid.h>

/// SYSTEM
#include <QObject>
#include <yaml-cpp/yaml.h>

namespace class_loader {
class ClassLoader;
}

namespace csapex
{

class CsApexCore : public QObject
{
    Q_OBJECT

public:
    struct Listener {
        virtual void resetSignal() = 0;
    };

public:
    CsApexCore(Settings& settings_,
               PluginLocatorPtr plugin_locator,
               GraphWorkerPtr graph,
               NodeFactory* node_factory, NodeAdapterFactory* node_adapter_factory, CommandDispatcher *cmd_dispatcher);
    virtual ~CsApexCore();

    void init(DragIO *dragio);
    void boot();
    void startup();

    void load(const std::string& file);
    void saveAs(const std::string& file);

    void reset();

    void unloadNode(csapex::UUID uuid);
    void reloadDone();

    void addListener(Listener* l);
    void removeListener(Listener* l);

    Settings& getSettings() const;
    NodeFactory& getNodeFactory() const;

    bool isPaused() const;

public Q_SLOTS:
    void setPause(bool pause);
    void settingsChanged();
    void setStatusMessage(const std::string& msg);

Q_SIGNALS:
    void configChanged();
    void showStatusMessage(const std::string& msg);
    void reloadBoxMenues();

    void resetRequest();

    void saveSettingsRequest(YAML::Node& e);
    void loadSettingsRequest(YAML::Node& n);

    void saveViewRequest(YAML::Node& e);
    void loadViewRequest(YAML::Node& n);

    void paused(bool);

private:
    CorePluginPtr makeCorePlugin(const std::string& name);
    void unloadCorePlugin(const std::string& plugin);
    void reloadCorePlugin(const std::string& plugin);

private:
    Settings& settings_;
    DragIO* drag_io_;

    csapex::PluginLocatorPtr plugin_locator_;

    GraphWorkerPtr graph_worker_;
    NodeFactory* node_factory_;
    NodeAdapterFactory* node_adapter_factory_;

    bool destruct;
    CommandDispatcher* cmd_dispatch;

    PluginManager<CorePlugin>* core_plugin_manager;
    std::map<std::string, boost::shared_ptr<CorePlugin> > core_plugins_;
    std::map<std::string, bool> core_plugins_connected_;

    std::vector<Listener*> listener_;

    std::vector<BootstrapPluginPtr> boot_plugins_;
    std::vector<class_loader::ClassLoader*> boot_plugin_loaders_;

    command::Meta::Ptr unload_commands_;

    bool init_;
};

}

#endif // CSAPEX_CORE_H
