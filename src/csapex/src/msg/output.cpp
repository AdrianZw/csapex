/// HEADER
#include <csapex/msg/output.h>

/// COMPONENT
#include <csapex/msg/input.h>
#include <csapex/model/connection.h>
#include <csapex/msg/output_transition.h>
#include <csapex/utility/timer.h>
#include <csapex/utility/assert.h>
#include <csapex/msg/message_traits.h>

/// SYSTEM
#include <iostream>

using namespace csapex;

Output::Output(OutputTransition* transition, const UUID& uuid)
    : Connectable(uuid), transition_(transition),
      force_send_message_(false), state_(State::IDLE)
{
}

Output::Output(OutputTransition* transition, Unique* parent, int sub_id)
    : Connectable(parent, sub_id, "out"),  transition_(transition),
      force_send_message_(false), state_(State::IDLE)
{
}

Output::~Output()
{
}

OutputTransition* Output::getTransition() const
{
    return transition_;
}

void Output::activate()
{
    setState(State::ACTIVE);
}

void Output::setState(State s)
{
    state_ = s;
}

Output::State Output::getState() const
{
    return state_;
}

void Output::reset()
{
    clear();

    setSequenceNumber(0);
    setState(State::IDLE);
}

int Output::countConnections()
{
    return connections_.size();
}

std::vector<ConnectionPtr> Output::getConnections() const
{
    return connections_;
}


void Output::addConnection(ConnectionPtr connection)
{
    transition_->addConnection(connection);
    Connectable::addConnection(connection);
}

void Output::fadeConnection(ConnectionPtr connection)
{
    transition_->fadeConnection(connection);
    Connectable::fadeConnection(connection);
}

void Output::removeConnection(Connectable* other_side)
{
    std::unique_lock<std::recursive_mutex> lock(sync_mutex);
    for(std::vector<ConnectionPtr>::iterator i = connections_.begin(); i != connections_.end();) {
        ConnectionPtr c = *i;
        if(c->to() == other_side) {
            other_side->removeConnection(this);

            i = connections_.erase(i);

            connectionRemoved(this);
            return;

        } else {
            ++i;
        }
    }
}


void Output::removeAllConnectionsNotUndoable()
{
    std::unique_lock<std::recursive_mutex> lock(sync_mutex);
    for(std::vector<ConnectionPtr>::iterator i = connections_.begin(); i != connections_.end();) {
        (*i)->to()->removeConnection(this);
        i = connections_.erase(i);
    }

    disconnected(this);
}

void Output::disable()
{
    Connectable::disable();
}

bool Output::isConnectionPossible(Connectable *other_side)
{
    if(!other_side->canInput()) {
        std::cerr << "cannot connect, other side can't input" << std::endl;
        return false;
    }
    if(!other_side->canConnectTo(this, false)) {
        std::cerr << "cannot connect, other side can't connect" << std::endl;
        return false;
    }

    return true;
}

bool Output::targetsCanBeMovedTo(Connectable* other_side) const
{
    std::unique_lock<std::recursive_mutex> lock(sync_mutex);
    for(ConnectionPtr connection : connections_) {
        if(!connection->to()->canConnectTo(other_side, true)/* || !canConnectTo(*it)*/) {
            return false;
        }
    }
    return true;
}

bool Output::isConnected() const
{
    if(force_send_message_) {
        return true;
    }

    return !connections_.empty();
}

bool Output::isForced() const
{
    if(!force_send_message_) {
        return false;

    } else if(isConnected()) {
        return false;
    }

    return true;
}

void Output::connectionMovePreview(Connectable *other_side)
{
    std::unique_lock<std::recursive_mutex> lock(sync_mutex);
    for(ConnectionPtr connection : connections_) {
        connectionInProgress(connection->to(), other_side);
    }
}

void Output::validateConnections()
{
    for(ConnectionPtr connection : connections_) {
        connection->to()->validateConnections();
    }
}

bool Output::canSendMessages() const
{
    return true;
}

void Output::forceSendMessage(bool force)
{
    force_send_message_ = force;
}
