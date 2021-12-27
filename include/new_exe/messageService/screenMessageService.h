#pragma once

#include <new_exe/messageService/messageService.h>

namespace NesEmulatorGL 
{
    class Screen;

    class ScreenMessageService : public IMessageService
    {
    public:
        ScreenMessageService(Screen& screen)
            : m_screen(screen)
        {}

        bool Push(const Message& message) override;
        bool Pull(Message& message) override;

    private:
        Screen& m_screen;
    };
}