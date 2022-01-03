#pragma once
#include <core/serializable.h>
#include <cstdint>
#include <core/utils/visitor.h>
#include <MyTonic.h>
#include <core/constants.h>
#include <core/audio/pulseChannel.h>
#include <core/audio/triangleChannel.h>

namespace NesEmulator
{
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
        void SetMode(Mode mode) { m_mode = mode; }

    private:
        Tonic::Synth m_synth;
        PulseChannel m_pulseChannel1;
        PulseChannel m_pulseChannel2;
        TriangleChannel m_triangleChannel;
        NoiseRegister m_noiseRegister;
        DMCRegister m_dmcRegister;
        APUStatus m_statusRegister;
        FrameCounter m_frameCounterRegister;

        size_t m_clockCounter = 0;
        size_t m_frameClockCounter = 0;
        Mode m_mode; // NTSC or PAL
        bool m_IRQFlag = false;
    };
}