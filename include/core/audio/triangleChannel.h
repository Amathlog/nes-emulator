#pragma once

#include <cstdint>
#include <core/utils/visitor.h>
#include <MyTonic.h>
#include <libs/maxiPolyBLEP.h>

namespace NesEmulator 
{
    struct TriangleRegister
    {
        uint8_t control;
        uint8_t linearCounterLoad;
        uint16_t timer;
        uint8_t lengthCounterLoad;

        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
        void Reset();
    };

    class TriangleChannel
    {
    public:
        TriangleChannel(Tonic::Synth& synth);
        ~TriangleChannel() = default;

        void Reset();
        void ClockLinear(bool isEnabled);
        void ClockLength(bool isEnabled);
        void Update(double cpuFrequency, Tonic::Synth& synth);

        TriangleRegister& GetRegister() { return m_register; }
        Tonic::Generator& GetWave() { return m_wave; }

        void SetLinearControlFlag(uint8_t value) { m_linearControlFlag = value; }
        void ReloadCounter();
        uint8_t GetCounter() const { return m_lengthCounter; }

        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
        double GetAudioSample();
        
    private:
        Tonic::Generator m_wave;
        TriangleRegister m_register;
        maxiPolyBLEP m_triangle;

        double m_frequency = 0.0;
        double m_enableValue = 0.0;
        uint8_t m_linearCounter = 0;
        uint8_t m_lengthCounter = 0;
        uint8_t m_linearControlFlag = 0;
    };
}