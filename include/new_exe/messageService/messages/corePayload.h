#pragma once

#include <new_exe/messageService/message.h>
#include <string>

namespace NesEmulatorGL 
{
    using CoreMessageType = uint32_t;

    enum DefaultCoreMessageType : CoreMessageType
    {
        LOAD_NEW_GAME,
        SAVE_STATE,
        LOAD_STATE
    };

    class CorePayload : public Payload
    {
    public:
        CorePayload(CoreMessageType type, std::string data)
            : m_type(type)
            , m_data(data)
        {}

        CoreMessageType m_type;
        std::string m_data;
    };
}