#ifndef REQUEST_SERIALIZER_H
#define REQUEST_SERIALIZER_H

/// PROJECT
#include <csapex/serialization/packet_serializer.h>
#include <csapex/io/remote_io_fwd.h>

/// SYSTEM
#include <inttypes.h>

namespace csapex
{
class RequestSerializerInterface
{
public:
    virtual ~RequestSerializerInterface();

    virtual void serializeRequest(const Request& packet, SerializationBuffer& data) = 0;
    virtual RequestPtr deserializeRequest(const SerializationBuffer& data, uint8_t request_id) = 0;

    virtual void serializeResponse(const Response& packet, SerializationBuffer& data) = 0;
    virtual ResponsePtr deserializeResponse(const SerializationBuffer& data, uint8_t request_id) = 0;
};

class RequestSerializer : public Singleton<RequestSerializer>, public Serializer
{
public:
    void serialize(const Streamable& packet, SerializationBuffer& data) override;
    StreamablePtr deserialize(const SerializationBuffer& data) override;

    static void registerSerializer(const std::string& type, std::shared_ptr<RequestSerializerInterface> serializer);

private:
    std::map<std::string, std::shared_ptr<RequestSerializerInterface>> serializers_;
};

template <typename S>
struct RequestSerializerRegistered
{
    RequestSerializerRegistered(const char* type)
    {
        RequestSerializer::registerSerializer(type, std::make_shared<S>());
    }
};
}  // namespace csapex

#define CSAPEX_REGISTER_REQUEST_SERIALIZER(Name)                                                                                                                                                       \
    namespace csapex                                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
    namespace io                                                                                                                                                                                       \
    {                                                                                                                                                                                                  \
    class Name##Serializer : public RequestSerializerInterface                                                                                                                                         \
    {                                                                                                                                                                                                  \
        virtual void serializeRequest(const Request& packet, SerializationBuffer& data) override                                                                                                       \
        {                                                                                                                                                                                              \
            packet.serializeVersioned(data);                                                                                                                                                           \
        }                                                                                                                                                                                              \
        virtual RequestPtr deserializeRequest(const SerializationBuffer& data, uint8_t request_id) override                                                                                            \
        {                                                                                                                                                                                              \
            auto result = std::make_shared<typename Name::RequestT>(request_id);                                                                                                                       \
            result->deserializeVersioned(data);                                                                                                                                                        \
            return result;                                                                                                                                                                             \
        }                                                                                                                                                                                              \
        virtual void serializeResponse(const Response& packet, SerializationBuffer& data) override                                                                                                     \
        {                                                                                                                                                                                              \
            packet.serializeVersioned(data);                                                                                                                                                           \
        }                                                                                                                                                                                              \
        virtual ResponsePtr deserializeResponse(const SerializationBuffer& data, uint8_t request_id) override                                                                                          \
        {                                                                                                                                                                                              \
            auto result = std::make_shared<typename Name::ResponseT>(request_id);                                                                                                                      \
            result->deserializeVersioned(data);                                                                                                                                                        \
            return result;                                                                                                                                                                             \
        }                                                                                                                                                                                              \
    };                                                                                                                                                                                                 \
    }                                                                                                                                                                                                  \
    RequestSerializerRegistered<io::Name##Serializer> g_register_##Name##_(#Name);                                                                                                                     \
    }

#endif  // REQUEST_SERIALIZER_H
