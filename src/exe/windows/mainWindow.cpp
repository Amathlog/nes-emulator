#include <exe/windows/mainWindow.h>
#include <core/processor2C02.h>
#include <iostream>
#include <cstring>

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
    connect(m_stepButton.get(), &QPushButton::released, this, &MainWindow::Step);

    m_mainLayout->addWidget(m_graphicView.get());

    m_buttonLayout->addWidget(m_helloButton.get());
    m_buttonLayout->addWidget(m_stepButton.get());

    m_mainLayout->addLayout(m_buttonLayout.get());

    m_renderingTimer = std::make_unique<QTimer>();
    connect(m_renderingTimer.get(), &QTimer::timeout, this, &MainWindow::UpdateImage);

    m_disassemblyWidget = std::make_unique<DisassemblyWidget>(m_bus, 25);
    m_mainLayout->addWidget(m_disassemblyWidget.get());

    SetFramerate(DEFAULT_FRAMERATE);
}

void MainWindow::CreatePalette()
{
    std::vector<int> colors = {
        84, 84, 84, 0, 30, 116, 8, 16, 144, 48, 0, 136, 68, 0, 100, 92, 0, 48, 84, 4, 0, 60, 24, 0, 32, 42, 0, 8, 58, 0, 0, 64, 0, 0, 60, 0, 0, 50, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        152, 150, 152, 8, 76, 196, 48, 50, 236, 92, 30, 228, 136, 20, 176, 160, 20, 100, 152, 34, 32, 120, 60, 0, 84, 90, 0, 40, 114, 0, 8, 124, 0, 0, 118, 40, 0, 102, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        236, 238, 236, 76, 154, 236, 120, 124, 236, 176, 98, 236, 228, 84, 236, 236, 88, 180, 236, 106, 100, 212, 136, 32, 160, 170, 0, 116, 196, 0, 76, 208, 32, 56, 204, 108, 56, 180, 204, 60, 60, 60, 0, 0, 0, 0, 0, 0,
        236, 238, 236, 168, 204, 236, 188, 188, 236, 212, 178, 236, 236, 174, 236, 236, 174, 212, 236, 180, 176, 228, 196, 144, 204, 210, 120, 180, 222, 120, 168, 226, 144, 152, 226, 180, 160, 214, 228, 160, 162, 160, 0, 0, 0, 0, 0, 0,
    };

    m_renderedImage->setColorCount((int)colors.size() / 3);

    for (int i = 0; i < (int)colors.size() / 3; ++i)
    {
        QRgb value = qRgb(colors[3 * i], colors[3 * i + 1], colors[3 * i + 2]);
        m_renderedImage->setColor(i, value);
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
}

void MainWindow::SetFramerate(unsigned framerate)
{
    m_framerate = framerate;
    m_renderingTimer->stop();
    m_renderingTimer->setInterval(m_mode == Mode::UNLIMITED ? 0 : 1000 / framerate);
    m_renderingTimer->start();
}