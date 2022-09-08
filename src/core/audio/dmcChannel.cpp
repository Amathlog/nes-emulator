#include <core/audio/dmcChannel.h>

using NesEmulator::DMCRegister;
using NesEmulator::DMCChannel;

void DMCRegister::Reset()
{
    flags.reg = 0x00;
    directLoad = 0x00;
    sampleAddress = 0x00;
    sampleLength = 0x00;
}

void DMCRegister::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    visitor.WriteValue(flags.reg);
    visitor.WriteValue(directLoad);
    visitor.WriteValue(sampleAddress);
    visitor.WriteValue(sampleLength);
}

void DMCRegister::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    visitor.ReadValue(flags.reg);
    visitor.ReadValue(directLoad);
    visitor.ReadValue(sampleAddress);
    visitor.ReadValue(sampleLength);
}

void DMCChannel::Reset()
{
    m_register.Reset();
}

void DMCChannel::Clock()
{
    // TODO
}

double DMCChannel::GetSample()
{
    return 0.0;
}

void DMCChannel::Update(double cpuFrequency)
{
    // TODO
}

void DMCChannel::WriteData(uint16_t address, uint8_t data)
{
    if (address == 0x4010)
    {
        m_register.flags.reg = data;
        // TODO
    }
    else if (address == 0x4011)
    {
        m_register.directLoad = data;
        // TODO
    }
    else if (address == 0x4012)
    {
        m_register.sampleAddress = data;
        // TODO
    }
    else if (address == 0x4013)
    {
        m_register.sampleLength = data;
        // TODO
    }
}

void DMCChannel::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_register.SerializeTo(visitor);
}

void DMCChannel::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_register.DeserializeFrom(visitor);
}