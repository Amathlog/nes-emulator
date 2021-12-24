#include "core/palette.h"
#include <cstdint>
#include <new_exe/mainWindow.h>
#include <new_exe/imguiManager.h>
#include <core/bus.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>

constexpr const char* vertexShaderPrg = R"foo(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = vec4(vertex.xy, -1.0f, 1.0f);
}
)foo";

constexpr const char* fragmentShaderPrg = R"foo(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D Texture;
uniform vec3 palette[64];

void main()
{   
    color = vec4(palette[int(texture(Texture, TexCoords) * 255.0f)], 1.0);
}  
)foo";

using NesEmulatorGL::MainWindow;

namespace {
    void framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    } 
}

MainWindow::MainWindow(unsigned width, unsigned height, unsigned internalResWidth, unsigned internalResHeight, int framerate)
    : m_internalResWidth(internalResWidth)
    , m_internalResHeight(internalResHeight)
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

    // Create the shaders
    if (!CreateShader())
    {
        return;
    }

    CreateImage();

    m_enable = true;
    m_lastUpdateTime = std::chrono::high_resolution_clock::now();
}

bool MainWindow::CreateShader()
{
    unsigned vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderPrg, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    };
    
    // similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderPrg, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    };
    
    // shader Program
    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertex);
    glAttachShader(m_programId, fragment);
    glLinkProgram(m_programId);
    // print linking errors if any
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_programId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
}

bool MainWindow::CreateImage()
{
    float vertices[] = {
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f,
    };

    unsigned indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_internalResWidth, m_internalResHeight, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUseProgram(m_programId);
    glUniform1i(glGetUniformLocation(m_programId, "Texture"), 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

MainWindow::~MainWindow()
{
    for (auto window : m_childrenWindows)
        glfwDestroyWindow(window);

    delete m_imguiManager;

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void MainWindow::RenderImage(NesEmulator::Bus& bus)
{
    int paletteLocation = glGetUniformLocation(m_programId, "palette");
    glUniform3fv(paletteLocation, 64, NesEmulator::Palette::GetPaletteFloat());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glUseProgram(m_programId);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_internalResWidth, m_internalResHeight, GL_RED, GL_UNSIGNED_BYTE, bus.GetPPU().GetScreen());
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainWindow::Update(NesEmulator::Bus& bus)
{
    if (RequestedClose())
        return;

    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderImage(bus);

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