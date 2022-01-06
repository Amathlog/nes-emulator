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

NoiseChannel::NoiseChannel(Tonic::Synth& synth)
{
    Tonic::ControlParameter volume = synth.addParameter("noiseVolume");
    // Tonic::ControlParameter freq = synth.addParameter("noiseFreq");
    m_wave = volume * Tonic::Noise();// * Tonic::SineWave().freq(freq);
    //m_wave = MyNoise();
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

    if (isEnabled)
    {
        if(--m_timer == -1)
        {
            m_timer = m_register.noisePeriod;
            uint16_t otherFeedback = 0;
            if (m_register.mode == 0)
                otherFeedback = (m_shiftRegister >> 1) & 0x0001;
            else
                otherFeedback = (m_shiftRegister >> 5) & 0x0001;

            uint16_t feedback = ((m_shiftRegister & 0x0001) ^ otherFeedback) & 0x0001;
            m_shiftRegister = (feedback << 14) | (m_shiftRegister >> 1);
        }
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
        // temp = (m_shiftRegister & 0x0001) > 0 ? volume : -volume;
    }
    else
    {
        temp = 0.0f;
    }

    if (m_currentOutput != temp)
    {
        m_currentOutput = temp;
        synth.setParameter("noiseVolume", temp);
        ///m_wave.setVolume(temp);
    }
    
    if (m_register.noisePeriodChanged && m_register.noisePeriod > 0)
    {
        m_register.noisePeriodChanged = false;
        //double newFrequency = cpuFrequency / (16.0 * (double)(m_register.noisePeriod + 1));
        //m_wave.setFreq((float)newFrequency);
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
    //m_wave.reset();
    //m_wave.setVolume(0);
    m_shiftRegister = 0x0001;
    m_lengthCounter = 0;
    m_timer = 0;
    m_currentOutput = 0.0f;
}

void NoiseChannel::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_register.SerializeTo(visitor);
    m_enveloppe.SerializeTo(visitor);
    visitor.WriteValue(m_shiftRegister);
    visitor.WriteValue(m_lengthCounter);
    visitor.WriteValue(m_timer);
}

void NoiseChannel::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_register.DeserializeFrom(visitor);
    m_enveloppe.DeserializeFrom(visitor);
    visitor.ReadValue(m_shiftRegister);
    visitor.ReadValue(m_lengthCounter);
    visitor.ReadValue(m_timer);
}

void NoiseChannel::ReloadCounter()
{
    m_lengthCounter = Cst::APU_LENGTH_TABLE[m_register.lengthCounterLoad];
}