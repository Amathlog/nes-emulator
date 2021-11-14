#include "core/processor2C02Registers.h"
#include <exe/windows/oamWidget.h>
#include <core/utils/busVisitor.h>
#include <core/utils/disassembly.h>
#include <core/utils/utils.h>
#include <sstream>
#include <iomanip>

using NesEmulatorExe::OAMWidget;
using NesEmulator::Utils::Dec;
using NesEmulator::Utils::Hex;

OAMWidget::OAMWidget(NesEmulator::Bus& bus, unsigned nbLines, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
{
    nbLines = nbLines > 64 ? 64 : nbLines;

    m_oamLines.resize(nbLines);
    m_mainLayout = std::make_unique<QVBoxLayout>(this);

    for (unsigned i = 0; i < nbLines; ++i)
    {
        m_oamLines[i] = std::make_unique<QLabel>();
        m_mainLayout->addWidget(m_oamLines[i].get());
    }

    setFixedWidth(180);

    Update();
}

void OAMWidget::Update()
{
    for (size_t i = 0; i < m_oamLines.size(); ++i)
    {
        std::stringstream text;
        const NesEmulator::OAM& oamData = m_bus.GetPPU().GetRegisters().oam[i];
        text << "(";
        Dec(text, oamData.x, 3, "", ", ");
        Dec(text, oamData.y, 3, "", ") ID: ");
        Hex(text, oamData.tileId, 2, "", " AT: ");
        Hex(text, oamData.attribute, 2);
        m_oamLines[i]->setText(text.str().c_str());
    }
}
