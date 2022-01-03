#pragma once

#include <core/constants.h>
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
        LOAD_STATE,
        GET_MODE,
        CHANGE_MODE
    };

    class CorePayload : public Payload
    {
    public:
        CorePayload(CoreMessageType type, std::string data, int saveStateNumber = 0, NesEmulator::Mode mode = NesEmulator::Mode::NTSC)
            : m_type(type)
            , m_data(data)
            , m_saveStateNumber(saveStateNumber)
            , m_mode(mode)
        {}

        CoreMessageType m_type;
        std::string m_data;
        int m_saveStateNumber;
        NesEmulator::Mode m_mode;
    };
}