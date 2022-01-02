#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <string>
#include <memory>

struct GLFWwindow;
namespace NesEmulator {
    class Bus;
}

namespace NesEmulatorGL
{
    class ImguiManager;
    class Screen;
    class Controller;

    constexpr int NTSC_FRAMERATE = 60;
    constexpr int PAL_FRAMERATE = 50;
    constexpr int UNCAPPED_FRAMERATE = -1;

    class MainWindow
    {
    public:
        MainWindow(const char* name, unsigned width, unsigned height, unsigned internalResWidth, unsigned internalResHeight, int framerate = NTSC_FRAMERATE);
        ~MainWindow();

        void Update(NesEmulator::Bus& bus, bool externalSync = false);

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

        void ConnectController(NesEmulator::Bus& bus);

        void OnScreenResized(int width, int height);

    private:
        void RenderImage(NesEmulator::Bus& bus);

        GLFWwindow* m_window = nullptr;
        std::shared_ptr<Controller> m_controller = nullptr;
        std::vector<GLFWwindow*> m_childrenWindows;
        bool m_enable = true;

        int m_framerate = -1;
        int64_t m_frametimeUS;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

        ImguiManager* m_imguiManager = nullptr;
        Screen* m_screen = nullptr;
    };
}