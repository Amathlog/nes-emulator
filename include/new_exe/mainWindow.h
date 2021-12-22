#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <cmath>

struct GLFWwindow;

namespace NesEmulatorGL
{
    class ImguiManager;

    constexpr int NTSC_FRAMERATE = 60;
    constexpr int PAL_FRAMERATE = 50;
    constexpr int UNCAPPED_FRAMERATE = -1;

    class MainWindow
    {
    public:
        MainWindow(unsigned int width, unsigned int height, int framerate = NTSC_FRAMERATE);
        ~MainWindow();

        void Update();

        GLFWwindow* GetWindow() {return m_window;}
        bool RequestedClose();

        void Enable(bool enable) {m_enable = enable;}
        bool IsEnabled() {return m_enable;}

        void SetFramerate(int framerate) {
            m_framerate = framerate; 
            m_frametimeUS = m_framerate < 0 ? 0u : static_cast<int64_t>(std::floor(1000000.0/ m_framerate));
        }
        int GetFramerate() const { return m_framerate; }
        int64_t GetFrametime() const { return m_frametimeUS; }

        // unsigned int RegisterInputCallback(int inputKey, std::function<void(int)> callback);
        // void RemoveInputCallback(unsigned int id);
        // void ClearInputCallbacks() { m_inputCallbacks.clear(); }

    private:
        GLFWwindow* m_window = nullptr;
        std::vector<GLFWwindow*> m_childrenWindows;
        bool m_enable = true;

        int m_framerate = -1;
        int64_t m_frametimeUS;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

        ImguiManager* m_imguiManager = nullptr;
        
        // using MapIdCallback = std::unordered_map<unsigned int, std::pair<int, std::function<void(int)>>>;
        // MapIdCallback m_inputCallbacks;
        // unsigned int m_latestCallbackId = 0;
    };
}