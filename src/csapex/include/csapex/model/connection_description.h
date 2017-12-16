#ifndef CONNECTION_DESCRIPTION_H
#define CONNECTION_DESCRIPTION_H

/// COMPONENT
#include <csapex/serialization/serializable.h>
#include <csapex/utility/uuid.h>
#include <csapex/model/model_fwd.h>
#include <csapex/csapex_export.h>
#include <csapex/model/fulcrum.h>

namespace csapex
{

struct CSAPEX_EXPORT ConnectionDescription : public Serializable
{
    UUID from;
    UUID to;
    std::string from_label;
    std::string to_label;
    TokenDataConstPtr type;

    int id;

    bool active;

    std::vector<Fulcrum> fulcrums;

    ConnectionDescription(const UUID& from, const UUID& to,
                          const TokenDataConstPtr &type, int id,
                          bool active, const std::vector<Fulcrum>& fulcrums);

    ConnectionDescription(const ConnectionDescription& other);

    ConnectionDescription& operator = (const ConnectionDescription& other);

    virtual void serialize(SerializationBuffer &data) const override;
    virtual void deserialize(const SerializationBuffer& data) override;

    bool operator == (const ConnectionDescription& other) const;

protected:
    virtual std::shared_ptr<Clonable> makeEmptyClone() const override;

private:
    friend class SerializationBuffer;
    ConnectionDescription();
};

}

#endif // CONNECTION_DESCRIPTION_H