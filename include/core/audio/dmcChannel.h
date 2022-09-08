#pragma once

#include <cstdint>
#include <core/utils/visitor.h>

namespace NesEmulator
{
    struct DMCRegister
    {
        union Flags
        {
            struct
            {
                uint8_t rate : 4;
                uint8_t unused : 2;
                uint8_t loop : 1;
                uint8_t irqEnabled : 1;
            };

            uint8_t reg = 0x00;
        } flags;

        uint8_t directLoad = 0x00;
        uint8_t sampleAddress = 0x00;
        uint8_t sampleLength = 0x00;

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);
    };

    class DMCChannel
    {
    public:
        void Reset();
        void Clock();
        double GetSample();
        void Update(double cpuFrequency);

        void WriteData(uint16_t address, uint8_t data);
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);

    private:
        DMCRegister m_register;
    };
}