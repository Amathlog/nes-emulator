#pragma once

#include <new_exe/messageService/messages/screenPayload.h>
#include <memory>

namespace NesEmulator
{
    class Bus;
}

namespace NesEmulatorGL
{
    class ScreenMessageService;
    
    class Screen
    {
    public:

        Screen(unsigned internalResWidth, unsigned internalResHeight);
        ~Screen();

        void Update(NesEmulator::Bus& bus);
        bool IsInitialized() const { return m_initialized; }
        void OnScreenResized(int width, int height);
        void SetScreenFormat(Format format);
        Format GetScreenFormat() const { return m_format; }

    private:
        bool CreateShader();
        bool CreateImage();

        bool m_initialized = false;

        unsigned m_internalResWidth;
        unsigned m_internalResHeight;

        int m_currentWidth;
        int m_currentHeight;

        Format m_format = Format::ORIGINAL;
        float m_screenFormat[2];

        // Shaders
        unsigned m_programId;
        // Image
        unsigned m_VAO;
        unsigned m_VBO;
        unsigned m_EBO;
        unsigned m_texture;

        std::unique_ptr<ScreenMessageService> m_screenMessageService;
    };
}