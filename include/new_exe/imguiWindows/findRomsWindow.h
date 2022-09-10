#pragma once

#include <new_exe/imguiWindows/imguiWindow.h>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>

struct ImGuiContext;

struct FileEntry
{
    std::string fullPath;
    std::string filenameWithImguiTag;
    bool selected;
};

namespace NesEmulatorGL
{
    class FindRomsWindow : public ImGuiWindow
    {
    public:
        FindRomsWindow() = default;

        WINDOW_ID_IMPL(AllWindowsId::FindRomsWindowId);

    protected:
        void RefreshRoms();
        void DrawInternal() override;
        const char* GetWindowName() const override { return "Find Roms##12"; }

        std::vector<FileEntry> m_allRoms;
    };
}