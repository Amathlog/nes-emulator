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

        void Update();

    private:
        void HandleFileExplorer();

        bool m_showFileExplorer = false;
        bool m_closeRequested = false;

        std::string m_currentPathToLoad;
    };
}