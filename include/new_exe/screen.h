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
        enum Format
        {
            STRETCH,
            ORIGINAL,
            FOUR_THIRD
        };

        Screen(unsigned internalResWidth, unsigned internalResHeight);
        ~Screen();

        void Update(NesEmulator::Bus& bus);
        bool IsInitialized() const { return m_initialized; }
        void OnScreenResized(int width, int height);
        void SetScreenFormat(Format format);

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
    };
}