#pragma once

#include "core/audio/enveloppe.h"
#include <MyTonic.h>
#include <core/utils/visitor.h>
#include <cstdint>
#include <string>

namespace NesEmulator {

    struct PulseRegister
    {
        uint8_t duty;
        uint8_t enveloppeLoop;
        uint16_t timer;
        uint8_t lengthCounterReload;

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
    };

    class PulseOscillator
    {
    public:
        PulseOscillator() = default;
        double Tick();

        void SetFrequency(double freq);
        void SetDuty(double duty) { m_duty = duty; }
        void Reset() { m_phase = 0.0; m_duty = 0.5; }

    private:
        double m_phase = 0.0;
        double m_freq = 0.0;
        double m_phaseIncrement = 0.0f;
        double m_duty = 0.5;
    };

    struct Sweep
    {
        bool enabled;
        bool down;
        bool reload;
        uint8_t shift;
        uint8_t timer;
        uint8_t period;
        uint16_t change;
        bool mute;

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);

        void Track(uint16_t target);
        void Clock(uint16_t& target, bool isChannel1);
    };

    class PulseChannel
    {
    public:
        PulseChannel(Tonic::Synth& synth, int number);
        ~PulseChannel() = default;

        PulseRegister& GetRegister() { return m_register; }
        Sweep& GetSweep() { return m_sweep; }
        Enveloppe& GetEnveloppe() { return m_enveloppe; }
        Tonic::Generator& GetWave() { return m_wave; }

        void Update(double cpuFrequency, Tonic::Synth& synth);
        void Clock(bool isEnabled);
        void ClockEnveloppe();
        void Track();
        void Reset();

        double GetSample();

        void ReloadCounter();
        uint8_t GetCounter() const { return m_lengthCounter; }

        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);

    private:
        std::string GetDutyCycleParameterName();
        std::string GetFrequencyParameterName();
        std::string GetOutputParameterName();
        std::string GetEnveloppeOutputParameterName();

        int m_number;
        Tonic::Generator m_wave;
        PulseRegister m_register;
        Sweep m_sweep;
        Enveloppe m_enveloppe;

        PulseOscillator m_oscillator;

        double m_frequency = 0.0;
        double m_dutyCycle = 0.0;
        double m_enableValue = 0.0;
        uint8_t m_lengthCounter = 0;
    };
}