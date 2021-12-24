#pragma once

namespace NesEmulator
{
    class Bus;
}

namespace NesEmulatorGL
{
    class Screen
    {
    public:
        Screen(unsigned internalResWidth, unsigned internalResHeight);
        ~Screen();

        void Update(NesEmulator::Bus& bus);
        bool IsInitialized() const { return m_initialized; }
    private:
        bool CreateShader();
        bool CreateImage();

        bool m_initialized = false;

        unsigned m_internalResWidth;
        unsigned m_internalResHeight;

        // Shaders
        unsigned m_programId;
        // Image
        unsigned m_VAO;
        unsigned m_VBO;
        unsigned m_EBO;
        unsigned m_texture;
    };
}