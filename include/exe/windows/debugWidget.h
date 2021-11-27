#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>

namespace NesEmulatorExe
{
    enum class Mode;

    class DebugWidget : public QWidget
    {
        //Q_OBJECT

    public:
        DebugWidget(NesEmulator::Bus& bus, Mode& mode, QWidget* parent = nullptr);
        virtual ~DebugWidget() {};

        bool ShouldBreak();
        void Update();

        QPushButton* GetStepButton() { return m_stepButton.get(); }

    private slots:
        void SetRunTo();
        void Break();

    private:
         
        NesEmulator::Bus& m_bus;
        std::unique_ptr<QHBoxLayout> m_buttonLayout;
        std::unique_ptr<QPushButton> m_stepButton;
        std::unique_ptr<QPushButton> m_breakButton;

        std::unique_ptr<QHBoxLayout> m_runToLayout;
        std::unique_ptr<QLabel> m_hexPrefix;
        std::unique_ptr<QLineEdit> m_pcEdit;
        std::unique_ptr<QPushButton> m_runToButton;

        uint32_t m_breakAddress = 1 << 24; // Just a number higher than 1 << 16
        Mode& m_mode;
    };
}