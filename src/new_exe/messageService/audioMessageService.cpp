#include <new_exe/messageService/message.h>
#include <new_exe/messageService/messages/audioPayload.h>
#include <new_exe/messageService/audioMessageService.h>
#include <new_exe/audio/audioSystem.h>

using NesEmulatorGL::AudioMessageService;

bool AudioMessageService::Push(const Message& message)
{
    if (message.GetType() != DefaultMessageType::AUDIO)
        return true;

    auto payload = reinterpret_cast<const AudioPayload*>(message.GetPayload());

    switch (payload->m_type)
    {
    case DefaultAudioMessageType::ENABLE_AUDIO:
        m_audioSystem.Enable(payload->m_data);
        break;
    }

    return true;
}

bool AudioMessageService::Pull(Message& message)
{
    if (message.GetType() != DefaultMessageType::AUDIO)
        return true;

    auto payload = reinterpret_cast<AudioPayload*>(message.GetPayload());

    switch (payload->m_type)
    {
    case DefaultAudioMessageType::GET_ENABLE:
        payload->m_data = m_audioSystem.IsEnabled();
        break;
    }

    return true;
}