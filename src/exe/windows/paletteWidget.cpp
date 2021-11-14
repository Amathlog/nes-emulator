#include <cstring>
#include <exe/windows/paletteWidget.h>
#include <core/palette.h>
#include <memory>
#include <qpushbutton.h>
#include <cstring>
#include <exe/utils.h>

using NesEmulatorExe::PaletteWidget;

PaletteWidget::PaletteWidget(NesEmulator::Bus& bus, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
{
    m_mainLayout = std::make_unique<QVBoxLayout>(this);

    m_switchPaletteButton = std::make_unique<QPushButton>("Switch");
    connect(m_switchPaletteButton.get(), &QPushButton::released, this, &PaletteWidget::SwitchPalette);
    m_mainLayout->addWidget(m_switchPaletteButton.get());

    m_scene = std::make_unique<QGraphicsScene>(0, 0, 266, 256);
    m_graphicView = std::make_unique<QGraphicsView>(m_scene.get());

    unsigned i = 0;
    for (auto& paletteImage : m_paletteImages)
    {
        paletteImage = std::make_unique<QImage>(20, 10, QImage::Format::Format_Indexed8);
        NesEmulatorExe::CreatePalette(paletteImage.get());
        m_palettePixmaps[i++] = m_scene->addPixmap(QPixmap::fromImage(*paletteImage));
    }

    i = 0;
    for (auto& namedTableImage : m_namedTableImages)
    {
        namedTableImage = std::make_unique<QImage>(128, 128, QImage::Format::Format_Indexed8);
        NesEmulatorExe::CreatePalette(namedTableImage.get());
        m_namedTablePixmaps[i++] = m_scene->addPixmap(QPixmap::fromImage(*namedTableImage));
    }

    m_graphicView->scale(2, 2);

    m_mainLayout->addWidget(m_graphicView.get());


    setFixedSize(m_scene->width() * 2 + 40, m_scene->height() * 2 + 60);

    Update();
}

void PaletteWidget::SwitchPalette()
{
    m_currentSelectedPalette = (m_currentSelectedPalette + 1) % nbPalettes;
    Update();
}

void PaletteWidget::Update()
{
    for (unsigned i = 0; i < nbPalettes; ++i)
    {
        unsigned quarterWidth = m_paletteImages[i]->width() / 4;
        for (unsigned idx = 0; idx < 4; ++idx)
        {
            uint8_t color = m_bus.GetPPU().GetColorFromPaletteRam(i, idx);
            uchar* bits = m_paletteImages[i]->bits();
            bits = bits;
            for (int scanline = 0; scanline < m_paletteImages[i]->height(); ++scanline)
            {
                memset(m_paletteImages[i]->scanLine(scanline) + idx * quarterWidth, color, quarterWidth);
            }
        }
        m_palettePixmaps[i]->setPixmap(QPixmap::fromImage(*m_paletteImages[i]));
        m_palettePixmaps[i]->setOffset(30 * i, (i == m_currentSelectedPalette) ? 10 : 0);
    }

    for (unsigned i = 0; i < nbNamedTables; ++i)
    {
        m_bus.GetPPU().FillFromPatternTable(i, m_currentSelectedPalette, m_namedTableImages[i]->bits());
        m_namedTablePixmaps[i]->setPixmap(QPixmap::fromImage(*m_namedTableImages[i]));
        m_namedTablePixmaps[i]->setOffset(138 * i, 40);
    }
}