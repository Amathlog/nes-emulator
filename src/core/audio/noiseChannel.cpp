#include "MyTonic.h"
#include "core/constants.h"
#include <core/audio/noiseChannel.h>

using NesEmulator::NoiseRegister;
using NesEmulator::NoiseChannel;

//////////////////////////////////////////////////////
// Noise Register
//////////////////////////////////////////////////////

void NoiseRegister::Reset()
{
    enveloppeLoop = 0;
    constantVolume = 0;
    volumeEnveloppe = 0;
    mode = 0;
    noisePeriod = 0;
    lengthCounterLoad = 0;
    noisePeriodChanged = true;
}

void NoiseRegister::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    visitor.WriteValue(enveloppeLoop);
    visitor.WriteValue(constantVolume);
    visitor.WriteValue(volumeEnveloppe);
    visitor.WriteValue(mode);
    visitor.WriteValue(noisePeriod);
    visitor.WriteValue(lengthCounterLoad);
}

void NoiseRegister::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    visitor.ReadValue(enveloppeLoop);
    visitor.ReadValue(constantVolume);
    visitor.ReadValue(volumeEnveloppe);
    visitor.ReadValue(mode);
    visitor.ReadValue(noisePeriod);
    visitor.ReadValue(lengthCounterLoad);
}

void NoiseRegister::SetNoisePeriod(uint8_t index, Mode mode)
{
    auto noisePeriodTable = mode == Mode::PAL ? Cst::APU_NOISE_PERIOD_PAL : Cst::APU_NOISE_PERIOD_NTSC;
    noisePeriod = noisePeriodTable[index & 0x0F];
    noisePeriodChanged = true;
}

//////////////////////////////////////////////////////
// Noise Channel
//////////////////////////////////////////////////////

NoiseChannel::NoiseChannel(Tonic::Synth&)
{
    m_wave = MyNoise();
}

void NoiseChannel::Clock(bool isEnabled)
{
    bool halt = m_register.enveloppeLoop;
    if (!isEnabled)
    {
        m_lengthCounter = 0;
    }
    else if (m_lengthCounter > 0 && !halt) 
    {
        m_lengthCounter--;
    }
}

void NoiseChannel::ClockEnveloppe()
{
    m_enveloppe.Clock(m_register.enveloppeLoop);
}

void NoiseChannel::Update(double cpuFrequency, Tonic::Synth& synth)
{
    float temp;
    if (m_lengthCounter > 0 && m_register.noisePeriod > 0)
    {
        temp = m_enveloppe.output > 1 ? (float)(m_enveloppe.output - 1) / 16.0f : 0.0f;
    }
    else
    {
        temp = 0.0f;
    }

    if (m_currentOutput != temp)
    {
        m_currentOutput = temp;
        m_wave.setVolume(temp);
    }
    
    if (m_register.noisePeriodChanged && m_register.noisePeriod > 0)
    {
        m_register.noisePeriodChanged = false;
        double newFrequency = cpuFrequency / ((double)m_register.noisePeriod);
        m_wave.setFreq((float)newFrequency);
    }
}

void NoiseChannel::SampleRequested()
{
    //m_wave.setOutput(m_currentOutput);
}

void NoiseChannel::Reset()
{
    m_register.Reset();
    m_enveloppe.Reset();
    m_wave.reset();
    m_lengthCounter = 0;
    m_timer = 0;
    m_currentOutput = 0.0f;
}

void NoiseChannel::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_register.SerializeTo(visitor);
    m_enveloppe.SerializeTo(visitor);
    visitor.WriteValue(m_lengthCounter);
    visitor.WriteValue(m_timer);
}

void NoiseChannel::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_register.DeserializeFrom(visitor);
    m_enveloppe.DeserializeFrom(visitor);
    visitor.ReadValue(m_lengthCounter);
    visitor.ReadValue(m_timer);
}

void NoiseChannel::ReloadCounter()
{
    m_lengthCounter = Cst::APU_LENGTH_TABLE[m_register.lengthCounterLoad];
}