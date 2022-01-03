#pragma once

#include <new_exe/messageService/message.h>
#include <new_exe/messageService/messages/screenPayload.h>

namespace NesEmulatorGL 
{
    using ScreenMessage = TypedMessage<DefaultMessageType::SCREEN, ScreenPayload>;

    struct ChangeFormatMessage : public ScreenMessage
    {
        ChangeFormatMessage(Format format)
            : ScreenMessage(DefaultScreenMessageType::CHANGE_FORMAT, format, 0, 0)
        {}
    };

    struct ResizeMessage : public ScreenMessage
    {
        ResizeMessage(int width, int height)
            : ScreenMessage(DefaultScreenMessageType::RESIZE, Format::UNDEFINED, width, height)
        {}
    };

    struct GetFormatMessage : public ScreenMessage
    {
        GetFormatMessage() 
            : ScreenMessage(DefaultScreenMessageType::GET_FORMAT, Format::UNDEFINED, 0, 0)
        {}
    };

    struct RenderMessage : public ScreenMessage
    {
        RenderMessage(const uint8_t* data, size_t size)
            : ScreenMessage(DefaultScreenMessageType::RENDER, data, size)
        {}
    };

    struct GetFrametimeMessage : public ScreenMessage
    {
        GetFrametimeMessage()
            : ScreenMessage(DefaultScreenMessageType::GET_FRAMETIME)
        {}
    };
}