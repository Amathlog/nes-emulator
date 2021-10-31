#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>
#include <exe/windows/disassemblyWidget.h>

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

        void SetFramerate(unsigned framerate);
        unsigned GetFramerate() const { return m_framerate; }

        Mode GetCurrentMode() const { return m_mode; }

    private slots:
        void HandleButton();
        void UpdateImage();
        void Step();

    private:
        void CreatePalette();

        NesEmulator::Bus& m_bus;
        std::unique_ptr<QHBoxLayout> m_mainLayout;
        std::unique_ptr<QVBoxLayout> m_buttonLayout;
        std::unique_ptr<QImage> m_renderedImage;
        std::unique_ptr<QGraphicsScene> m_scene;
        std::unique_ptr<QGraphicsView> m_graphicView;
        std::unique_ptr<QPushButton> m_helloButton;
        std::unique_ptr<QPushButton> m_stepButton;
        QGraphicsPixmapItem* m_imagePixmap = nullptr;
        std::unique_ptr<QTimer> m_renderingTimer;
        std::unique_ptr<DisassemblyWidget> m_disassemblyWidget;

        unsigned m_framerate;
        Mode m_mode = Mode::NORMAL;
    };
}