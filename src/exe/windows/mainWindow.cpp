#include "exe/windows/paletteWidget.h"
#include "exe/windows/renderWidget.h"
#include <exe/windows/mainWindow.h>
#include <core/processor2C02.h>
#include <core/palette.h>
#include <iostream>
#include <cstring>
#include <memory>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <string>

using NesEmulatorExe::MainWindow;
using NesEmulatorExe::DisassemblyWidget;


constexpr unsigned DEFAULT_FRAMERATE = 60;

MainWindow::MainWindow(NesEmulator::Bus& bus, Mode mode, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
{
    m_mainLayout = std::make_unique<QHBoxLayout>(this);
    m_buttonLayout = std::make_unique<QVBoxLayout>();

    m_stepButton = std::make_unique<QPushButton>("Step");
    m_stepButton->setEnabled(m_mode == Mode::STEP);
    connect(m_stepButton.get(), &QPushButton::released, this, &MainWindow::Step);

    m_breakButton = std::make_unique<QPushButton>("Break");
    connect(m_breakButton.get(), &QPushButton::released, this, &MainWindow::Break);

    m_renderWidget = std::make_unique<RenderWidget>(m_bus);

    m_mainLayout->addWidget(m_renderWidget.get());

    m_resetButton = std::make_unique<QPushButton>("Reset");
    connect(m_resetButton.get(), &QPushButton::released, this, &MainWindow::Reset);

    m_buttonLayout->addWidget(m_resetButton.get());
    m_buttonLayout->addWidget(m_breakButton.get());
    m_buttonLayout->addWidget(m_stepButton.get());

    // Run to
    m_runToLayout = std::make_unique<QHBoxLayout>();
    m_hexPrefix = std::make_unique<QLabel>("0x");
    m_pcEdit = std::make_unique<QLineEdit>();
    m_runToButton = std::make_unique<QPushButton>("Run to");
    connect(m_runToButton.get(), &QPushButton::released, this, &MainWindow::SetRunTo);

    m_runToLayout->addWidget(m_hexPrefix.get());
    m_runToLayout->addWidget(m_pcEdit.get());
    m_runToLayout->addWidget(m_runToButton.get());
    m_buttonLayout->addLayout(m_runToLayout.get());

    m_mainLayout->addLayout(m_buttonLayout.get());

    m_disassemblyWidget = std::make_unique<DisassemblyWidget>(m_bus, 25);
    m_mainLayout->addWidget(m_disassemblyWidget.get());

    m_paletteWidget = std::make_unique<PaletteWidget>(m_bus);
    m_mainLayout->addWidget(m_paletteWidget.get());

    m_renderingTimer = std::make_unique<QTimer>();
    connect(m_renderingTimer.get(), &QTimer::timeout, this, &MainWindow::Update);

    SetFramerate(DEFAULT_FRAMERATE);

    SetMode(mode);
}

void MainWindow::HandleButton()
{
    m_bus.GetPPU().RandomizeScreen();
}

void MainWindow::Update()
{
    m_renderWidget->Update();
    if (m_mode == Mode::NORMAL)
    {
        // Run 29781 clocks
        for (unsigned i = 0; i < 29781; ++i)
        {
            m_bus.Clock();
            if (m_breakAddress <= 0xFFFF && m_bus.GetCPU().GetPC() == (uint16_t)m_breakAddress)
            {
                SetMode(Mode::STEP);
                break;
            }
        }
        m_disassemblyWidget->Update();
        m_paletteWidget->Update();
    }
}

void MainWindow::Step()
{
    do
    {
        m_bus.Clock();
        m_bus.Clock();
        m_bus.Clock();
    } while (!m_bus.GetCPU().IsOpComplete());

    m_renderWidget->Update();
    m_disassemblyWidget->Update();
    m_paletteWidget->Update();
}

void MainWindow::Break()
{
    switch (m_mode)
    {
    case Mode::NORMAL:
        SetMode(Mode::STEP);
        break;
    case Mode::STEP:
        SetMode(Mode::NORMAL);
    default:
        break;
    }
    m_breakAddress = 1 << 24;
}

void MainWindow::SetFramerate(unsigned framerate)
{
    m_framerate = framerate;
    m_renderingTimer->stop();
    m_renderingTimer->setInterval(m_mode == Mode::UNLIMITED ? 0 : 1000 / framerate);
    m_renderingTimer->start();
}

void MainWindow::SetRunTo()
{
    std::string addrString = m_pcEdit->text().toStdString();
    uint16_t addr = (uint16_t)(std::stoi(addrString, nullptr, 16) & 0x0000FFFF);
    m_breakAddress = addr;
    SetMode(Mode::NORMAL);
}

void MainWindow::SetMode(Mode mode)
{
    m_stepButton->setEnabled(mode == Mode::STEP);
    m_breakButton->setText(mode == Mode::STEP ? "Continue" : "Break");
    m_mode = mode;
}

void MainWindow::Reset()
{
    m_bus.Reset();
    m_renderWidget->Update();
    m_disassemblyWidget->Update();
    m_paletteWidget->Update();
}