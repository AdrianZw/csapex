/// HEADER
#include <csapex/model/graph_facade_impl.h>

/// PROJECT
#include <csapex/model/node_facade_impl.h>
#include <csapex/model/graph_facade_impl.h>
#include <csapex/model/graph/graph_impl.h>
#include <csapex/model/graph/vertex.h>
#include <csapex/model/subgraph_node.h>
#include <csapex/model/node_state.h>
#include <csapex/model/node_runner.h>
#include <csapex/model/node_handle.h>
#include <csapex/scheduling/thread_pool.h>
#include <csapex/msg/direct_connection.h>
#include <csapex/model/connectable.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex/signal/event.h>
#include <csapex/signal/slot.h>

using namespace csapex;

GraphFacadeImplementation::GraphFacadeImplementation(ThreadPool& executor, GraphImplementationPtr graph, SubgraphNodePtr graph_node, NodeFacadeImplementationPtr nh, GraphFacadeImplementation* parent)
  : absolute_uuid_(graph_node->getUUID()), parent_(parent), graph_handle_(nh), executor_(executor), graph_(graph), graph_node_(graph_node)
{
    observe(graph->vertex_added, this, &GraphFacadeImplementation::nodeAddedHandler);
    observe(graph->vertex_removed, this, &GraphFacadeImplementation::nodeRemovedHandler);
    observe(graph->notification, notification);

    observe(graph->connection_added, [this](const ConnectionDescription& ci) { connection_added(ci); });
    observe(graph->connection_removed, [this](const ConnectionDescription& ci) { connection_removed(ci); });
    observe(graph->state_changed, state_changed);

    observe(graph_node_->forwarding_connector_added, forwarding_connector_added);
    observe(graph_node_->forwarding_connector_removed, forwarding_connector_removed);

    if (parent_) {
        // TODO: refactor!
        apex_assert_hard(graph_handle_);

        AUUID parent_auuid = parent_->getAbsoluteUUID();
        if (!parent_auuid.empty()) {
            absolute_uuid_ = AUUID(UUIDProvider::makeDerivedUUID_forced(parent_auuid, absolute_uuid_.getFullName()));
        }
    }
}

NodeFacadePtr GraphFacadeImplementation::getNodeFacade() const
{
    return graph_handle_;
}

AUUID GraphFacadeImplementation::getAbsoluteUUID() const
{
    return absolute_uuid_;
}

UUID GraphFacadeImplementation::generateUUID(const std::string& prefix)
{
    return graph_->generateUUID(prefix);
}

GraphFacade* GraphFacadeImplementation::getParent() const
{
    return parent_;
}

GraphFacadePtr GraphFacadeImplementation::getSubGraph(const UUID& uuid)
{
    if (uuid.empty()) {
        throw std::logic_error("cannot get subgraph for empty UUID");
    }

    if (uuid.composite()) {
        GraphFacadePtr facade = children_[uuid.rootUUID()];
        return facade->getSubGraph(uuid.nestedUUID());
    } else {
        GraphFacadePtr facade = children_[uuid];
        return facade;
    }
}

GraphFacadeImplementationPtr GraphFacadeImplementation::getLocalSubGraph(const UUID& uuid)
{
    if (uuid.empty()) {
        throw std::logic_error("cannot get subgraph for empty UUID");
    }

    if (uuid.composite()) {
        GraphFacadeImplementationPtr facade = children_.at(uuid.rootUUID());
        return facade->getLocalSubGraph(uuid.nestedUUID());
    } else {
        return children_.at(uuid);
    }
}

SubgraphNodePtr GraphFacadeImplementation::getSubgraphNode()
{
    return std::dynamic_pointer_cast<SubgraphNode>(graph_node_);
}

NodeFacadePtr GraphFacadeImplementation::findNodeFacade(const UUID& uuid) const
{
    return graph_->findNodeFacade(uuid);
}
NodeFacadePtr GraphFacadeImplementation::findNodeFacadeNoThrow(const UUID& uuid) const noexcept
{
    return graph_->findNodeFacadeNoThrow(uuid);
}
NodeFacadePtr GraphFacadeImplementation::findNodeFacadeForConnector(const UUID& uuid) const
{
    return graph_->findNodeFacadeForConnector(uuid);
}
NodeFacadePtr GraphFacadeImplementation::findNodeFacadeForConnectorNoThrow(const UUID& uuid) const noexcept
{
    return graph_->findNodeFacadeForConnectorNoThrow(uuid);
}
NodeFacadePtr GraphFacadeImplementation::findNodeFacadeWithLabel(const std::string& label) const
{
    return graph_->findNodeFacadeWithLabel(label);
}

ConnectorPtr GraphFacadeImplementation::findConnector(const UUID& uuid)
{
    return graph_->findConnector(uuid);
}
ConnectorPtr GraphFacadeImplementation::findConnectorNoThrow(const UUID& uuid) noexcept
{
    return graph_->findConnectorNoThrow(uuid);
}

bool GraphFacadeImplementation::isConnected(const UUID& from, const UUID& to) const
{
    return graph_->getConnection(from, to) != nullptr;
}

ConnectionDescription GraphFacadeImplementation::getConnection(const UUID& from, const UUID& to) const
{
    ConnectionPtr c = graph_->getConnection(from, to);
    if (!c) {
        throw std::runtime_error("unknown connection requested");
    }

    return c->getDescription();
}

ConnectionDescription GraphFacadeImplementation::getConnectionWithId(int id) const
{
    ConnectionPtr c = graph_->getConnectionWithId(id);
    if (!c) {
        throw std::runtime_error("Cannot get connection with id " + std::to_string(id));
    }
    return c->getDescription();
}

std::size_t GraphFacadeImplementation::countNodes() const
{
    return graph_->countNodes();
}

int GraphFacadeImplementation::getComponent(const UUID& node_uuid) const
{
    return graph_->getComponent(node_uuid);
}
int GraphFacadeImplementation::getDepth(const UUID& node_uuid) const
{
    return graph_->getDepth(node_uuid);
}

GraphImplementationPtr GraphFacadeImplementation::getLocalGraph() const
{
    return graph_;
}

GraphFacadeImplementation* GraphFacadeImplementation::getLocalParent() const
{
    return parent_;
}

NodeFacadeImplementationPtr GraphFacadeImplementation::getLocalNodeFacade() const
{
    return graph_handle_;
}

ThreadPool* GraphFacadeImplementation::getThreadPool()
{
    return &executor_;
}

TaskGenerator* GraphFacadeImplementation::getTaskGenerator(const UUID& uuid)
{
    return generators_.at(uuid).get();
}

void GraphFacadeImplementation::addNode(NodeFacadeImplementationPtr nh)
{
    graph_->addNode(nh);
}

void GraphFacadeImplementation::clear()
{
    stop();
    graph_->clear();

    generators_.clear();
}

void GraphFacadeImplementation::stop()
{
    for (NodeHandle* nh : graph_->getAllNodeHandles()) {
        nh->stop();
    }

    executor_.stop();

    stopped();
}

ConnectionPtr GraphFacadeImplementation::connect(OutputPtr output, InputPtr input)
{
    auto c = DirectConnection::connect(output, input);
    graph_->addConnection(c);
    return c;
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandlePtr output, int output_id, NodeHandlePtr input, int input_id)
{
    return connect(output.get(), output_id, input.get(), input_id);
}

ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeHandlePtr input, int input_id)
{
    OutputPtr o = getOutput(output_id);
    InputPtr i = getInput(getInputUUID(input.get(), input_id));
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandlePtr output, int output_id, const UUID& input_id)
{
    OutputPtr o = getOutput(getOutputUUID(output.get(), output_id));
    InputPtr i = getInput(input_id);
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeHandlePtr input, const std::string& input_id)
{
    return connect(output_id, input.get(), input_id);
}

ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeHandle* input, const std::string& input_id)
{
    OutputPtr o = getOutput(output_id);
    InputPtr i = getInput(getInputUUID(input, input_id));
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandlePtr output, const std::string& output_id, const UUID& input_id)
{
    return connect(output.get(), output_id, input_id);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandle* output, const std::string& output_id, const UUID& input_id)
{
    OutputPtr o = getOutput(getOutputUUID(output, output_id));
    InputPtr i = getInput(input_id);
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandle* output, int output_id, NodeHandle* input, int input_id)
{
    OutputPtr o = getOutput(getOutputUUID(output, output_id));
    InputPtr i = getInput(getInputUUID(input, input_id));
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandlePtr output, const std::string& output_id, NodeHandlePtr input, const std::string& input_id)
{
    return connect(output.get(), output_id, input.get(), input_id);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeHandle* output, const std::string& output_name, NodeHandle* input, const std::string& input_name)
{
    OutputPtr o = getOutput(getOutputUUID(output, output_name));
    InputPtr i = getInput(getInputUUID(input, input_name));
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, const UUID& input_id)
{
    OutputPtr o = getOutput(output_id);
    InputPtr i = getInput(input_id);
    return connect(o, i);
}

ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeFacade* input, const std::string& input_name)
{
    UUID i = getInputUUID(input, input_name);
    return connect(output_id, i);
}
ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeFacadePtr input, const std::string& input_name)
{
    return connect(output_id, input.get(), input_name);
}
ConnectionPtr GraphFacadeImplementation::connect(const UUID& output_id, NodeFacadePtr input, int input_id)
{
    UUID i = getInputUUID(input.get(), input_id);
    return connect(output_id, i);
}

ConnectionPtr GraphFacadeImplementation::connect(NodeFacade* output, const std::string& output_name, NodeFacade* input, const std::string& input_name)
{
    UUID o = getOutputUUID(output, output_name);
    UUID i = getInputUUID(input, input_name);
    return connect(o, i);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacadePtr output, const std::string& output_name, NodeFacadePtr input, const std::string& input_name)
{
    return connect(output.get(), output_name, input.get(), input_name);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacade* output, const std::string& output_name, const UUID& input_id)
{
    UUID o = getOutputUUID(output, output_name);
    return connect(o, input_id);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacadePtr output, const std::string& output_name, const UUID& input_id)
{
    return connect(output.get(), output_name, input_id);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacadePtr output, int output_id, const UUID& input_id)
{
    UUID o = getOutputUUID(output.get(), output_id);
    return connect(o, input_id);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacade* output, int output_id, NodeFacade* input, int input_id)
{
    UUID o = getOutputUUID(output, output_id);
    UUID i = getInputUUID(input, input_id);
    return connect(o, i);
}
ConnectionPtr GraphFacadeImplementation::connect(NodeFacadePtr output, int output_id, NodeFacadePtr input, int input_id)
{
    UUID o = getOutputUUID(output.get(), output_id);
    UUID i = getInputUUID(input.get(), input_id);
    return connect(o, i);
}

OutputPtr GraphFacadeImplementation::getOutput(const UUID& uuid)
{
    OutputPtr o = std::dynamic_pointer_cast<Output>(getConnectable(uuid));
    apex_assert_hard(o);
    return o;
}

InputPtr GraphFacadeImplementation::getInput(const UUID& uuid)
{
    InputPtr i = std::dynamic_pointer_cast<Input>(getConnectable(uuid));
    apex_assert_hard(i);
    return i;
}

ConnectablePtr GraphFacadeImplementation::getConnectable(const UUID& uuid)
{
    NodeHandle* node = graph_->findNodeHandleForConnector(uuid);
    apex_assert_hard(node);
    return node->getConnector(uuid);
}

UUID GraphFacadeImplementation::getOutputUUID(NodeFacade* node, const std::string& label)
{
    for (const ConnectorDescription& out : node->getExternalOutputs()) {
        if (out.label == label) {
            return out.id;
        }
    }
    for (const ConnectorDescription& event : node->getEvents()) {
        if (event.label == label) {
            return event.id;
        }
    }
    throw std::logic_error(node->getUUID().getFullName() + " does not have an output with the label " + label);
}

UUID GraphFacadeImplementation::getInputUUID(NodeFacade* node, const std::string& label)
{
    for (const ConnectorDescription& in : node->getExternalInputs()) {
        if (in.label == label) {
            return in.id;
        }
    }
    for (const ConnectorDescription& slot : node->getSlots()) {
        if (slot.label == label) {
            return slot.id;
        }
    }
    throw std::logic_error(node->getUUID().getFullName() + " does not have an input with the label " + label);
}

UUID GraphFacadeImplementation::getOutputUUID(NodeHandle* node, const std::string& label)
{
    for (const OutputPtr& out : node->getExternalOutputs()) {
        if (out->getLabel() == label) {
            return out->getUUID();
        }
    }
    for (const EventPtr& event : node->getEvents()) {
        if (event->getLabel() == label) {
            return event->getUUID();
        }
    }
    throw std::logic_error(node->getUUID().getFullName() + " does not have an output with the label " + label);
}

UUID GraphFacadeImplementation::getInputUUID(NodeHandle* node, const std::string& label)
{
    for (const InputPtr& in : node->getExternalInputs()) {
        if (in->getLabel() == label) {
            return in->getUUID();
        }
    }
    for (const SlotPtr& slot : node->getSlots()) {
        if (slot->getLabel() == label) {
            return slot->getUUID();
        }
    }
    throw std::logic_error(node->getUUID().getFullName() + " does not have an input with the label " + label);
}

template <class Container>
UUID GraphFacadeImplementation::getOutputUUID(Container* node, int id)
{
    return graph_->makeTypedUUID_forced(node->getUUID(), "out", id);
}
template <class Container>
UUID GraphFacadeImplementation::getInputUUID(Container* node, int id)
{
    return graph_->makeTypedUUID_forced(node->getUUID(), "in", id);
}

void GraphFacadeImplementation::nodeAddedHandler(graph::VertexPtr vertex)
{
    NodeFacadeImplementationPtr facade = std::dynamic_pointer_cast<NodeFacadeImplementation>(vertex->getNodeFacade());
    apex_assert_hard(facade);
    if (facade->isGraph()) {
        createSubgraphFacade(facade);
    }

    node_facades_[facade->getUUID()] = facade;

    if (!facade->getNodeHandle()->isIsolated()) {
        NodeRunnerPtr runner = facade->getNodeRunner();
        apex_assert_hard(runner);
        generators_[facade->getUUID()] = runner;

        int thread_id = facade->getNodeState()->getThreadId();
        if (thread_id >= 0) {
            executor_.addToGroup(runner.get(), thread_id);
        } else {
            executor_.add(runner.get());
        }

        facade->getNode()->finishSetup();
    }

    vertex->getNodeFacade()->notification.connect(notification);

    node_facade_added(facade);
}

void GraphFacadeImplementation::nodeRemovedHandler(graph::VertexPtr vertex)
{
    NodeFacadeImplementationPtr facade = std::dynamic_pointer_cast<NodeFacadeImplementation>(vertex->getNodeFacade());

    auto pos = generators_.find(facade->getUUID());
    if (pos != generators_.end()) {
        TaskGeneratorPtr runner = pos->second;
        generators_.erase(facade->getUUID());
        executor_.remove(runner.get());
    }

    NodeFacadePtr facade_ptr = node_facades_[facade->getUUID()];
    node_facade_removed(facade_ptr);
    node_facades_.erase(facade_ptr->getUUID());

    if (facade->isGraph()) {
        auto pos = children_.find(facade->getUUID());
        apex_assert_hard(pos != children_.end());
        child_removed(pos->second);
        children_.erase(pos);
    }
}

void GraphFacadeImplementation::createSubgraphFacade(NodeFacadePtr nf)
{
    NodeFacadeImplementationPtr local_facade = std::dynamic_pointer_cast<NodeFacadeImplementation>(nf);
    apex_assert_hard(local_facade);

    NodePtr node = local_facade->getNode();
    apex_assert_hard(node);
    SubgraphNodePtr sub_graph = std::dynamic_pointer_cast<SubgraphNode>(node);
    apex_assert_hard(sub_graph);

    NodeHandle* subnh = graph_->findNodeHandle(local_facade->getUUID());
    apex_assert_hard(subnh == local_facade->getNodeHandle().get());

    GraphImplementationPtr graph_local = sub_graph->getLocalGraph();

    GraphFacadeImplementationPtr sub_graph_facade = std::make_shared<GraphFacadeImplementation>(executor_, graph_local, sub_graph, local_facade, this);
    children_[local_facade->getUUID()] = sub_graph_facade;

    observe(sub_graph_facade->notification, notification);
    observe(sub_graph_facade->node_facade_added, child_node_facade_added);
    observe(sub_graph_facade->node_facade_removed, child_node_facade_removed);
    observe(sub_graph_facade->child_node_facade_added, child_node_facade_added);
    observe(sub_graph_facade->child_node_facade_removed, child_node_facade_removed);

    child_added(sub_graph_facade);
}

void GraphFacadeImplementation::clearBlock()
{
    executor_.clear();
}

void GraphFacadeImplementation::resetActivity()
{
    bool pause = isPaused();

    pauseRequest(true);

    graph_->resetActivity();

    for (auto pair : children_) {
        GraphFacadePtr child = pair.second;
        child->resetActivity();
    }

    if (!parent_) {
        graph_node_->activation();
    }

    pauseRequest(pause);
}

bool GraphFacadeImplementation::isPaused() const
{
    return executor_.isPaused();
}

void GraphFacadeImplementation::pauseRequest(bool pause)
{
    if (executor_.isPaused() == pause) {
        return;
    }

    executor_.setPause(pause);

    paused(pause);
}

std::string GraphFacadeImplementation::makeStatusString() const
{
    return graph_node_->makeStatusString();
}

std::vector<UUID> GraphFacadeImplementation::enumerateAllNodes() const
{
    return graph_->getAllNodeUUIDs();
}

std::vector<ConnectionDescription> GraphFacadeImplementation::enumerateAllConnections() const
{
    std::vector<ConnectionDescription> result;
    auto connections = graph_->getConnections();
    result.reserve(connections.size());
    for (const ConnectionPtr& c : connections) {
        result.push_back(c->getDescription());
    }
    return result;
}
