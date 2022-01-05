#include "core/palette.h"
#include <new_exe/messageService/messageService.h>
#include <new_exe/messageService/messages/screenMessage.h>
#include <cstdint>
#include <memory>
#include <new_exe/mainWindow.h>
#include <new_exe/imguiManager.h>
#include <new_exe/screen.h>
#include <new_exe/controller.h>
#include <core/bus.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>

using NesEmulatorGL::MainWindow;

namespace {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        // Notify the screen that we have resized
        NesEmulatorGL::DispatchMessageServiceSingleton::GetInstance().Push(NesEmulatorGL::ResizeMessage(width, height));
    } 
}

MainWindow::MainWindow(const char* name, unsigned width, unsigned height, unsigned internalResWidth, unsigned internalResHeight, int framerate)
    : Window(name, width, height, framerate)
{
    if (!m_enable)
        // Something went wrong
        return;

    m_controller = std::make_shared<NesEmulatorGL::Controller>(m_window, 0);

    m_screen = std::make_unique<Screen>(internalResWidth, internalResHeight);
    m_screen->OnScreenResized(width, height);
    m_imguiManager = std::make_unique<ImguiManager>(m_window);

    m_enable = m_screen->IsInitialized();
}

void MainWindow::InternalUpdate(bool externalSync)
{
    if (m_userData == nullptr)
        // No user data, something is wrong
        return;

    NesEmulator::Bus* bus = reinterpret_cast<NesEmulator::Bus*>(m_userData);

    m_controller->Update();

    static bool hasPressed = false;
    
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        hasPressed = true;
    }
    else if (hasPressed && glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
    {
        hasPressed = false;
        m_imguiManager->ToggleMainMenu();
    }

    if (!externalSync)
        m_screen->UpdateScreen(bus->GetPPU().GetScreen(), bus->GetPPU().GetHeight() * bus->GetPPU().GetWidth());

    m_screen->Update(*bus);
    m_imguiManager->Update();
}

bool MainWindow::RequestedClose()
{
    if (!m_enable || !m_window)
        return true;

    return m_imguiManager->ShouldClose() || glfwWindowShouldClose(m_window);
}

void MainWindow::ConnectController()
{
    if (m_userData == nullptr)
        // No user data, something is wrong
        return;

    NesEmulator::Bus* bus = reinterpret_cast<NesEmulator::Bus*>(m_userData);
    bus->ConnectController(m_controller, 0);
}

void MainWindow::OnScreenResized(int width, int height)
{
    if (m_screen != nullptr)
    {
        m_screen->OnScreenResized(width, height);
    }
}
