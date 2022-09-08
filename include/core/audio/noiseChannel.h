#pragma once

#include <cstdint>
#include <core/audio/enveloppe.h>
#include <core/utils/visitor.h>
#include <core/constants.h>

#include <MyTonic.h>

namespace NesEmulator 
{
    struct NoiseRegister
    {
        uint8_t enveloppeLoop;
        uint8_t constantVolume;
        uint8_t volumeEnveloppe;
        uint8_t mode;
        int16_t noisePeriod;
        uint8_t lengthCounterLoad;
        bool noisePeriodChanged;

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
        void SetNoisePeriod(uint8_t index, Mode mode);
    };

    class NoiseOscillator
    {
    public:
        NoiseOscillator();
        void Reset();
        void SetFrequency(double freq);

        double Tick();

        double m_elaspedTime = 0.0;
        uint16_t m_shiftRegister = 1;
        double m_realSampleDuration = 0.0;
        double m_sampleDuration = 0.0;
    };

    class NoiseChannel
    {
    public:
        NoiseChannel(Tonic::Synth& synth);
        ~NoiseChannel() = default;

        void Reset();
        void Clock(bool isEnabled);
        void ClockEnveloppe();
        void Update(double cpuFrequency, Tonic::Synth& synth);

        double GetSample();

        NoiseRegister& GetRegister() { return m_register; }
        Tonic::Generator& GetWave() { return m_wave; }
        Enveloppe& GetEnveloppe() { return m_enveloppe; }

        void ReloadCounter();
        uint8_t GetCounter() const { return m_lengthCounter; }

        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
        
    private:
        MyNoise m_wave;
        NoiseRegister m_register;
        Enveloppe m_enveloppe;

        NoiseOscillator m_oscillator;

        uint8_t m_lengthCounter = 0;
        int16_t m_timer = 0;
        float m_currentOutput = 0.0f;
    };
}