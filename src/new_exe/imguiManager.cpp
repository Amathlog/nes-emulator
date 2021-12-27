#include <new_exe/messageService/messageService.h>
#include <new_exe/messageService/messages/coreMessage.h>
#include <new_exe/imguiManager.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <ImGuiFileBrowser.h>

using NesEmulatorGL::ImguiManager;
using NesEmulatorGL::DispatchMessageServiceSingleton;
using NesEmulatorGL::LoadNewGameMessage;
using NesEmulatorGL::SaveStateMessage;
using NesEmulatorGL::LoadStateMessage;

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
            ImGui::MenuItem("Save state", nullptr, &m_requestSaveState);
            ImGui::MenuItem("Load state", nullptr, &m_requestLoadState);
            ImGui::Separator();
            ImGui::MenuItem("Exit", nullptr, &m_closeRequested);
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
        }
        ImGui::End();
    }

    if (m_requestSaveState)
    {
        DispatchMessageServiceSingleton::GetInstance().Push(SaveStateMessage());
        m_requestSaveState = false;
    }

    if (m_requestLoadState)
    {
        DispatchMessageServiceSingleton::GetInstance().Push(LoadStateMessage());
        m_requestLoadState = false;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiManager::HandleFileExplorer()
{
    if (m_showFileExplorer)
        ImGui::OpenPopup("Open File");

    static imgui_addons::ImGuiFileBrowser fileDialog;

    if(fileDialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".nes"))
    {
        // Load a new file
        DispatchMessageServiceSingleton::GetInstance().Push(LoadNewGameMessage(fileDialog.selected_path));
        m_showFileExplorer = false;
    }    
}