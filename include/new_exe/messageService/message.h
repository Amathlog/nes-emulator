#pragma once
#include <cstdint>
#include <core/constants.h>
#include <iterator>
#include <type_traits>

namespace NesEmulatorGL {

    using MessageType = uint32_t;

    enum DefaultMessageType : MessageType
    {
        CORE = 0,
        IMGUI = 1,
        SCREEN = 2,
        AUDIO = 3,

        INVALID = 0xFFFFFFFF
    };

    class Payload{};

    class GenericPayload : public Payload
    {
    public:
        GenericPayload(uint8_t* data, size_t dataSize, size_t dataCapacity)
            : m_data(data)
            , m_dataSize(dataSize)
            , m_dataCapacity(dataCapacity)
        {}

        virtual ~GenericPayload() = default;

    private:
        uint8_t* m_data;
        size_t m_dataSize;
        size_t m_dataCapacity;
    };

    class Message
    {
    public:
        Message() = default;

        Message(MessageType type_, Payload* payload_)
            : m_type(type_)
            , m_payload(payload_)
        {}

        virtual ~Message() = default;

        MessageType GetType() const { return m_type; }
        Payload* GetPayload() { return m_payload; }
        const Payload* GetPayload() const { return m_payload; }

    private:
        MessageType m_type = DefaultMessageType::INVALID;
        Payload* m_payload = nullptr;
    };

    template<MessageType Type, typename PayloadType, typename = std::enable_if_t<std::is_base_of_v<Payload, PayloadType>>>
    struct TypedMessage : public Message
    {
    public:
        template<typename... Args>
        TypedMessage(Args&& ...args)
            : ownPayload(std::forward<Args>(args)...)
            , Message(Type, &ownPayload)
        {}

        const PayloadType& GetTypedPayload() { return ownPayload; }

    private:
        PayloadType ownPayload;
    };
}
