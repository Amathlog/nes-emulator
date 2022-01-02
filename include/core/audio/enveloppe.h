#pragma once

#include <cstdint>
#include <core/utils/visitor.h>

namespace NesEmulator
{
    struct Enveloppe
    {
        void Clock(bool loop);

        void Reset();
        void SerializeTo(Utils::IWriteVisitor& visitor) const;
        void DeserializeFrom(Utils::IReadVisitor& visitor);

        bool start;
        bool disable;
        uint16_t dividerCount;
        uint16_t volume;
        uint16_t output;
        uint16_t decayCount;
        bool updated;
    };
}