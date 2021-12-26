#pragma once
#include <cstdint>

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
        uint8_t lengthCounter;

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
            lengthCounter = 0;
        }
    };

    struct TriangleRegister
    {
        uint8_t control;
        uint8_t linearCounterLoad;
        uint16_t timer;
        uint8_t lengthCounterLoad;

        void Reset()
        {
            control = 0;
            linearCounterLoad = 0;
            timer = 0;
            lengthCounterLoad = 0;
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
    };

    union APUStatus
    {
        struct
        {
            uint8_t dmcInterrupt : 1;
            uint8_t frameInterrupt : 1;
            uint8_t unused : 1;
            uint8_t enableActivedmc : 1;
            uint8_t enableLengthCounterNoise : 1;
            uint8_t enableLengthCounterTriangle : 1;
            uint8_t enableLengthCounterPulse2 : 1;
            uint8_t enableLengthCounterPulse1 : 1;
        };

        uint8_t flags;
    };

    union FrameCounter
    {
        struct
        {
            uint8_t mode : 1;
            uint8_t irqInhibit : 1;
            uint8_t unused : 6;
        };

        uint8_t flags;
    };

    class Processor2A03
    {
    public:
        Processor2A03() = default;
        ~Processor2A03() = default;

        void Clock();

        void WriteCPU(uint16_t addr, uint8_t data);
        uint8_t ReadCPU(uint16_t addr);

        void Reset();

    private:
        PulseRegister m_pulseRegister1;
        PulseRegister m_pulseRegister2;
        TriangleRegister m_triangleRegister;
        NoiseRegister m_noiseRegister;
        DMCRegister m_dmcRegister;
        APUStatus m_statusRegister;
        FrameCounter m_frameCounterRegister;
    };
}