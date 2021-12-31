#include <core/audio/pulseChannel.h>
#include <core/constants.h>
#include <string>

using NesEmulator::PulseChannel;
using NesEmulator::PulseRegister;

//////////////////////////////////////////////////////////////
// PULSE REGISTER
//////////////////////////////////////////////////////////////
void PulseRegister::Reset()
{
    duty = 0;
    enveloppeLoop = 0;
    constantVolume = 0;
    volumeEnveloppe = 0;
    sweepEnable = 0;
    sweepPeriod = 0;
    sweepNegate = 0;
    sweepShift = 0;
    timer = 0;
    lengthCounterReload = 0;
}

void PulseRegister::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    visitor.WriteValue(duty);
    visitor.WriteValue(enveloppeLoop);
    visitor.WriteValue(constantVolume);
    visitor.WriteValue(volumeEnveloppe);
    visitor.WriteValue(sweepEnable);
    visitor.WriteValue(sweepPeriod);
    visitor.WriteValue(sweepNegate);
    visitor.WriteValue(sweepShift);
    visitor.WriteValue(timer);
    visitor.WriteValue(lengthCounterReload);
}

void PulseRegister::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    visitor.ReadValue(duty);
    visitor.ReadValue(enveloppeLoop);
    visitor.ReadValue(constantVolume);
    visitor.ReadValue(volumeEnveloppe);
    visitor.ReadValue(sweepEnable);
    visitor.ReadValue(sweepPeriod);
    visitor.ReadValue(sweepNegate);
    visitor.ReadValue(sweepShift);
    visitor.ReadValue(timer);
    visitor.ReadValue(lengthCounterReload);
}

//////////////////////////////////////////////////////////////
// PULSE CHANNEL
//////////////////////////////////////////////////////////////

std::string PulseChannel::GetDutyCycleParameterName()
{
    return std::string("dutyCyclePulse") + std::to_string(m_number);
}

std::string PulseChannel::GetFrequencyParameterName()
{
    return std::string("freqPulse") + std::to_string(m_number);
}

std::string PulseChannel::GetOutputParameterName()
{
    return std::string("outputPulse") + std::to_string(m_number);
}

PulseChannel::PulseChannel(Tonic::Synth& synth, int number)
    : m_number(number)
{
    Tonic::ControlGenerator controlDutyPulse = synth.addParameter(GetDutyCycleParameterName());
    Tonic::ControlGenerator controlFreqPulse = synth.addParameter(GetFrequencyParameterName());
    Tonic::ControlGenerator controlOutputPulse = synth.addParameter(GetOutputParameterName());
    m_wave = controlOutputPulse * Tonic::RectWave().freq(controlFreqPulse).pwm(controlDutyPulse);
}

void PulseChannel::Update(double cpuFrequency, Tonic::Synth& synth)
{
    double newEnableValue = m_lengthCounter > 0 ? 1.0 : 0.0;
    double newFrequency = cpuFrequency / (16.0 * (double)(m_register.timer + 1));
    double newDutyCycle = 0.0;
    switch (m_register.duty)
    {
        case 0:
            newDutyCycle = 0.125;
            break;
        case 1:
            newDutyCycle = 0.25;
            break;
        case 2:
            newDutyCycle = 0.5;
            break;
        case 3:
            newDutyCycle = 0.75;
            break;
    }

    if (newFrequency != m_frequency || newDutyCycle != m_dutyCycle || newEnableValue != m_enableValue)
    {
        m_frequency = newFrequency;
        m_dutyCycle = newDutyCycle;
        m_enableValue = newEnableValue;
        synth.setParameter(GetDutyCycleParameterName(), newDutyCycle);
        synth.setParameter(GetFrequencyParameterName(), newFrequency);
        synth.setParameter(GetOutputParameterName(), newEnableValue);
    }
}

void PulseChannel::SerializeTo(Utils::IWriteVisitor &visitor) const
{
    m_register.SerializeTo(visitor);

    visitor.WriteValue(m_frequency);
    visitor.WriteValue(m_dutyCycle);
    visitor.WriteValue(m_enableValue);
    visitor.WriteValue(m_lengthCounter);
}

void PulseChannel::DeserializeFrom(Utils::IReadVisitor &visitor)
{
    m_register.DeserializeFrom(visitor);

    visitor.ReadValue(m_frequency);
    visitor.ReadValue(m_dutyCycle);
    visitor.ReadValue(m_enableValue);
    visitor.ReadValue(m_lengthCounter);
}

void PulseChannel::Clock(bool isEnabled)
{
    if (!isEnabled)
    {
        m_lengthCounter = 0;
        return;
    }
    else if(m_lengthCounter > 0)
    {
        m_lengthCounter--;
    }
}

void PulseChannel::Reset()
{
    m_register.Reset();

    m_frequency = 0.0;
    m_dutyCycle = 0.0;
    m_enableValue = 0.0;
    m_lengthCounter = 0;
}

void PulseChannel::ReloadCounter()
{
    m_lengthCounter = Cst::APU_LENGTH_TABLE[m_register.lengthCounterReload];
}