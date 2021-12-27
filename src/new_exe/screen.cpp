#include <new_exe/screen.h>
#include <core/bus.h>
#include <core/palette.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//////////////////////////// SHADERS ////////////////////////////////

constexpr const char* vertexShaderPrg = R"foo(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform vec2 screenFormat;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = vec4(vertex.x * screenFormat.x, vertex.y * screenFormat.y, 0.0f, 1.0f);
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


/////////////////////////////////////////////////////////////////////

using NesEmulatorGL::Screen;

Screen::Screen(unsigned internalResWidth, unsigned internalResHeight)
    : m_internalResWidth(internalResWidth)
    , m_internalResHeight(internalResHeight)
{
    if (!CreateShader())
        return;

    if (!CreateImage())
        return;

    m_initialized = true;

    m_screenFormat[0] = 1.0f;
    m_screenFormat[1] = 1.0f;
}

Screen::~Screen()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_programId);
    glDeleteTextures(1, &m_texture);
}

void Screen::SetScreenFormat(Format format)
{
    m_format = format;
    OnScreenResized(m_currentWidth, m_currentHeight);
}

void Screen::OnScreenResized(int width, int height)
{
    m_currentWidth = width;
    m_currentHeight = height;

    float currentRatio = (float) width / height;
    float targetRatio = 1.0f;

    switch(m_format)
    {
    case Format::STRETCH:
        targetRatio = currentRatio;
        break;
    case Format::ORIGINAL:
    {
        targetRatio = (float)m_internalResWidth / m_internalResHeight;
        break;
    }
    case Format::FOUR_THIRD:
    {
        targetRatio = 4.0f / 3.0f;
        break;
    }
    }

    if (currentRatio > targetRatio)
    {
        // We have a bigger width that needed
        m_screenFormat[0] = targetRatio / currentRatio;
        m_screenFormat[1] = 1.0f;
    }
    else 
    {
        // We have a bigger height that needed
        m_screenFormat[0] = 1.0f;
        m_screenFormat[1] = currentRatio / targetRatio;
    }
}

void Screen::Update(NesEmulator::Bus& bus)
{
    if (!m_initialized)
        return;
        
    int paletteLocation = glGetUniformLocation(m_programId, "palette");
    glUniform3fv(paletteLocation, 64, NesEmulator::Palette::GetPaletteFloat());

    int screenFormatLocation = glad_glGetUniformLocation(m_programId, "screenFormat");
    glUniform2fv(screenFormatLocation, 1, m_screenFormat);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glUseProgram(m_programId);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_internalResWidth, m_internalResHeight, GL_RED, GL_UNSIGNED_BYTE, bus.GetPPU().GetScreen());
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Screen::CreateShader()
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

bool Screen::CreateImage()
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