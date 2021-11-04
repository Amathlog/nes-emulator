#include "core/bus.h"
#include <cstdint>
#include <exe/windows/renderWidget.h>
#include <memory>
#include <qboxlayout.h>
#include <qwidget.h>
#include <core/palette.h>
#include <exe/utils.h>

using NesEmulatorExe::RenderWidget;

RenderWidget::RenderWidget(NesEmulator::Bus& bus, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
    
{
    m_mainLayout = std::make_unique<QVBoxLayout>(this);
    m_renderedImage = std::make_unique<QImage>(m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetWidth(), 
                                               m_bus.GetPPU().GetHeight(), m_bus.GetPPU().GetWidth(), 
                                               QImage::Format::Format_Indexed8);
    
    NesEmulatorExe::CreatePalette(m_renderedImage.get());

    m_scene = std::make_unique<QGraphicsScene>(0, 0, m_bus.GetPPU().GetWidth(), m_bus.GetPPU().GetHeight());
    m_graphicView = std::make_unique<QGraphicsView>(m_scene.get());
    m_imagePixmap = m_scene->addPixmap(QPixmap::fromImage(*m_renderedImage));

    m_graphicView->scale(2, 2);

    m_mainLayout->addWidget(m_graphicView.get());
}


void RenderWidget::Update()
{
    // memcpy(m_renderedImage->bits(), m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetWidth()*m_bus.GetPPU().GetHeight());
    // Cheating here, only displaying the tile for a namedtable
    m_bus.GetPPU().FillFromNameTable(0, 0, m_renderedImage->bits());
    m_imagePixmap->setPixmap(QPixmap::fromImage(*m_renderedImage));
}