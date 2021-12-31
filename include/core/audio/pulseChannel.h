#pragma once

#include <MyTonic.h>
#include <core/utils/visitor.h>
#include <string>

namespace NesEmulator {

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

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
    };

    class PulseChannel
    {
    public:
        PulseChannel(Tonic::Synth& synth, int number);
        ~PulseChannel() = default;

        PulseRegister& GetRegister() { return m_register; };
        Tonic::Generator& GetWave() { return m_wave; }
        void Update(double cpuFrequency, Tonic::Synth& synth);
        void Clock(bool isEnabled);
        void Reset();

        void ReloadCounter();

        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);

    private:
        std::string GetDutyCycleParameterName();
        std::string GetFrequencyParameterName();
        std::string GetOutputParameterName();

        int m_number;
        Tonic::Generator m_wave;
        PulseRegister m_register;

        double m_frequency = 0.0;
        double m_dutyCycle = 0.0;
        double m_enableValue = 0.0;
        uint8_t m_lengthCounter = 0;
    };
}