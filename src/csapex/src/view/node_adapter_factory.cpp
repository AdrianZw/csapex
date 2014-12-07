/// HEADER
#include <csapex/view/node_adapter_factory.h>

/// COMPONENT
#include <csapex/model/node_worker.h>
#include <csapex/view/node_adapter.h>
#include <csapex/view/default_node_adapter.h>
#include <csapex/utility/plugin_manager.hpp>

/// SYSTEM
#include <qglobal.h>

using namespace csapex;

NodeAdapterFactory::NodeAdapterFactory(Settings &settings, PluginLocator *locator)
    : settings_(settings), plugin_locator_(locator),
    node_adapter_manager_(new PluginManager<NodeAdapterBuilder> ("csapex::NodeAdapterBuilder"))
{

}


NodeAdapterFactory::~NodeAdapterFactory()
{
    node_adapter_builders_.clear();

    delete node_adapter_manager_;
    node_adapter_manager_ = NULL;
}

NodeAdapter::Ptr NodeAdapterFactory::makeNodeAdapter(NodeWorker* node, WidgetController* widget_controller)
{
    std::string type = node->getType();
    if(node_adapter_builders_.find(type) != node_adapter_builders_.end()) {
        return node_adapter_builders_[type]->build(node, widget_controller);
    } else {
        return NodeAdapter::Ptr(new DefaultNodeAdapter(node, widget_controller));
    }
}

void NodeAdapterFactory::loadPlugins()
{
    ensureLoaded();

    rebuildPrototypes();
}

void NodeAdapterFactory::rebuildPrototypes()
{
    typedef std::pair<std::string, DefaultConstructor<NodeAdapterBuilder> > ADAPTER_PAIR;
    Q_FOREACH(const ADAPTER_PAIR& p, node_adapter_manager_->availableClasses()) {
        NodeAdapterBuilder::Ptr builder = p.second.construct();
        node_adapter_builders_[builder->getWrappedType()] = builder;
    }
}

void NodeAdapterFactory::ensureLoaded()
{
    if(!node_adapter_manager_->pluginsLoaded()) {
        node_adapter_manager_->load(plugin_locator_);

        rebuildPrototypes();
    }
}
