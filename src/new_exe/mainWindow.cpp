#include "core/palette.h"
#include <cstdint>
#include <new_exe/mainWindow.h>
#include <new_exe/imguiManager.h>
#include <new_exe/screen.h>
#include <core/bus.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>

using NesEmulatorGL::MainWindow;

namespace {
    void framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    } 
}

MainWindow::MainWindow(unsigned width, unsigned height, unsigned internalResWidth, unsigned internalResHeight, int framerate)
{
    SetFramerate(framerate);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, "Renderer", nullptr, nullptr);
    if (m_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    framebuffer_size_callback(m_window, width, height);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    m_imguiManager = new ImguiManager(m_window);
    m_screen = new Screen(internalResWidth, internalResHeight);

    m_enable = m_screen->IsInitialized();
    m_lastUpdateTime = std::chrono::high_resolution_clock::now();
}

MainWindow::~MainWindow()
{
    for (auto window : m_childrenWindows)
        glfwDestroyWindow(window);

    delete m_screen;
    delete m_imguiManager;

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void MainWindow::Update(NesEmulator::Bus& bus)
{
    if (RequestedClose())
        return;

    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_screen->Update(bus);
    m_imguiManager->Update();

    // Swap buffers
    glfwSwapBuffers(m_window);

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>((currentTime - m_lastUpdateTime)).count();

    if (diff < m_frametimeUS)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_frametimeUS - diff));
    }

    m_lastUpdateTime = currentTime;
}

bool MainWindow::RequestedClose()
{
    if (!m_enable || !m_window)
        return true;

    return glfwWindowShouldClose(m_window);
}