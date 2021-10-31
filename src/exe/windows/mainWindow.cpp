#include "exe/windows/paletteWidget.h"
#include <exe/windows/mainWindow.h>
#include <core/processor2C02.h>
#include <core/palette.h>
#include <iostream>
#include <cstring>
#include <memory>

using NesEmulatorExe::MainWindow;
using NesEmulatorExe::DisassemblyWidget;


constexpr unsigned DEFAULT_FRAMERATE = 60;

MainWindow::MainWindow(NesEmulator::Bus& bus, Mode mode, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
    , m_mode(mode)
{
    m_mainLayout = std::make_unique<QHBoxLayout>(this);
    m_buttonLayout = std::make_unique<QVBoxLayout>();

    m_renderedImage = std::make_unique<QImage>(m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetWidth(), 
    m_bus.GetPPU().GetHeight(), m_bus.GetPPU().GetWidth(), QImage::Format::Format_Indexed8);
    
    CreatePalette();

    m_scene = std::make_unique<QGraphicsScene>(0, 0, m_bus.GetPPU().GetWidth(), m_bus.GetPPU().GetHeight());
    m_graphicView = std::make_unique<QGraphicsView>(m_scene.get());
    m_imagePixmap = m_scene->addPixmap(QPixmap::fromImage(*m_renderedImage));

    m_graphicView->scale(2, 2);
    
    m_helloButton = std::make_unique<QPushButton>("Hello");
    connect(m_helloButton.get(), &QPushButton::released, this, &MainWindow::HandleButton);

    m_stepButton = std::make_unique<QPushButton>("Step");
    m_stepButton->setEnabled(m_mode == Mode::STEP);
    connect(m_stepButton.get(), &QPushButton::released, this, &MainWindow::Step);

    m_breakButton = std::make_unique<QPushButton>("Break");
    connect(m_breakButton.get(), &QPushButton::released, this, &MainWindow::Break);

    m_mainLayout->addWidget(m_graphicView.get());

    m_buttonLayout->addWidget(m_helloButton.get());
    m_buttonLayout->addWidget(m_breakButton.get());
    m_buttonLayout->addWidget(m_stepButton.get());

    m_mainLayout->addLayout(m_buttonLayout.get());

    m_renderingTimer = std::make_unique<QTimer>();
    connect(m_renderingTimer.get(), &QTimer::timeout, this, &MainWindow::UpdateImage);

    m_disassemblyWidget = std::make_unique<DisassemblyWidget>(m_bus, 25);
    m_mainLayout->addWidget(m_disassemblyWidget.get());

    m_paletteWidget = std::make_unique<PaletteWidget>(m_bus);
    m_mainLayout->addWidget(m_paletteWidget.get());

    SetFramerate(DEFAULT_FRAMERATE);
}

void MainWindow::CreatePalette()
{
    m_renderedImage->setColorCount(NesEmulator::Palette::paletteSize);

    for (uint8_t i = 0; i < NesEmulator::Palette::paletteSize; ++i)
    {
        m_renderedImage->setColor(i, NesEmulator::Palette::GetRGBColor(i));
    }
}

void MainWindow::HandleButton()
{
    m_bus.GetPPU().RandomizeScreen();
}

void MainWindow::UpdateImage()
{
    memcpy(m_renderedImage->bits(), m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetWidth()*m_bus.GetPPU().GetHeight());
    m_imagePixmap->setPixmap(QPixmap::fromImage(*m_renderedImage));
    if (m_mode == Mode::NORMAL)
    {
        // Run 29781 clocks
        for (unsigned i = 0; i < 29781; ++i)
        {
            m_bus.Clock();
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

    m_disassemblyWidget->Update();
    m_paletteWidget->Update();
}

void MainWindow::Break()
{
    switch (m_mode)
    {
    case Mode::NORMAL:
        m_mode = Mode::STEP;
        break;
    case Mode::STEP:
        m_mode = Mode::NORMAL;
    default:
        break;
    }
    m_stepButton->setEnabled(m_mode == Mode::STEP);
}

void MainWindow::SetFramerate(unsigned framerate)
{
    m_framerate = framerate;
    m_renderingTimer->stop();
    m_renderingTimer->setInterval(m_mode == Mode::UNLIMITED ? 0 : 1000 / framerate);
    m_renderingTimer->start();
}