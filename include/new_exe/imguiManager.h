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

        bool ShouldSaveState() const { return m_requestSaveState; }
        void ResetSaveState() { m_requestSaveState = false; }

        bool ShouldLoadState() const { return m_requestLoadState; }
        void ResetLoadState() { m_requestLoadState = false; } 

        void Update();

    private:
        void HandleFileExplorer();

        bool m_showFileExplorer = false;
        bool m_closeRequested = false;
        bool m_showMainMenu = true;
        bool m_requestSaveState = false;
        bool m_requestLoadState = false;

        std::string m_currentPathToLoad;
    };
}