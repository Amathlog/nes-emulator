#pragma once

#include "core/bus.h"
#include <QWidget>
#include <QImage>
#include <QGraphicsView>
#include <memory>
#include <qboxlayout.h>
#include <QTimer>
#include <QGraphicsPixmapItem>

namespace NesEmulatorExe 
{

class RenderWidget : public QWidget
{
public:
    RenderWidget(NesEmulator::Bus& bus, QWidget* parent = nullptr);
    virtual ~RenderWidget() = default;

    void Update();

private:
    NesEmulator::Bus& m_bus;

    std::unique_ptr<QVBoxLayout> m_mainLayout;
    std::unique_ptr<QImage> m_renderedImage;
    std::unique_ptr<QGraphicsScene> m_scene;
    std::unique_ptr<QGraphicsView> m_graphicView;
    QGraphicsPixmapItem* m_imagePixmap = nullptr;
};

}