/// HEADER
#include <csapex/model/connection_information.h>

/// PROJECT
#include <csapex/model/connectable.h>

using namespace csapex;

ConnectionInformation::ConnectionInformation(const UUID& from, const UUID& to, const TokenDataConstPtr& type, bool active, const std::vector<Fulcrum> &fulcrums)
    : from(from), to(to), from_label(""), to_label(""), type(type), active(active),
      fulcrums(fulcrums)
{
}
