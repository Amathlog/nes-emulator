#include <exe/windows/debugWidget.h>
#include <exe/windows/mainWindow.h>

using NesEmulatorExe::DebugWidget;

DebugWidget::DebugWidget(NesEmulator::Bus& bus, Mode& mode, QWidget* parent)
    : QWidget(parent)
    , m_bus(bus)
    , m_mode(mode)
{
    m_buttonLayout = std::make_unique<QHBoxLayout>();

    m_stepButton = std::make_unique<QPushButton>("Step");

    m_breakButton = std::make_unique<QPushButton>("Break");
    connect(m_breakButton.get(), &QPushButton::released, this, &DebugWidget::Break);

    m_buttonLayout->addWidget(m_breakButton.get());
    m_buttonLayout->addWidget(m_stepButton.get());

    // Run to
    m_runToLayout = std::make_unique<QHBoxLayout>();
    m_hexPrefix = std::make_unique<QLabel>("0x");
    m_pcEdit = std::make_unique<QLineEdit>();
    m_runToButton = std::make_unique<QPushButton>("Run to");
    connect(m_runToButton.get(), &QPushButton::released, this, &DebugWidget::SetRunTo);

    m_runToLayout->addWidget(m_hexPrefix.get());
    m_runToLayout->addWidget(m_pcEdit.get());
    m_runToLayout->addWidget(m_runToButton.get());
    m_buttonLayout->addLayout(m_runToLayout.get());

    setLayout(m_buttonLayout.get());

    Update();
}

bool DebugWidget::ShouldBreak()
{
    return m_breakAddress <= 0xFFFF && m_bus.GetCPU().GetPC() == (uint16_t)m_breakAddress;
}

void DebugWidget::SetRunTo()
{
    std::string addrString = m_pcEdit->text().toStdString();
    uint16_t addr = (uint16_t)(std::stoi(addrString, nullptr, 16) & 0x0000FFFF);
    m_breakAddress = addr;
    m_mode = Mode::NORMAL;
}

void DebugWidget::Update()
{
    if (m_stepButton->isEnabled() ^ (m_mode == Mode::STEP))
    {
        m_stepButton->setEnabled(m_mode == Mode::STEP);
        m_breakButton->setText(m_mode == Mode::STEP ? "Continue" : "Break");
    }
}

void DebugWidget::Break()
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
    m_breakAddress = 1 << 24;

    Update();
}