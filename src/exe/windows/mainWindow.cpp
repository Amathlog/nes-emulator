#include "exe/windows/disassemblyWidget.h"
#include "exe/windows/oamWidget.h"
#include "exe/windows/paletteWidget.h"
#include "exe/windows/renderWidget.h"
#include <chrono>
#include <cstdio>
#include <exe/windows/mainWindow.h>
#include <core/processor2C02.h>
#include <core/palette.h>
#include <iostream>
#include <cstring>
#include <memory>
#include <qaction.h>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qtextformat.h>
#include <qwidget.h>
#include <string>

using NesEmulatorExe::MainWindow;
using NesEmulatorExe::DisassemblyWidget;
using NesEmulatorExe::Mode;


constexpr unsigned DEFAULT_FRAMERATE = 60;
constexpr unsigned NB_LINES_DISASSEMBLY = 25;


MainWindow::MainWindow(NesEmulator::Bus& bus, Mode mode, QWidget* parent)
    : QMainWindow(parent)
    , m_bus(bus)
    , m_mode(mode)
{
    m_centralWidget = std::make_unique<QWidget>();
    setCentralWidget(m_centralWidget.get());

    m_mainLayout = std::make_unique<QHBoxLayout>(m_centralWidget.get());
    m_renderDebugLayout = std::make_unique<QVBoxLayout>();

    m_mainLayout->addLayout(m_renderDebugLayout.get());

    m_renderWidget = std::make_unique<RenderWidget>(m_bus);
    m_renderDebugLayout->addWidget(m_renderWidget.get());

    m_renderingTimer = std::make_unique<QTimer>();
    connect(m_renderingTimer.get(), &QTimer::timeout, this, &MainWindow::Update);

    SetFramerate(DEFAULT_FRAMERATE);

    m_controller = std::make_shared<QtController>(0);
    m_bus.ConnectController(m_controller, 0);

    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    auto statusBarItem = statusBar();
    m_statusBarFPS = std::make_unique<QLabel>();
    statusBarItem->addWidget(m_statusBarFPS.get());


    m_lastUpdateTime = std::chrono::high_resolution_clock::now();

    // Files
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    m_resetAction = std::make_unique<QAction>(tr("&Reset"), this);
    connect(m_resetAction.get(), &QAction::triggered, this, &MainWindow::Reset);
    fileMenu->addAction(m_resetAction.get());

    // Modes
    auto modeMenu = menuBar()->addMenu(tr("&Modes"));
    m_setNormalMode = std::make_unique<QAction>(tr("&Normal"), this);
    m_setNormalMode->setCheckable(true);
    connect(m_setNormalMode.get(), &QAction::triggered, this, &MainWindow::SetNormalMode);

    m_setUnlimittedMode = std::make_unique<QAction>(tr("&Unlimitted"), this);
    m_setUnlimittedMode->setCheckable(true);
    connect(m_setUnlimittedMode.get(), &QAction::triggered, this, &MainWindow::SetUnlimittedMode);

    modeMenu->addAction(m_setNormalMode.get());
    modeMenu->addAction(m_setUnlimittedMode.get());

    CreateAllExtraActions();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (m_controller->UpdateOnEvent((Qt::Key)event->key(), true))
        return;

    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (m_controller->UpdateOnEvent((Qt::Key)event->key(), false))
        return;

    QWidget::keyPressEvent(event);
}

void MainWindow::Update()
{
    if (m_mode != Mode::STEP)
    {
        do
        {
            m_bus.Clock();
            if (m_debugWidget.get() != nullptr && m_debugWidget->ShouldBreak())
            {
                m_mode = Mode::STEP;
                break;
            }
        } while (!m_bus.GetPPU().IsFrameComplete());
    }

    UpdateStatus();
    UpdateModes();
    UpdateAllWidgets();
}

void MainWindow::Step()
{
    do
    {
        m_bus.Clock();
        m_bus.Clock();
        m_bus.Clock();
    } while (!m_bus.GetCPU().IsOpComplete());

    UpdateAllWidgets();
}

void MainWindow::SetFramerate(unsigned framerate)
{
    m_framerate = framerate;
    m_renderingTimer->stop();
    m_renderingTimer->setInterval(m_mode == Mode::UNLIMITED ? 0 : 1000 / framerate);
    m_renderingTimer->start();
}

void MainWindow::Reset()
{
    m_bus.Reset();

    UpdateAllWidgets();
}

void MainWindow::UpdateAllWidgets()
{
    if (m_renderWidget)
        m_renderWidget->Update();
    
    if (m_disassemblyWidget)
        m_disassemblyWidget->Update();

    if (m_oamWidget)
        m_oamWidget->Update();

    if (m_paletteWidget)
        m_paletteWidget->Update();

    if (m_debugWidget)
        m_debugWidget->Update();
}

void MainWindow::ToggleDebug()
{
    if (m_debugWidget)
    {
        m_debugWidget->setEnabled(false);
        m_renderDebugLayout->removeWidget(m_debugWidget.get());
        m_debugWidget.reset();
    }
    else
    {
        m_debugWidget = std::make_unique<DebugWidget>(m_bus, m_mode);
        m_debugWidget->setEnabled(true);
        m_renderDebugLayout->addWidget(m_debugWidget.get());
    }
    m_toggleDebugAction->setChecked((bool)m_debugWidget);
}

void MainWindow::TogglePalette()
{
    if (m_paletteWidget)
    {
        m_paletteWidget->setEnabled(false);
        m_mainLayout->removeWidget(m_paletteWidget.get());
        m_paletteWidget.reset();
    }
    else
    {
        m_paletteWidget = std::make_unique<PaletteWidget>(m_bus);
        m_paletteWidget->setEnabled(true);
        m_mainLayout->addWidget(m_paletteWidget.get());
    }
    m_togglePaletteAction->setChecked((bool)m_paletteWidget);
}

void MainWindow::ToggleDisassembly()
{
    if (m_disassemblyWidget)
    {
        m_disassemblyWidget->setEnabled(false);
        m_mainLayout->removeWidget(m_disassemblyWidget.get());
        m_disassemblyWidget.reset();
    }
    else
    {
        m_disassemblyWidget = std::make_unique<DisassemblyWidget>(m_bus, NB_LINES_DISASSEMBLY);
        m_disassemblyWidget->setEnabled(true);
        m_mainLayout->addWidget(m_disassemblyWidget.get());
    }
    m_toggleDisassemblyAction->setChecked((bool)m_disassemblyWidget);
}

void MainWindow::ToggleOAM()
{
    if (m_oamWidget)
    {
        m_oamWidget->setEnabled(false);
        m_mainLayout->removeWidget(m_oamWidget.get());
        m_oamWidget.reset();
    }
    else
    {
        m_oamWidget = std::make_unique<OAMWidget>(m_bus, NB_LINES_DISASSEMBLY);
        m_oamWidget->setEnabled(true);
        m_mainLayout->addWidget(m_oamWidget.get());
    }
    m_toggleOAMAction->setChecked((bool)m_oamWidget);
}

void MainWindow::CreateAllExtraActions()
{
    auto extraMenu = menuBar()->addMenu(tr("&Extra"));

    m_toggleDebugAction = std::make_unique<QAction>(tr("&Debug"), this);
    m_toggleDebugAction->setCheckable(true);
    connect(m_toggleDebugAction.get(), &QAction::triggered, this, &MainWindow::ToggleDebug);
    extraMenu->addAction(m_toggleDebugAction.get());

    m_toggleDisassemblyAction = std::make_unique<QAction>(tr("Di&sassembly"), this);
    m_toggleDisassemblyAction->setCheckable(true);
    connect(m_toggleDisassemblyAction.get(), &QAction::triggered, this, &MainWindow::ToggleDisassembly);
    extraMenu->addAction(m_toggleDisassemblyAction.get());

    m_toggleOAMAction = std::make_unique<QAction>(tr("&OAM"), this);
    m_toggleOAMAction->setCheckable(true);
    connect(m_toggleOAMAction.get(), &QAction::triggered, this, &MainWindow::ToggleOAM);
    extraMenu->addAction(m_toggleOAMAction.get());

    m_togglePaletteAction = std::make_unique<QAction>(tr("&Palette"), this);
    m_togglePaletteAction->setCheckable(true);
    connect(m_togglePaletteAction.get(), &QAction::triggered, this, &MainWindow::TogglePalette);
    extraMenu->addAction(m_togglePaletteAction.get());
}

void MainWindow::UpdateStatus()
{
    auto tick = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_lastUpdateTime).count();

    double fps = 0;
    if (diff > 0)
        fps = 1000.0 / diff;

    m_lastUpdateTime = tick;
    m_accumulator += (unsigned)fps;

    constexpr unsigned maxCount = 30;

    if (++m_fpsCounterCount == maxCount)
    {
        char label[50];
        std::sprintf(label, "FPS: %u", (unsigned)(m_accumulator / maxCount));
        m_statusBarFPS->setText(label);
        m_accumulator = 0;
        m_fpsCounterCount = 0;
    }
}

void MainWindow::SetNormalMode()
{
    m_mode = Mode::NORMAL;
    SetFramerate(m_framerate);
}

void MainWindow::SetUnlimittedMode()
{
    m_mode = Mode::UNLIMITED;
    SetFramerate(m_framerate);
}

void MainWindow::UpdateModes()
{
    m_setNormalMode->setChecked(m_mode == Mode::NORMAL);
    m_setUnlimittedMode->setChecked(m_mode == Mode::UNLIMITED);
}