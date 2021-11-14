#include <exe/windows/disassemblyWidget.h>
#include <core/utils/busVisitor.h>
#include <core/utils/disassembly.h>
#include <core/utils/utils.h>
#include <sstream>

using NesEmulatorExe::DisassemblyWidget;
using NesEmulator::Utils::BusReadVisitor;
using NesEmulator::Utils::Hex;

DisassemblyWidget::DisassemblyWidget(NesEmulator::Bus& bus, unsigned nbDisassemblyLines, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
{
    m_disassemblyLines.resize(nbDisassemblyLines);
    m_mainLayout = std::make_unique<QVBoxLayout>(this);

    m_statusLabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_statusLabel.get());

    m_mainLayout->addSpacing(5);

    m_ALabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_ALabel.get());

    m_XLabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_XLabel.get());

    m_YLabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_YLabel.get());

    m_PCLabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_PCLabel.get());

    m_stackLabel = std::make_unique<QLabel>();
    m_mainLayout->addWidget(m_stackLabel.get());

    m_mainLayout->addSpacing(20);

    for (unsigned i = 0; i < nbDisassemblyLines; ++i)
    {
        m_disassemblyLines[i] = std::make_unique<QLabel>();
        m_mainLayout->addWidget(m_disassemblyLines[i].get());
    }

    setFixedWidth(170);

    Update();
}

void DisassemblyWidget::Update()
{
    UpdateStatus();
    UpdateRegistersAndAddr();
    UpdateDisassembly();
}

void DisassemblyWidget::UpdateDisassembly()
{
    uint16_t maxBytesToRead = (uint16_t)m_disassemblyLines.size() * 3u;
    // int16_t offset = -(maxBytesToRead / 2);
    uint16_t pc = m_bus.GetCPU().GetPC();
    // pc += offset;
    BusReadVisitor visitor(m_bus, pc, pc + maxBytesToRead);
    uint16_t indexOfPC = 0;
    std::vector<std::string> disassemblyLines = NesEmulator::Utils::Disassemble(visitor, pc, indexOfPC);
    for (std::size_t i = 0; i < m_disassemblyLines.size(); ++i)
    {
        m_disassemblyLines[i]->setText(disassemblyLines[i].c_str());
        if (i == indexOfPC)
        {
            m_disassemblyLines[i]->setStyleSheet("QLabel {color : blue; }");
        }
        else
        {
            m_disassemblyLines[i]->setStyleSheet("QLabel {color : black; }");
        }
    }
}

void DisassemblyWidget::UpdateRegistersAndAddr()
{
    std::stringstream aLabel;
    Hex(aLabel, m_bus.GetCPU().GetA(), 2, "A: ");
    m_ALabel->setText(aLabel.str().c_str());

    std::stringstream xLabel;
    Hex(xLabel, m_bus.GetCPU().GetX(), 2, "X: ");
    m_XLabel->setText(xLabel.str().c_str());

    std::stringstream yLabel;
    Hex(yLabel, m_bus.GetCPU().GetY(), 2, "Y: ");
    m_YLabel->setText(yLabel.str().c_str());

    std::stringstream pcLabel;
    Hex(pcLabel, m_bus.GetCPU().GetPC(), 4, "PC: 0x");
    m_PCLabel->setText(pcLabel.str().c_str());

    std::stringstream spLabel;
    Hex(spLabel, m_bus.GetCPU().GetSP(), 4, "SP: 0x");
    m_stackLabel->setText(spLabel.str().c_str());
}

void DisassemblyWidget::UpdateStatus()
{
    NesEmulator::Status status = m_bus.GetCPU().GetStatus();

    std::stringstream res;

    auto setLabel = [&res](uint8_t s, const char* name)
    {
        res << "<font color=\"";
        res << (s == 0 ? "red" : "green");
        res << "\">";
        res << name;
        res << "</font> ";
    };

    setLabel(status.N, "N");
    setLabel(status.V, "V");
    setLabel(status.U, "U");
    setLabel(status.B, "B");
    setLabel(status.D, "D");
    setLabel(status.I, "I");
    setLabel(status.Z, "Z");
    setLabel(status.C, "C");

    m_statusLabel->setText(res.str().c_str());
}

