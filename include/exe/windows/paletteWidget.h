#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QPushButton>
#include <QVBoxLayout>
#include <qgraphicsitem.h>
#include <memory>
#include <array>

#include <core/bus.h>

namespace NesEmulatorExe
{
    class PaletteWidget : public QWidget
    {
    public:
        PaletteWidget(NesEmulator::Bus& bus, QWidget* parent = nullptr);
        virtual ~PaletteWidget() = default;

        void Update();

    private slots:
        void SwitchPalette();

    private:
        NesEmulator::Bus& m_bus;

        static inline constexpr unsigned nbPalettes = 8;
        static inline constexpr unsigned nbNamedTables = 2;

        std::unique_ptr<QVBoxLayout> m_mainLayout;
        std::unique_ptr<QPushButton> m_switchPaletteButton;
        std::array<std::unique_ptr<QImage>, nbPalettes> m_paletteImages;
        std::array<QGraphicsPixmapItem*, nbPalettes> m_palettePixmaps;
        std::array<std::unique_ptr<QImage>, nbNamedTables> m_namedTableImages;
        std::array<QGraphicsPixmapItem*, nbNamedTables> m_namedTablePixmaps;
        std::unique_ptr<QGraphicsScene> m_scene;
        std::unique_ptr<QGraphicsView> m_graphicView;

        unsigned m_currentSelectedPalette = 0;
    };
}