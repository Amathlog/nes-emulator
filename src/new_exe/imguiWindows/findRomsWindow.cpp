#include <new_exe/imguiWindows/findRomsWindow.h>
#include <new_exe/messageService/messageService.h>
#include <new_exe/messageService/messages/coreMessage.h>
#include <new_exe/utils.h>
#include <imgui.h>
#include <iostream>

using NesEmulatorGL::FindRomsWindow;

void FindRomsWindow::DrawInternal()
{
    if (m_allRoms.empty())
        RefreshRoms();

    for (auto& file : m_allRoms)
    {
        ImGui::Selectable(file.filenameWithImguiTag.c_str(), &file.selected);
    }
    
    for (auto& file : m_allRoms)
    {
        if (file.selected)
        {
            file.selected = false;
            m_open = false;
            DispatchMessageServiceSingleton::GetInstance().Push(LoadNewGameMessage(file.fullPath));
        }
    }
}

void FindRomsWindow::RefreshRoms()
{
    m_allRoms.clear();
    int index = 0;

    std::filesystem::path root = NesEmulatorGL::GetRootPath();

    auto findRoms = [this, &index](const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
            return;

        for (const auto& file : std::filesystem::recursive_directory_iterator(path))
        {
            if (!file.is_regular_file())
                continue;

            auto extension = file.path().extension();
            if (extension == ".nes")
            {
                FileEntry fileEntry;
                fileEntry.fullPath = file.path().string();
                fileEntry.filenameWithImguiTag = file.path().filename().string() + "##" + std::to_string(index++);
                fileEntry.selected = false;
                m_allRoms.push_back(std::move(fileEntry));
            }
        }
    };

    // Hard coded paths
    findRoms(root / "roms");
    findRoms(root / "tests");
}