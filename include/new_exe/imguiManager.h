#pragma once
#include <string>
#include <new_exe/messageService/messages/screenPayload.h>
#include <array>

struct GLFWwindow;

namespace NesEmulatorGL
{
    class ImguiManager
    {
    public:
        ImguiManager(GLFWwindow* window);
        ~ImguiManager();

        bool ShouldClose() const { return m_closeRequested; }
        void ToggleMainMenu() { m_showMainMenu = !m_showMainMenu; }

        void Update();

    private:
        void HandleFileExplorer();

        bool m_showFileExplorer = false;
        bool m_closeRequested = false;
        bool m_showMainMenu = true;
        bool m_isSoundEnabled = false;
        bool m_previousSoundState = false;

        Format m_currentFormat = Format::ORIGINAL;

        std::array<bool, (size_t)Format::COUNT> m_changeFormats;
        
        inline static constexpr unsigned MAX_SAVE_STATES = 4;
        std::array<bool, MAX_SAVE_STATES> m_requestSaveState;
        std::array<bool, MAX_SAVE_STATES> m_requestLoadState;

        std::array<float, 100> m_frametimes = {};
        size_t m_frametimeOffset = 0;
    };
}