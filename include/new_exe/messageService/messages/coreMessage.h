#pragma once

#include <new_exe/messageService/message.h>
#include <new_exe/messageService/messages/corePayload.h>

namespace NesEmulatorGL 
{
    using CoreMessage = TypedMessage<DefaultMessageType::CORE, CorePayload>;

    struct LoadNewGameMessage : CoreMessage
    {
        LoadNewGameMessage(std::string file)
            : CoreMessage(DefaultCoreMessageType::LOAD_NEW_GAME, file)
        {}
    };

    struct SaveStateMessage : CoreMessage
    {
        SaveStateMessage()
            : CoreMessage(DefaultCoreMessageType::SAVE_STATE, "")
        {}
    };

    struct LoadStateMessage : CoreMessage
    {
        LoadStateMessage()
            : CoreMessage(DefaultCoreMessageType::LOAD_STATE, "")
        {}
    };
}