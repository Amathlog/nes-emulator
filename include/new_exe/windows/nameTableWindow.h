#pragma once

#include <new_exe/window.h>
#include <new_exe/rendering/image.h>
#include <memory>

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
        std::unique_ptr<Image> m_image;
    };
}