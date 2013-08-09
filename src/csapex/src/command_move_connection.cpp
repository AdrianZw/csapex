/// HEADER
#include <csapex/command_move_connection.h>

/// COMPONENT
#include <csapex/command.h>
#include <csapex/connector_in.h>
#include <csapex/connector_out.h>
#include <csapex/command_meta.h>
#include <csapex/command_delete_connection.h>
#include <csapex/command_add_connection.h>

/// SYSTEM
#include <boost/foreach.hpp>

using namespace csapex::command;

MoveConnection::MoveConnection(Connector *a, Connector *b)
{
    from = dynamic_cast<ConnectorOut*>(a);
    if(from) {
        to = dynamic_cast<ConnectorOut*>(b);
        output = true;

    } else {
        from = dynamic_cast<ConnectorIn*>(a);
        to = dynamic_cast<ConnectorIn*>(b);
        output = false;
    }
    assert(from);
    assert(to);

    from_uuid = from->UUID();
    to_uuid = to->UUID();

    makeCommand();
}

void MoveConnection::makeCommand()
{
    nested.clear();
    locked = false;

    if(output) {
        ConnectorOut* out = dynamic_cast<ConnectorOut*>(from);

        for(ConnectorOut::TargetIterator it = out->beginTargets(); it != out->endTargets(); ++it) {
            add(Command::Ptr(new DeleteConnection(from, *it)));
            add(Command::Ptr(new AddConnection(to, *it)));
        }

    } else {
        ConnectorIn* in = dynamic_cast<ConnectorIn*>(from);

        ConnectorOut* target = in->getConnected();
        add(Command::Ptr(new DeleteConnection(from, target)));
        add(Command::Ptr(new AddConnection(to, target)));
    }
}