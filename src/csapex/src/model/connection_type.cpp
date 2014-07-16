/// HEADER
#include <csapex/model/connection_type.h>

/// COMPONENT
#include <csapex/model/message.h>
#include <csapex/utility/assert.h>

/// SYSTEM
#include <iostream>

using namespace csapex;

ConnectionType::Ptr ConnectionType::default_;

ConnectionType::ConnectionType(const std::string& name)
    : name_(name), seq_no_(-1)
{
}

ConnectionType::~ConnectionType()
{
}

void ConnectionType::setName(const std::string &name)
{
    name_ = name;
}

ConnectionType::Ptr ConnectionType::getDefaultConnectionType()
{
    return default_->clone();
}

bool ConnectionType::canConnectTo(const ConnectionType *other_side) const
{
    return other_side->acceptsConnectionFrom(this);
}

bool ConnectionType::acceptsConnectionFrom(const ConnectionType *other_side) const
{
    return name_ == other_side->name();
}

std::string ConnectionType::name() const
{
    return name_;
}

std::string ConnectionType::rawName() const
{
    return name_;
}

int ConnectionType::sequenceNumber() const
{
    return seq_no_;
}

void ConnectionType::setSequenceNumber(int seq_no)
{
    seq_no_ = seq_no;
}

ConnectionType::Ptr ConnectionType::makeDefault()
{
    if(!default_) {
        ConnectionType::default_ = connection_types::AnyMessage::make();
    }

    ConnectionType::Ptr res = default_->clone();
    apex_assert_hard(res);
    return res;
}

bool ConnectionType::isValid() const
{
    return true;
}

void ConnectionType::write(std::ostream &out) const
{
    YAML::Emitter yaml;
    yaml << /*YAML::Flow << */YAML::BeginMap;
    yaml << YAML::Key << "type" << YAML::Value << name();

    writeYaml(yaml);

    yaml << YAML::EndMap;

    out << yaml.c_str();
}

void ConnectionType::writeRaw(const std::string &/*file*/, const std::string& /*suffix*/) const
{
    std::cerr << "error: writeRaw not implemented for message type " << name() << std::endl;
}
