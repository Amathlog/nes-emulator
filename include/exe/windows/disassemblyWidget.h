#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>

namespace NesEmulatorExe
{
    class DisassemblyWidget : public QWidget
    {
        //Q_OBJECT

    public:
        DisassemblyWidget(NesEmulator::Bus& bus, unsigned nbDisassemblyLines, QWidget* parent = nullptr);
        virtual ~DisassemblyWidget() {};

        void Update();

    private:
        void UpdateStatus();
        void UpdateDisassembly();
        void UpdateRegistersAndAddr();
         
        NesEmulator::Bus& m_bus;
        std::unique_ptr<QVBoxLayout> m_mainLayout;
        std::vector<std::unique_ptr<QLabel>> m_disassemblyLines;
        std::unique_ptr<QLabel> m_statusLabel;
        std::unique_ptr<QLabel> m_ALabel;
        std::unique_ptr<QLabel> m_XLabel;
        std::unique_ptr<QLabel> m_YLabel;
        std::unique_ptr<QLabel> m_PCLabel;
        std::unique_ptr<QLabel> m_stackLabel;
    };
}