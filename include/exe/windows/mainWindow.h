#pragma once

#include "exe/windows/renderWidget.h"
#include <exe/windows/paletteWidget.h>
#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>
#include <exe/windows/disassemblyWidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>

namespace NesEmulatorExe
{
    enum class Mode
    {
        NORMAL,
        UNLIMITED,
        STEP
    };

    class MainWindow : public QWidget
    {
        //Q_OBJECT

    public:
        MainWindow(NesEmulator::Bus& bus, Mode mode = Mode::NORMAL, QWidget* parent = nullptr);
        virtual ~MainWindow() {};

        Mode GetCurrentMode() const { return m_mode; }

        void SetFramerate(unsigned framerate);
        unsigned GetFramerate() const { return m_framerate; }

    private slots:
        void HandleButton();
        void Step();
        void Break();
        void Update();
        void SetRunTo();
        void Reset();

    private:

        void SetMode(Mode mode);

        NesEmulator::Bus& m_bus;
        std::unique_ptr<QHBoxLayout> m_mainLayout;
        std::unique_ptr<QVBoxLayout> m_buttonLayout;

        std::unique_ptr<QPushButton> m_resetButton;
        std::unique_ptr<QPushButton> m_stepButton;
        std::unique_ptr<QPushButton> m_breakButton;

        std::unique_ptr<QHBoxLayout> m_runToLayout;
        std::unique_ptr<QLabel> m_hexPrefix;
        std::unique_ptr<QLineEdit> m_pcEdit;
        std::unique_ptr<QPushButton> m_runToButton;

        std::unique_ptr<DisassemblyWidget> m_disassemblyWidget;
        std::unique_ptr<PaletteWidget> m_paletteWidget;
        std::unique_ptr<RenderWidget> m_renderWidget;

        std::unique_ptr<QTimer> m_renderingTimer;

        Mode m_mode = Mode::NORMAL;
        unsigned m_framerate;
        uint32_t m_breakAddress = 1 << 24; // Just a number higher than 1 << 16
        // unsigned m_remainingClocksAfterBreak = 0;
    };
}