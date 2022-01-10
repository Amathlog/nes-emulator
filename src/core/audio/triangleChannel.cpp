#include <core/audio/triangleChannel.h>
#include <core/constants.h>

using NesEmulator::TriangleChannel;
using NesEmulator::TriangleRegister;

constexpr const char* frequencyParameterName = "freqTriangle";
constexpr const char* outputParameterName = "outputTriangle";

////////////////////////////////////////////////////////////////////////
// TRIANGLE REGISTER
////////////////////////////////////////////////////////////////////////
void TriangleRegister::Reset()
{
    control = 0;
    linearCounterLoad = 0;
    timer = 0;
    lengthCounterLoad = 0;
}

void TriangleRegister::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    visitor.WriteValue(control);
    visitor.WriteValue(linearCounterLoad);
    visitor.WriteValue(timer);
    visitor.WriteValue(lengthCounterLoad);
}

void TriangleRegister::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    visitor.ReadValue(control);
    visitor.ReadValue(linearCounterLoad);
    visitor.ReadValue(timer);
    visitor.ReadValue(lengthCounterLoad);
}

////////////////////////////////////////////////////////////////////////
// TRIANGLE CHANNEL
////////////////////////////////////////////////////////////////////////
TriangleChannel::TriangleChannel(Tonic::Synth& synth)
{
    Tonic::ControlGenerator controlFreqTriangle = synth.addParameter(frequencyParameterName);
    Tonic::ControlGenerator controlOutputTriangle = synth.addParameter(outputParameterName);
    m_wave = controlOutputTriangle * Tonic::TriangleWave().freq(controlFreqTriangle);
}

void TriangleChannel::Reset()
{
    m_register.Reset();

    m_frequency = 0.0;
    m_enableValue = 0.0;
    m_linearCounter = 0;
    m_lengthCounter = 0;
    m_linearControlFlag = 0;
}

void TriangleChannel::ClockLinear(bool isEnabled)
{
    if (!isEnabled)
    {
        m_linearCounter = 0;
        return;
    }

    if (m_linearControlFlag > 0)
    {
        m_linearCounter = m_register.linearCounterLoad;
    }
    else if (m_linearCounter > 0)
    {
        m_linearCounter--;
    }

    if (m_register.control == 0)
        SetLinearControlFlag(0);
}

void TriangleChannel::ClockLength(bool isEnabled)
{
    if (!isEnabled)
    {
        m_lengthCounter = 0;
    }
    else if (m_lengthCounter > 0)
    {
        m_lengthCounter--;
    }
}

void TriangleChannel::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_register.SerializeTo(visitor);

    visitor.WriteValue(m_frequency);
    visitor.WriteValue(m_lengthCounter);
    visitor.WriteValue(m_linearCounter);
    visitor.WriteValue(m_linearControlFlag);
}

void TriangleChannel::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_register.DeserializeFrom(visitor);

    visitor.ReadValue(m_frequency);
    visitor.ReadValue(m_lengthCounter);
    visitor.ReadValue(m_linearCounter);
    visitor.ReadValue(m_linearControlFlag);
}

void TriangleChannel::Update(double cpuFrequency, Tonic::Synth& synth)
{
    double newFrequency = cpuFrequency / (32.0 * (double)(m_register.timer + 1));
    double newEnableValue = m_linearCounter > 0 && m_lengthCounter > 0;
    if (newFrequency != m_frequency)
    {
        m_frequency = newFrequency;
        synth.setParameter(frequencyParameterName, (float)newFrequency);
    }

    if (newEnableValue != m_enableValue)
    {
        m_enableValue = newEnableValue;
        synth.setParameter(outputParameterName, (float)newEnableValue);
    }
}

void TriangleChannel::ReloadCounter()
{
    m_lengthCounter = Cst::APU_LENGTH_TABLE[m_register.lengthCounterLoad];
}