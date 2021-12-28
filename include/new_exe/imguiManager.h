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
        bool m_requestSaveState = false;
        bool m_requestLoadState = false;

        Format m_currentFormat = Format::ORIGINAL;

        std::array<bool, Format::COUNT> m_changeFormats;
    };
}