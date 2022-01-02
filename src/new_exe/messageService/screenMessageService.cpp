#include <new_exe/messageService/message.h>
#include <new_exe/messageService/messages/screenPayload.h>
#include <new_exe/messageService/screenMessageService.h>
#include <new_exe/screen.h>

using NesEmulatorGL::ScreenMessageService;

bool ScreenMessageService::Push(const Message& message)
{
    if (message.GetType() != DefaultMessageType::SCREEN)
        return true;

    auto payload = reinterpret_cast<const ScreenPayload*>(message.GetPayload());

    switch (payload->m_type)
    {
    case DefaultScreenMessageType::CHANGE_FORMAT:
        m_screen.SetScreenFormat(payload->m_format);
        break;
    case DefaultScreenMessageType::RESIZE:
        m_screen.OnScreenResized(payload->m_width, payload->m_height);
        break;
    case DefaultScreenMessageType::RENDER:
        m_screen.UpdateScreen(payload->m_screenData, payload->m_screenDataSize);
        break;
    }

    return true;
}

bool ScreenMessageService::Pull(Message& message)
{
    if (message.GetType() != DefaultMessageType::SCREEN)
        return true;

    auto payload = reinterpret_cast<ScreenPayload*>(message.GetPayload());

    switch (payload->m_type)
    {
    case DefaultScreenMessageType::GET_FORMAT:
        payload->m_format = m_screen.GetScreenFormat();
        break;
    }

    return true;
}