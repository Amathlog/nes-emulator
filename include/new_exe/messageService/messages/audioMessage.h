#pragma once

#include <new_exe/messageService/message.h>
#include <new_exe/messageService/messages/audioPayload.h>

namespace NesEmulatorGL 
{
    using AudioMessage = TypedMessage<DefaultMessageType::AUDIO, AudioPayload>;

    struct EnableAudioMessage : AudioMessage
    {
        EnableAudioMessage(bool enable)
            : AudioMessage(DefaultAudioMessageType::ENABLE_AUDIO, enable)
        {}
    };

    struct IsAudioEnabledMessage : AudioMessage
    {
        IsAudioEnabledMessage()
            : AudioMessage(DefaultAudioMessageType::GET_ENABLE, false)
        {}
    };
}