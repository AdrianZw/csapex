#ifndef COMMAND_ADD_CONNECTOR_H
#define COMMAND_ADD_CONNECTOR_H

/// COMPONENT
#include "command.h"
#include <csapex/csapex_fwd.h>

namespace csapex
{
namespace command
{

struct AddConnector : public Command
{
    AddConnector(const std::string& box_uuid, const std::string& label, const std::string& type, bool input, const std::string& uuid, bool forward = false);

protected:
    bool doExecute();
    bool doUndo();
    bool doRedo();

private:
    std::string type;
    std::string label;
    bool input;


    Connector* c;

    std::string b_uuid;
    std::string c_uuid;

    bool forward;
};

}

}

#endif // COMMAND_ADD_CONNECTOR_H