#pragma once
#include <core/serializable.h>
#include <cstdint>
#include <core/utils/visitor.h>
#include <MyTonic.h>
#include <core/constants.h>

namespace NesEmulator
{
    struct PulseRegister
    {
        uint8_t duty;
        uint8_t enveloppeLoop;
        uint8_t constantVolume;
        uint8_t volumeEnveloppe;
        uint8_t sweepEnable;
        uint8_t sweepPeriod;
        uint8_t sweepNegate;
        uint8_t sweepShift;
        uint16_t timer;
        uint8_t lengthCounterReload;

        double frequency = 0.0;
        double dutyCycle = 0.0;
        double enableValue = 0.0;
        uint8_t lengthCounter = 0;

        void Reset()
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

            frequency = 0.0;
            dutyCycle = 0.0;
            enableValue = 0.0;
            lengthCounter = 0;
        }

        void Clock(bool isEnabled)
        {
            if (!isEnabled)
            {
                lengthCounter = 0;
                return;
            }
            else if(lengthCounter > 0)
            {
                lengthCounter--;
            }
        }

        void SerializeTo(Utils::IWriteVisitor& visitor) const
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
            visitor.WriteValue(frequency);
            visitor.WriteValue(dutyCycle);
            visitor.WriteValue(enableValue);
            visitor.WriteValue(lengthCounter);
        }

        void DeserializeFrom(Utils::IReadVisitor& visitor)
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
            visitor.ReadValue(frequency);
            visitor.ReadValue(dutyCycle);
            visitor.ReadValue(enableValue);
            visitor.ReadValue(lengthCounter);
        }
    };

    struct TriangleRegister
    {
        uint8_t control;
        uint8_t linearCounterLoad;
        uint16_t timer;
        uint8_t lengthCounterLoad;

        double frequency = 0.0;
        double enableValue = 0.0;
        uint8_t linearCounter = 0;
        uint8_t lengthCounter = 0;
        uint8_t linearControlFlag = 0;

        void Reset()
        {
            control = 0;
            linearCounterLoad = 0;
            timer = 0;
            lengthCounterLoad = 0;

            frequency = 0.0;
            enableValue = 0.0;
            linearCounter = 0;
            lengthCounter = 0;
            linearControlFlag = 0;
        }

        void ClockLinear(bool isEnabled)
        {
            if (!isEnabled)
            {
                linearCounter = 0;
                return;
            }

            if (linearControlFlag > 0)
            {
                linearCounter = linearCounterLoad;
            }
            else if (linearCounter > 0)
            {
                linearCounter--;
            }

            if (control == 0)
                linearControlFlag = 0;
        }

        void ClockLength(bool isEnabled)
        {
            if (!isEnabled)
            {
                lengthCounter = 0;
            }
            else if (lengthCounter > 0)
            {
                lengthCounter--;
            }
        }

        void SerializeTo(Utils::IWriteVisitor& visitor) const
        {
            visitor.WriteValue(control);
            visitor.WriteValue(linearCounterLoad);
            visitor.WriteValue(timer);
            visitor.WriteValue(lengthCounterLoad);
            visitor.WriteValue(frequency);
            visitor.WriteValue(lengthCounter);
            visitor.WriteValue(linearCounter);
            visitor.WriteValue(linearControlFlag);
        }

        void DeserializeFrom(Utils::IReadVisitor& visitor)
        {
            visitor.ReadValue(control);
            visitor.ReadValue(linearCounterLoad);
            visitor.ReadValue(timer);
            visitor.ReadValue(lengthCounterLoad);
            visitor.ReadValue(frequency);
            visitor.ReadValue(lengthCounter);
            visitor.ReadValue(linearCounter);
            visitor.ReadValue(linearControlFlag);
        }
    };

    struct NoiseRegister
    {
        uint8_t enveloppeLoop;
        uint8_t constantVolume;
        uint8_t volumeEnveloppe;
        uint8_t loopNoise;
        uint8_t noisePeriod;
        uint8_t lengthCounterLoad;

        void Reset()
        {
            enveloppeLoop = 0;
            constantVolume = 0;
            volumeEnveloppe = 0;
            loopNoise = 0;
            noisePeriod = 0;
            lengthCounterLoad = 0;
        }

        void SerializeTo(Utils::IWriteVisitor& visitor) const
        {
            visitor.WriteValue(enveloppeLoop);
            visitor.WriteValue(constantVolume);
            visitor.WriteValue(volumeEnveloppe);
            visitor.WriteValue(loopNoise);
            visitor.WriteValue(noisePeriod);
            visitor.WriteValue(lengthCounterLoad);
        }

        void DeserializeFrom(Utils::IReadVisitor& visitor)
        {
            visitor.ReadValue(enveloppeLoop);
            visitor.ReadValue(constantVolume);
            visitor.ReadValue(volumeEnveloppe);
            visitor.ReadValue(loopNoise);
            visitor.ReadValue(noisePeriod);
            visitor.ReadValue(lengthCounterLoad);
        }
    };

    struct DMCRegister
    {
        uint8_t irqEnable;
        uint8_t loop;
        uint8_t frequency;
        uint8_t loadCounter;
        uint8_t sampleAddress;
        uint8_t sampleLength;

        void Reset()
        {
            irqEnable = 0;
            loop = 0;
            frequency = 0;
            loadCounter = 0;
            sampleAddress = 0;
            sampleLength = 0;
        }

        void SerializeTo(Utils::IWriteVisitor& visitor) const
        {
            visitor.WriteValue(irqEnable);
            visitor.WriteValue(loop);
            visitor.WriteValue(frequency);
            visitor.WriteValue(loadCounter);
            visitor.WriteValue(sampleAddress);
            visitor.WriteValue(sampleLength);
        }

        void DeserializeFrom(Utils::IReadVisitor& visitor)
        {
            visitor.ReadValue(irqEnable);
            visitor.ReadValue(loop);
            visitor.ReadValue(frequency);
            visitor.ReadValue(loadCounter);
            visitor.ReadValue(sampleAddress);
            visitor.ReadValue(sampleLength);
        }
    };

    union APUStatus
    {
        struct
        {
            uint8_t enableLengthCounterPulse1 : 1;
            uint8_t enableLengthCounterPulse2 : 1;
            uint8_t enableLengthCounterTriangle : 1;
            uint8_t enableLengthCounterNoise : 1;
            uint8_t enableActivedmc : 1;
            uint8_t unused : 1;
            uint8_t frameInterrupt : 1;
            uint8_t dmcInterrupt : 1;
        };

        uint8_t flags;
    };

    union FrameCounter
    {
        struct
        {
            uint8_t unused : 6;
            uint8_t irqInhibit : 1;
            uint8_t mode : 1;
        };

        uint8_t flags;
    };

    class Processor2A03 : public ISerializable
    {
    public:
        Processor2A03();
        ~Processor2A03() = default;

        void Clock();
        bool ShouldIRQ() { return m_IRQFlag; }

        void WriteCPU(uint16_t addr, uint8_t data);
        uint8_t ReadCPU(uint16_t addr);

        void Reset();

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

        Tonic::Synth* GetSynth() { return &m_synth; }
        void SetMode(Mode mode) { m_mode = mode ;}

    private:
        PulseRegister m_pulseRegister1;
        PulseRegister m_pulseRegister2;
        TriangleRegister m_triangleRegister;
        NoiseRegister m_noiseRegister;
        DMCRegister m_dmcRegister;
        APUStatus m_statusRegister;
        FrameCounter m_frameCounterRegister;

        Tonic::Synth m_synth;
        size_t m_clockCounter = 0;
        size_t m_frameClockCounter = 0;
        Mode m_mode; // NTSC or PAL
        bool m_IRQFlag = false;
    };
}