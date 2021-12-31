#include "core/constants.h"
#include <core/processor2A03.h>
#include <string>

using NesEmulator::Processor2A03;

uint8_t length_table[] = {  10, 254, 20,  2, 40,  4, 80,  6,
                            160,   8, 60, 10, 14, 12, 26, 14,
                            12,  16, 24, 18, 48, 20, 96, 22,
                            192,  24, 72, 26, 16, 28, 32, 30 };

Processor2A03::Processor2A03()
{
    // Create all the waves
    Tonic::ControlGenerator controlDutyPulse1 = m_synth.addParameter("dutyCyclePulse1");
    Tonic::ControlGenerator controlFreqPulse1 = m_synth.addParameter("freqPulse1");
    Tonic::ControlGenerator controlOutputPulse1 = m_synth.addParameter("outputPulse1");
    Tonic::Generator pulse1 = controlOutputPulse1 * Tonic::RectWave().freq(controlFreqPulse1).pwm(controlDutyPulse1);

    Tonic::ControlGenerator controlDutyPulse2 = m_synth.addParameter("dutyCyclePulse2");
    Tonic::ControlGenerator controlFreqPulse2 = m_synth.addParameter("freqPulse2");
    Tonic::ControlGenerator controlOutputPulse2 = m_synth.addParameter("outputPulse2");
    Tonic::Generator pulse2 = controlOutputPulse2 * Tonic::RectWave().freq(controlFreqPulse2).pwm(controlDutyPulse2);

    Tonic::ControlGenerator controlFreqTriangle = m_synth.addParameter("freqTriangle");
    Tonic::ControlGenerator controlOutputTriangle = m_synth.addParameter("outputTriangle");
    Tonic::Generator triangle = controlOutputTriangle * Tonic::TriangleWave().freq(controlFreqTriangle);

    m_synth.setOutputGen(5.0 * (0.00752 * (pulse1 + pulse2) + 0.00851 * triangle));
}

void Processor2A03::Clock()
{
    bool quarterFrame = false;
    bool halfFrame = false;

    if (m_clockCounter % 6 == 0)
    {
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
            // TODO

            // Update linear counter for triangle
            m_triangleRegister.ClockLinear(m_statusRegister.enableLengthCounterTriangle);
        }

        if (halfFrame)
        {
            // Update sweep
            // TODO

            // Update Length counters
            m_pulseRegister1.Clock(m_statusRegister.enableLengthCounterPulse1);
            m_pulseRegister2.Clock(m_statusRegister.enableLengthCounterPulse2);
            m_triangleRegister.ClockLength(m_statusRegister.enableLengthCounterTriangle);
        }

        double cpuFrequency = (m_mode == Mode::NTSC) ? Cst::NTSC_CPU_FREQUENCY : Cst::PAL_CPU_FREQUENCY;

        // Pulse 1 and 2
        auto updatePulse = [this, cpuFrequency](PulseRegister& pulseRegister, bool isEnabled, int number)
        {
            double newEnableValue = pulseRegister.lengthCounter > 0 ? 1.0 : 0.0;
            double newFrequency = isEnabled ? cpuFrequency / (16.0 * (double)(pulseRegister.timer + 1)) : 0.0;
            double newDutyCycle = 0.0;
            switch (pulseRegister.duty)
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

            if (newFrequency != pulseRegister.frequency || newDutyCycle != pulseRegister.dutyCycle 
                || newEnableValue != pulseRegister.enableValue)
            {
                pulseRegister.frequency = newFrequency;
                pulseRegister.dutyCycle = newDutyCycle;
                pulseRegister.enableValue = newEnableValue;
                m_synth.setParameter(std::string("dutyCyclePulse") + std::to_string(number), newDutyCycle);
                m_synth.setParameter(std::string("freqPulse") + std::to_string(number), newFrequency);
                m_synth.setParameter(std::string("outputPulse") + std::to_string(number), newEnableValue);
            }
        };

        updatePulse(m_pulseRegister1, m_statusRegister.enableLengthCounterPulse1, 1);
        updatePulse(m_pulseRegister2, m_statusRegister.enableLengthCounterPulse2, 2);

        // Triangle
        double newFrequency = m_statusRegister.enableLengthCounterTriangle ? cpuFrequency / (32.0 * (double)(m_triangleRegister.timer + 1)) : 0.0;
        double newEnableValue = m_triangleRegister.linearCounter > 0 && m_triangleRegister.lengthCounter > 0;
        if (newFrequency != m_triangleRegister.frequency || newEnableValue != m_triangleRegister.enableValue)
        {
            m_triangleRegister.frequency = newFrequency;
            m_triangleRegister.enableValue = newEnableValue;
            m_synth.setParameter("freqTriangle", newFrequency);
            m_synth.setParameter("outputTriangle", newEnableValue);
        }
    }

    m_clockCounter++;
}

void Processor2A03::WriteCPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x4000 && addr <= 0x4007)
    {
        // Pulse
        PulseRegister& currPulseRegister = ((addr & 0x0004) > 0) ? m_pulseRegister2 : m_pulseRegister1;
        switch (addr & 0x0003)
        {
        case 0:
            currPulseRegister.duty = data >> 6;
            currPulseRegister.enveloppeLoop = (data & 0x20) > 0;
            currPulseRegister.constantVolume = (data & 0x10) > 0;
            currPulseRegister.volumeEnveloppe = (data & 0x0F);
            break;
        case 1:
            currPulseRegister.sweepEnable = (data & 0x80) > 0;
            currPulseRegister.sweepPeriod = (data & 0x70) >> 4;
            currPulseRegister.sweepNegate = (data & 0x08) > 0;
            currPulseRegister.sweepShift = (data & 0x07);
            break;
        case 2:
            currPulseRegister.timer = (currPulseRegister.timer & 0xFF00) | (uint16_t)data;
            break;
        case 3:
            currPulseRegister.lengthCounterReload = (data & 0xF8) >> 3;
            currPulseRegister.lengthCounter = length_table[currPulseRegister.lengthCounterReload];
            currPulseRegister.timer = (uint16_t)(data & 0x07) << 8 | (currPulseRegister.timer & 0x00FF);
            break;
        }
    }
    else if (addr >= 0x4008 && addr <= 0x400B)
    {
        // Triangle
        switch (addr & 0x0003)
        {
        case 0:
            m_triangleRegister.control = data >> 7;
            m_triangleRegister.linearCounterLoad = data & 0x7F;
            break;
        case 1:
            // Unused
            break;
        case 2:
            m_triangleRegister.timer = (m_triangleRegister.timer & 0xFF00) | (uint16_t)data;
            break;
        case 3:
            m_triangleRegister.lengthCounterLoad = (data & 0xF8) >> 3;
            m_triangleRegister.lengthCounter = length_table[m_triangleRegister.lengthCounterLoad];
            m_triangleRegister.timer = (uint16_t)(data & 0x07) << 8 | (m_triangleRegister.timer & 0x00FF);
            // As a side effect, it also set the linear control flag
            m_triangleRegister.linearControlFlag = 1;
            break;
        }
    }
    else if (addr >= 0x400C && addr <= 0x400F)
    {
        // Noise
        switch (addr & 0x0003)
        {
        case 0:
            m_noiseRegister.enveloppeLoop = (data & 0x20) >> 5;
            m_noiseRegister.constantVolume = (data & 0x10) > 0;
            m_noiseRegister.volumeEnveloppe = (data & 0x0F);
            break;
        case 1:
            // Unused
            break;
        case 2:
            m_noiseRegister.loopNoise = (data & 0x80) > 0;
            m_noiseRegister.noisePeriod = (data & 0x0F);
            break;
        case 3:
            m_noiseRegister.lengthCounterLoad = (data & 0xF8) >> 3;
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
    if (addr == 0x4015)
    {
        // Status
        // Also by reading this register, we clear the IRQFlag
        m_IRQFlag = false;
        return m_statusRegister.flags;
    }

    return 0;
}

void Processor2A03::Reset()
{
    m_pulseRegister1.Reset();
    m_pulseRegister2.Reset();
    m_triangleRegister.Reset();
    m_noiseRegister.Reset();
    m_dmcRegister.Reset();
    m_statusRegister.flags = 0;
    m_frameCounterRegister.flags = 0;
    m_clockCounter = 0;
    m_frameClockCounter = 0;
}

void Processor2A03::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    m_pulseRegister1.SerializeTo(visitor);
    m_pulseRegister2.SerializeTo(visitor);
    m_triangleRegister.SerializeTo(visitor);
    m_noiseRegister.SerializeTo(visitor);
    m_dmcRegister.SerializeTo(visitor);

    visitor.WriteValue(m_statusRegister.flags);
    visitor.WriteValue(m_frameCounterRegister.flags);
    visitor.WriteValue(m_clockCounter);
    visitor.WriteValue(m_frameClockCounter);
}

void Processor2A03::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    m_pulseRegister1.DeserializeFrom(visitor);
    m_pulseRegister2.DeserializeFrom(visitor);
    m_triangleRegister.DeserializeFrom(visitor);
    m_noiseRegister.DeserializeFrom(visitor);
    m_dmcRegister.DeserializeFrom(visitor);

    visitor.ReadValue(m_statusRegister.flags);
    visitor.ReadValue(m_frameCounterRegister.flags);
    visitor.ReadValue(m_clockCounter);
    visitor.ReadValue(m_frameClockCounter);
}