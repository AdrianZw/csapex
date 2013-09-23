/// HEADER
#include "nand.h"

/// COMPONENT
#include <csapex_boolean/boolean_message.h>

/// PROJECT
#include <csapex/model/box.h>
#include <csapex/model/connector_out.h>
#include <csapex/model/connector_in.h>

/// SYSTEM
#include <pluginlib/class_list_macros.h>

PLUGINLIB_EXPORT_CLASS(csapex::boolean::NAND, csapex::BoxedObject)

using namespace csapex;
using namespace csapex::boolean;
using namespace csapex::connection_types;

NAND::NAND()
{
    addTag(Tag::get("Boolean"));
}

void NAND::fill(QBoxLayout *layout)
{
    box_->setSynchronizedInputs(true);

    in_a = new ConnectorIn(box_, 0);
    in_a->setType(BooleanMessage::make());
    in_a->setLabel("A");
    box_->addInput(in_a);

    in_b = new ConnectorIn(box_, 1);
    in_b->setType(BooleanMessage::make());
    in_b->setLabel("B");
    box_->addInput(in_b);

    out = new ConnectorOut(box_, 0);
    out->setType(BooleanMessage::make());
    out->setLabel("A nand B");
    box_->addOutput(out);
}

void NAND::allConnectorsArrived()
{
    ConnectionType::Ptr msg_a = in_a->getMessage();
    BooleanMessage::Ptr a = boost::dynamic_pointer_cast<BooleanMessage> (msg_a);
    assert(a);

    ConnectionType::Ptr msg_b = in_b->getMessage();
    BooleanMessage::Ptr b = boost::dynamic_pointer_cast<BooleanMessage> (msg_b);
    assert(b);

    BooleanMessage::Ptr msg(new BooleanMessage);
    msg->value = !(a->value && b->value);
    out->publish(msg);
}