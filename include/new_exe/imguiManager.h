#pragma once
#include <string>

struct GLFWwindow;

namespace NesEmulatorGL
{
    class ImguiManager
    {
    public:
        ImguiManager(GLFWwindow* window);
        ~ImguiManager();

        std::string GetPathToNewGame() const { return m_currentPathToLoad; }
        bool ShouldClose() const { return m_closeRequested; }
        void ToggleMainMenu() { m_showMainMenu = !m_showMainMenu; }

        void Update();

    private:
        void HandleFileExplorer();

        bool m_showFileExplorer = false;
        bool m_closeRequested = false;
        bool m_showMainMenu = true;

        std::string m_currentPathToLoad;
    };
}