#include "core/audio/pulseChannel.h"
#include "core/constants.h"
#include <core/processor2A03.h>
#include <mutex>
#include <string>

using NesEmulator::Processor2A03;

Processor2A03::Processor2A03()
    : m_pulseChannel1(1)
    , m_pulseChannel2(2)
    , m_circularBuffer(1000000)
{
}

void Processor2A03::Stop()
{
    m_circularBuffer.Stop();
}

void Processor2A03::Clock()
{
    bool quarterFrame = false;
    bool halfFrame = false;

    m_frameClockCounter++;

    // Step 1 and Step 3 of the sequencer
    if (m_frameClockCounter == Cst::APU_SEQUENCER_STEP1 || m_frameClockCounter == Cst::APU_SEQUENCER_STEP3)
    {
        quarterFrame = true;
    }

    // Step 2 of the sequencer
    if (m_frameClockCounter == Cst::APU_SEQUENCER_STEP2)
    {
        quarterFrame = true;
        halfFrame = true;
    }

    // In case of 4-Step, reset is on step 4. In case of 5-Step, reset is on step 5
    // and there is nothing done on step4
    // Also, we raise the IRQ flag in 4-Step only
    if ((m_frameCounterRegister.mode == 0 && m_frameClockCounter == Cst::APU_SEQUENCER_STEP4) ||
        (m_frameCounterRegister.mode == 1 && m_frameClockCounter == Cst::APU_SEQUENCER_STEP5))
    {
        // 4-step
        quarterFrame = true;
        halfFrame = true;
        m_frameClockCounter = 0;

        if (m_frameCounterRegister.mode == 0)
            m_IRQFlag = m_frameCounterRegister.irqInhibit == 0;
    }

    if (quarterFrame)
    {
        // Update volume enveloppe
        m_pulseChannel1.ClockEnveloppe();
        m_pulseChannel2.ClockEnveloppe();
        m_noiseChannel.ClockEnveloppe();

        // Update linear counter for triangle
        m_triangleChannel.ClockLinear(m_statusRegister.enableLengthCounterTriangle);
    }

    if (halfFrame)
    {
        // Update Length counters and sweep
        m_pulseChannel1.Clock(m_statusRegister.enableLengthCounterPulse1);
        m_pulseChannel2.Clock(m_statusRegister.enableLengthCounterPulse2);
        m_triangleChannel.ClockLength(m_statusRegister.enableLengthCounterTriangle);
        m_noiseChannel.Clock(m_statusRegister.enableLengthCounterNoise);
    }

    double cpuFrequency = (m_mode == Mode::NTSC) ? Cst::NTSC_CPU_FREQUENCY : Cst::PAL_CPU_FREQUENCY;

    {
        m_pulseChannel1.Track();
        m_pulseChannel2.Track();
        m_pulseChannel1.Update(cpuFrequency);
        m_pulseChannel2.Update(cpuFrequency);
        m_triangleChannel.Update(cpuFrequency);
        m_noiseChannel.Update(cpuFrequency);
    }
}
void Processor2A03::FillSamples(float *outData, unsigned int numFrames, unsigned int numChannels)
{
    if (m_enable)
        m_circularBuffer.ReadData(outData, numFrames * numChannels * sizeof(float));
}

void Processor2A03::WriteCPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x4000 && addr <= 0x4007)
    {
        // Pulse
        PulseChannel& currPulseChannel = ((addr & 0x0004) > 0) ? m_pulseChannel2 : m_pulseChannel1;
        PulseRegister& currPulseRegister = currPulseChannel.GetRegister();
        Sweep& currSweep = currPulseChannel.GetSweep();
        Enveloppe& currEnv = currPulseChannel.GetEnveloppe();
        bool isEnabled = ((addr & 0x0004) > 0) ? m_statusRegister.enableLengthCounterPulse2 > 0 : m_statusRegister.enableLengthCounterPulse1 > 0;
        switch (addr & 0x0003)
        {
        case 0:
            currPulseRegister.duty = data >> 6;
            currPulseRegister.enveloppeLoop = (data & 0x20) > 0;
            currEnv.disable = (data & 0x10) > 0;
            currEnv.volume = (data & 0x0F);
            break;
        case 1:
            currSweep.enabled = (data & 0x80) > 0;
            currSweep.period = (data & 0x70) >> 4;
            currSweep.down = (data & 0x08) > 0;
            currSweep.shift = (data & 0x07);
            currSweep.reload = true;
            break;
        case 2:
            currPulseRegister.timer = (currPulseRegister.timer & 0xFF00) | (uint16_t)data;
            break;
        case 3:
            currPulseRegister.lengthCounterReload = (data & 0xF8) >> 3;
            if (isEnabled)
                currPulseChannel.ReloadCounter();
            currPulseRegister.timer = (uint16_t)(data & 0x07) << 8 | (currPulseRegister.timer & 0x00FF);
            currEnv.start = true;
            break;
        }
    }
    else if (addr >= 0x4008 && addr <= 0x400B)
    {
        // Triangle
        TriangleRegister& currRegister = m_triangleChannel.GetRegister();
        switch (addr & 0x0003)
        {
        case 0:
            currRegister.control = data >> 7;
            currRegister.linearCounterLoad = data & 0x7F;
            break;
        case 1:
            // Unused
            break;
        case 2:
            currRegister.timer = ( currRegister.timer & 0xFF00) | (uint16_t)data;
            break;
        case 3:
            currRegister.lengthCounterLoad = (data & 0xF8) >> 3;
            if (m_statusRegister.enableLengthCounterTriangle)
                m_triangleChannel.ReloadCounter();
            currRegister.timer = (uint16_t)(data & 0x07) << 8 | (currRegister.timer & 0x00FF);
            // As a side effect, it also set the linear control flag
            m_triangleChannel.SetLinearControlFlag(1);
            break;
        }
    }
    else if (addr >= 0x400C && addr <= 0x400F)
    {
        // Noise
        NoiseRegister& noiseRegister = m_noiseChannel.GetRegister();
        switch (addr & 0x0003)
        {
        case 0:
            noiseRegister.enveloppeLoop = (data & 0x20) > 0;
            m_noiseChannel.GetEnveloppe().disable = (data & 0x10) > 0;
            m_noiseChannel.GetEnveloppe().volume = (data & 0x0F);
            break;
        case 1:
            // Unused
            break;
        case 2:
            noiseRegister.mode = (data & 0x80) > 0;
            noiseRegister.SetNoisePeriod((data & 0x0F), m_mode);
            m_noiseChannel.SetMode(noiseRegister.mode != 0);
            break;
        case 3:
            noiseRegister.lengthCounterLoad = (data & 0xF8) >> 3;
            if (m_statusRegister.enableLengthCounterNoise)
                m_noiseChannel.ReloadCounter();
            m_noiseChannel.GetEnveloppe().start = true;
            break;
        }
    }
    else if (addr >= 0x4010 && addr <= 0x4013)
    {
        // DMC
        switch (addr & 0x0003)
        {
        case 0:
            m_dmcRegister.irqEnable = (data & 0x80) > 0;
            m_dmcRegister.loop = (data & 0x40) > 0;
            m_dmcRegister.frequency = (data & 0x0F);
            break;
        case 1:
            m_dmcRegister.loadCounter = (data & 0x7F);
            break;
        case 2:
            m_dmcRegister.sampleAddress = data;
            break;
        case 3:
            m_dmcRegister.sampleLength = data;
            break;
        }
    }
    else if (addr == 0x4015)
    {
        // Status
        m_statusRegister.flags = (m_statusRegister.flags & 0xE0) | (data & 0x1F);
        // Silence length counters if we disable a channel
        if (!m_statusRegister.enableLengthCounterPulse1)
            m_pulseChannel1.Reset();
        if (!m_statusRegister.enableLengthCounterPulse2)
            m_pulseChannel2.Reset();
        if (!m_statusRegister.enableLengthCounterTriangle)
            m_triangleChannel.Reset();
        if (!m_statusRegister.enableLengthCounterNoise)
            m_noiseChannel.Reset();
        if (!m_statusRegister.enableActivedmc)
        {
            // TODO
            m_statusRegister.dmcInterrupt = false;
        }
    }
    else if (addr == 0x4017)
    {
        // Frame counter
        m_frameCounterRegister.flags = data & 0xC0;

        if (m_frameCounterRegister.irqInhibit)
            m_IRQFlag = false;
    }
}

uint8_t Processor2A03::ReadCPU(uint16_t addr)
{
    // Status
    if (addr == 0x4015)
    {
        APUStatus statusCopy = m_statusRegister;
        statusCopy.enableLengthCounterPulse1 &= m_pulseChannel1.GetCounter() != 0;
        statusCopy.enableLengthCounterPulse2 &= m_pulseChannel2.GetCounter() != 0;
        statusCopy.enableLengthCounterTriangle &= m_triangleChannel.GetCounter() != 0;
        statusCopy.enableLengthCounterNoise &= m_noiseChannel.GetCounter() != 0;
        // statusCopy.enableActivedmc &= m_dmcChannel.Remaining() > 0;
        // Also by reading this register, we clear the IRQFlag
        m_IRQFlag = false;
        return statusCopy.flags;
    }

    return 0;
}

void Processor2A03::SampleRequested()
{
    // Based on the linear approximation
    // output = square_out + tnd_out
    // square_out = 0.00752 * (square1 + square2)
    // tnd_out = 0.00851 * triangle + 0.00494 * noise + 0.00335 * dmc
    // To compute the final output, we sum all the coefficients as
    // a scale factor
    // With this, it is close to 26% for a pulse channel 29% for triangle
    // and 17% for noise
    // (not implementing DMC for now)

    constexpr double pulseCoeff = 0.00752;
    constexpr double triangleCoeff = 0.00851;
    constexpr double noiseCoeff = 0.00494;

    constexpr double maxCoeff = 2 * pulseCoeff + triangleCoeff + noiseCoeff;

    if (m_enable)
    {
        double sample = pulseCoeff * (m_pulseChannel1.GetSample() + m_pulseChannel2.GetSample()) + 
            triangleCoeff * m_triangleChannel.GetSample() + 
            noiseCoeff * m_noiseChannel.GetSample();

        sample /= maxCoeff;
        m_internalBuffer[m_bufferPtr++] = (float)sample;
        m_internalBuffer[m_bufferPtr++] = (float)sample;

        if (m_bufferPtr == m_internalBuffer.max_size())
        {
            m_bufferPtr = 0;
            m_circularBuffer.WriteData(m_internalBuffer.data(), m_internalBuffer.max_size() * sizeof(float));
        }
    }
}

void Processor2A03::Reset()
{
    m_pulseChannel1.Reset();
    m_pulseChannel2.Reset();
    m_triangleChannel.Reset();
    m_noiseChannel.Reset();
    m_dmcRegister.Reset();
    m_statusRegister.flags = 0;
    m_frameCounterRegister.flags = 0;
    m_frameClockCounter = 0;

    m_bufferPtr = 0;
    m_circularBuffer.Reset();
}

void Processor2A03::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_pulseChannel1.SerializeTo(visitor);
    m_pulseChannel2.SerializeTo(visitor);
    m_triangleChannel.SerializeTo(visitor);
    m_noiseChannel.SerializeTo(visitor);
    m_dmcRegister.SerializeTo(visitor);

    visitor.WriteValue(m_statusRegister.flags);
    visitor.WriteValue(m_frameCounterRegister.flags);
    visitor.WriteValue(m_frameClockCounter);
}

void Processor2A03::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_pulseChannel1.DeserializeFrom(visitor);
    m_pulseChannel2.DeserializeFrom(visitor);
    m_triangleChannel.DeserializeFrom(visitor);
    m_noiseChannel.DeserializeFrom(visitor);
    m_dmcRegister.DeserializeFrom(visitor);

    visitor.ReadValue(m_statusRegister.flags);
    visitor.ReadValue(m_frameCounterRegister.flags);
    visitor.ReadValue(m_frameClockCounter);
}

void Processor2A03::SetEnable(bool enable)
{
    m_enable = enable;

    if (enable)
    {
        m_circularBuffer.Reset();
    }
    else
    {
        m_circularBuffer.Stop();
    }
}