#include <new_exe/messageService/messages/audioMessage.h>
#include <new_exe/messageService/messages/screenMessage.h>
#include <cstddef>
#include <new_exe/messageService/messageService.h>
#include <new_exe/messageService/messages/coreMessage.h>
#include <new_exe/imguiManager.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <ImGuiFileBrowser.h>
#include <string>
#include <chrono>

using NesEmulatorGL::ImguiManager;
using NesEmulatorGL::DispatchMessageServiceSingleton;
using NesEmulatorGL::LoadNewGameMessage;
using NesEmulatorGL::SaveStateMessage;
using NesEmulatorGL::LoadStateMessage;

using Clock = std::chrono::high_resolution_clock;

static Clock::time_point s_lastTick;

ImguiManager::ImguiManager(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    m_changeFormats.fill(false);

    // Get the current format
    GetFormatMessage message;
    if (DispatchMessageServiceSingleton::GetInstance().Pull(message))
    {
        m_currentFormat = message.GetTypedPayload().m_format;
        m_changeFormats[(unsigned)m_currentFormat] = true;
    }

    m_requestSaveState.fill(false);
    m_requestLoadState.fill(false);

    // Get is sound enabled
    IsAudioEnabledMessage audioMessage;
    if (DispatchMessageServiceSingleton::GetInstance().Pull(audioMessage))
    {
        m_isSoundEnabled = audioMessage.GetTypedPayload().m_data;
        m_previousSoundState = m_isSoundEnabled;
    }

    s_lastTick = Clock::now();
    m_frametimes.fill(0.0f);
    m_frametimeOffset = 0;

    UpdateCurrentMode();
}

ImguiManager::~ImguiManager()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DemoWindow()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ImguiManager::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool showFPS = false;

    if (m_showMainMenu)
    {
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Open File", nullptr, &m_showFileExplorer);
            ImGui::Separator();

            if (ImGui::BeginMenu("State states"))
            {
                for (auto i = 0; i < MAX_SAVE_STATES; ++i)
                {
                    std::string label = "State State ";
                    label += std::to_string(i);
                    ImGui::MenuItem(label.c_str(), nullptr, &m_requestSaveState[i]);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Load states"))
            {
                for (auto i = 0; i < MAX_SAVE_STATES; ++i)
                {
                    std::string label = "State State ";
                    label += std::to_string(i);
                    ImGui::MenuItem(label.c_str(), nullptr, &m_requestLoadState[i]);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();
            ImGui::MenuItem("Exit", nullptr, &m_closeRequested);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::BeginMenu("Screen Format"))
            {
                ImGui::MenuItem("Stretch", nullptr, &m_changeFormats[(unsigned)Format::STRETCH]);
                ImGui::MenuItem("Original", nullptr, &m_changeFormats[(unsigned)Format::ORIGINAL]);
                ImGui::MenuItem("4/3", nullptr, &m_changeFormats[(unsigned)Format::FOUR_THIRD]);
                ImGui::EndMenu();
            }

            ImGui::MenuItem("Enable Audio", nullptr, &m_isSoundEnabled);

            if (ImGui::BeginMenu("Region"))
            {
                ImGui::MenuItem("NTSC", nullptr, &m_requestChangeMode[(unsigned)NesEmulator::Mode::NTSC]);
                ImGui::MenuItem("PAL", nullptr, &m_requestChangeMode[(unsigned)NesEmulator::Mode::PAL]);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Show FPS", nullptr, &showFPS);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    HandleFileExplorer();

    if (showFPS)
    {
        static int corner = 0;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1)
        {
            const float PAD = 10.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = 0.0f;
            window_pos.y = 30.0f;
            window_pos_pivot.x = 0.0f;
            window_pos_pivot.y = 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("FPS", &showFPS, window_flags))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
            // Frametime
            auto newTick = Clock::now();
            float frametime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(newTick - s_lastTick).count();
            s_lastTick = newTick;
            m_frametimes[m_frametimeOffset] = frametime;
            m_frametimeOffset = (m_frametimeOffset + 1) % m_frametimes.size();
            ImGui::PlotLines("Frametime", m_frametimes.data(), (int)m_frametimes.size(), (int)m_frametimeOffset, nullptr, 0.0f, 100.f, ImVec2(0, 80.0f));
        }
        ImGui::End();
    }

    for (auto i = 0; i < MAX_SAVE_STATES; ++i)
    {
        if (m_requestSaveState[i])
        {
            DispatchMessageServiceSingleton::GetInstance().Push(SaveStateMessage("", (int)i));
            m_requestSaveState[i] = false;
            break;
        }

        if (m_requestLoadState[i])
        {
            DispatchMessageServiceSingleton::GetInstance().Push(LoadStateMessage("", (int)i));
            m_requestLoadState[i] = false;
            break;
        }
    }

    // Check format
    for (auto i = 0; i < m_changeFormats.size(); ++i)
    {
        if (m_changeFormats[i] && i != (size_t)m_currentFormat)
        {
            if (m_currentFormat != Format::UNDEFINED)
                m_changeFormats[(unsigned)m_currentFormat] = false;

            m_currentFormat = (Format)i;
            DispatchMessageServiceSingleton::GetInstance().Push(ChangeFormatMessage(m_currentFormat));
            break;
        }
    }

    // Check audio
    if (m_previousSoundState != m_isSoundEnabled)
    {
        m_previousSoundState = m_isSoundEnabled;
        DispatchMessageServiceSingleton::GetInstance().Push(EnableAudioMessage(m_isSoundEnabled));
    }

    // Check region
    for (auto i = 0; i < m_requestChangeMode.size(); ++i)
    {
        if (m_requestChangeMode[i] && (unsigned)i != (unsigned)m_currentMode)
        {
            DispatchMessageServiceSingleton::GetInstance().Push(ChangeModeMessage(NesEmulator::Mode((unsigned)i)));
            UpdateCurrentMode();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiManager::HandleFileExplorer()
{
    if (m_showFileExplorer)
        ImGui::OpenPopup("Open File");

    m_showFileExplorer = false;

    static imgui_addons::ImGuiFileBrowser fileDialog;

    if(fileDialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".nes"))
    {
        // Load a new file
        if (!fileDialog.selected_path.empty())
        {
            DispatchMessageServiceSingleton::GetInstance().Push(LoadNewGameMessage(fileDialog.selected_path));
            UpdateCurrentMode();
        }
    }
}

void ImguiManager::UpdateCurrentMode()
{
    // Get mode status
    m_requestChangeMode.fill(false);
    m_currentMode = NesEmulator::Mode::NTSC;
    GetModeMessage modeMessage;
    if (DispatchMessageServiceSingleton::GetInstance().Pull(modeMessage))
    {
        m_currentMode = modeMessage.GetTypedPayload().m_mode;
        m_requestChangeMode[(unsigned)m_currentMode] = true;
    }
}