#pragma once
#include <core/serializable.h>
#include <cstdint>
#include <core/utils/visitor.h>
#include <core/constants.h>
#include <core/audio/pulseChannel.h>
#include <core/audio/triangleChannel.h>
#include <core/audio/noiseChannel.h>
#include <core/audio/dmcChannel.h>
#include <core/audio/circularBuffer.h>

namespace NesEmulator
{
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

        void Stop();

        void WriteCPU(uint16_t addr, uint8_t data);
        uint8_t ReadCPU(uint16_t addr);

        void Reset();

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

        void FillSamples(float *outData, unsigned int numFrames, unsigned int numChannels);
        void SetMode(Mode mode) { m_mode = mode; }
        void SampleRequested();

        void SetEnable(bool enable);

    private:
        PulseChannel m_pulseChannel1;
        PulseChannel m_pulseChannel2;
        TriangleChannel m_triangleChannel;
        NoiseChannel m_noiseChannel;
        DMCChannel m_dmcChannel;
        APUStatus m_statusRegister;
        FrameCounter m_frameCounterRegister;

        std::array<float, 128> m_internalBuffer;
        size_t m_bufferPtr = 0;
        CircularBuffer<float> m_circularBuffer;

        size_t m_frameClockCounter = 0;
        Mode m_mode; // NTSC or PAL
        bool m_IRQFlag = false;

        bool m_enable;
    };
}