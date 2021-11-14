#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <core/bus.h>
#include <memory>

namespace NesEmulatorExe
{
    class OAMWidget : public QWidget
    {
        //Q_OBJECT

    public:
        OAMWidget(NesEmulator::Bus& bus, unsigned nbLines, QWidget* parent = nullptr);
        virtual ~OAMWidget() {};

        void Update();

    private:
        NesEmulator::Bus& m_bus;
        std::unique_ptr<QVBoxLayout> m_mainLayout;
        std::vector<std::unique_ptr<QLabel>> m_oamLines;
    };
}