/// HEADER
#include <csapex/command/delete_node.h>

/// COMPONENT
#include <csapex/command/command_factory.h>
#include <csapex/model/node_constructor.h>
#include <csapex/model/node.h>
#include <csapex/model/node_facade_impl.h>
#include <csapex/model/node_handle.h>
#include <csapex/model/node_state.h>
#include <csapex/model/graph/graph_impl.h>
#include <csapex/factory/node_factory_impl.h>
#include <csapex/model/subgraph_node.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex/model/connection.h>
#include <csapex/core/graphio.h>
#include <csapex/command/command_serializer.h>
#include <csapex/serialization/io/std_io.h>
#include <csapex/serialization/io/csapex_io.h>
#include <csapex/model/graph_facade_impl.h>

/// SYSTEM

using namespace csapex;
using namespace csapex::command;

CSAPEX_REGISTER_COMMAND_SERIALIZER(DeleteNode)

DeleteNode::DeleteNode(const AUUID& parent_uuid, const UUID& uuid) : Meta(parent_uuid, "delete node and connections"), uuid(uuid)
{
}

std::string DeleteNode::getDescription() const
{
    return std::string("deleted node ") + uuid.getFullName();
}

bool DeleteNode::doExecute()
{
    GraphImplementationPtr graph = getGraph();
    NodeHandle* node_handle = graph->findNodeHandle(uuid);

    type = node_handle->getType();

    locked = false;
    clear();

    for (const ConnectablePtr& connectable : node_handle->getExternalConnectors()) {
        apex_assert(!connectable->isConnected());
    }

    // serialize sub graph
    if (node_handle->isGraph()) {
        GraphFacadeImplementationPtr g = root_graph_facade_->getLocalSubGraph(node_handle->getUUID());
        apex_assert_hard(g);

        GraphIO io(*g, getNodeFactory());
        saved_graph = io.saveGraph();
    }

    locked = true;

    if (Meta::doExecute()) {
        saved_state = node_handle->getNodeStateCopy();

        graph->deleteNode(node_handle->getUUID());
        return true;
    }

    return false;
}

bool DeleteNode::doUndo()
{
    GraphImplementationPtr graph = getGraph();
    NodeFacadeImplementationPtr node_facade = getNodeFactory()->makeNode(type, uuid, graph);
    node_facade->setNodeState(saved_state);

    graph->addNode(node_facade);

    // deserialize subgraph
    if (node_facade->isGraph()) {
        GraphFacadeImplementationPtr g = root_graph_facade_->getLocalSubGraph(node_facade->getUUID());
        apex_assert_hard(g);

        GraphIO io(*g, getNodeFactory());
        io.loadGraph(saved_graph);
    }

    return Meta::doUndo();
}

bool DeleteNode::doRedo()
{
    if (Meta::doRedo()) {
        GraphImplementationPtr graph = getGraph();
        NodeHandle* node_handle = graph->findNodeHandle(uuid);
        saved_state = node_handle->getNodeStateCopy();

        graph->deleteNode(node_handle->getUUID());
        return true;
    }

    return false;
}

void DeleteNode::serialize(SerializationBuffer& data, SemanticVersion& version) const
{
    Meta::serialize(data, version);

    data << uuid;
    data << type;
}

void DeleteNode::deserialize(const SerializationBuffer& data, const SemanticVersion& version)
{
    Meta::deserialize(data, version);

    data >> uuid;
    data >> type;
}
