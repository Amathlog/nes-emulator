#pragma once

#include <cstdint>
#include <new_exe/messageService/messages/debugPayload.h>
#include <new_exe/messageService/message.h>

namespace NesEmulatorGL 
{
    using DebugMessage = TypedMessage<DefaultMessageType::DEBUG, DebugPayload>;

    struct GetNametablesMessage : DebugMessage
    {
        GetNametablesMessage(uint8_t* data, size_t dataSize, size_t dataCapacity)
            : DebugMessage(DefaultDebugMessageType::READ_NAMETABLES, data, dataSize, dataCapacity)
        {}
    };
}