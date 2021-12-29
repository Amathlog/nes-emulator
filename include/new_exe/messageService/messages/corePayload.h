#pragma once

#include <new_exe/messageService/message.h>
#include <string>

namespace NesEmulatorGL 
{
    using CoreMessageType = uint32_t;

    enum DefaultCoreMessageType : CoreMessageType
    {
        LOAD_NEW_GAME,
        SAVE_GAME,
        LOAD_SAVE,
        SAVE_STATE,
        LOAD_STATE
    };

    class CorePayload : public Payload
    {
    public:
        CorePayload(CoreMessageType type, std::string data, int saveStateNumber = 0)
            : m_type(type)
            , m_data(data)
            , m_saveStateNumber(saveStateNumber)
        {}

        CoreMessageType m_type;
        std::string m_data;
        int m_saveStateNumber;
    };
}