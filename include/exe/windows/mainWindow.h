#pragma once

#include "exe/windows/renderWidget.h"
#include <chrono>
#include <exe/windows/paletteWidget.h>
#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>
#include <exe/windows/disassemblyWidget.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <exe/qtController.h>
#include <exe/windows/oamWidget.h>
#include <exe/windows/debugWidget.h>
#include <core/bus.h>

namespace NesEmulatorExe
{
    enum class Mode
    {
        NORMAL,
        UNLIMITED,
        STEP
    };

    class MainWindow : public QMainWindow
    {
        //Q_OBJECT

    public:
        MainWindow(NesEmulator::Bus& bus, Mode mode = Mode::NORMAL, QWidget* parent = nullptr);
        virtual ~MainWindow() {};

        Mode GetCurrentMode() const { return m_mode; }

        void SetFramerate(unsigned framerate);
        unsigned GetFramerate() const { return m_framerate; }

        // bool event(QEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    private slots:
        void Step();
        void Update();
        void Reset();
        void ToggleDebug();
        void TogglePalette();
        void ToggleDisassembly();
        void ToggleOAM();
        void SetNormalMode();
        void SetUnlimittedMode();

    private:

        void SetMode(Mode mode);
        void UpdateAllWidgets();
        void UpdateStatus();
        void UpdateModes();
        void CreateAllExtraActions();

        NesEmulator::Bus& m_bus;
        std::unique_ptr<QWidget> m_centralWidget;
        std::unique_ptr<QHBoxLayout> m_mainLayout;

        std::unique_ptr<RenderWidget> m_renderWidget;
        std::unique_ptr<QVBoxLayout> m_renderDebugLayout;
        
        std::unique_ptr<DebugWidget> m_debugWidget;
        std::unique_ptr<DisassemblyWidget> m_disassemblyWidget;
        std::unique_ptr<OAMWidget> m_oamWidget;
        std::unique_ptr<PaletteWidget> m_paletteWidget;

        std::unique_ptr<QAction> m_toggleDebugAction;
        std::unique_ptr<QAction> m_toggleDisassemblyAction;
        std::unique_ptr<QAction> m_togglePaletteAction;
        std::unique_ptr<QAction> m_toggleOAMAction;

        std::unique_ptr<QAction> m_setNormalMode;
        std::unique_ptr<QAction> m_setUnlimittedMode;

        std::unique_ptr<QAction> m_resetAction;

        std::unique_ptr<QTimer> m_renderingTimer;
        std::unique_ptr<QLabel> m_statusBarFPS;

        Mode m_mode = Mode::NORMAL;
        unsigned m_framerate;
        // unsigned m_remainingClocksAfterBreak = 0;

        std::shared_ptr<QtController> m_controller;
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        unsigned long long m_accumulator = 0;
        unsigned long long m_fpsCounterCount = 0;
    };
}