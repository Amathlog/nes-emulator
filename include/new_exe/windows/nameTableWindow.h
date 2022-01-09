#pragma once

#include <new_exe/window.h>

class ImGuiContext;

namespace NesEmulatorGL
{
    class NameTableWindow : public Window
    {
    public:
        NameTableWindow(unsigned width, unsigned height);
        ~NameTableWindow();

    protected:
        void InternalUpdate(bool externalSync) override;
        ImGuiContext* m_context;
    };
}