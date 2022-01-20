#pragma once

#include <core/mapper.h>
#include <cstdint>

namespace NesEmulator
{
    class Mapper_003 : public IMapper
    {
    public:
        Mapper_003(const iNESHeader& header, Mapping& mapping);
        ~Mapper_003() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        void Reset() override { IMapper::Reset(); InternalReset(); }

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

    private:
        void InternalReset();
        void UpdateMapping();
         
        uint8_t m_currentSwitchChrBank = 0;
    };
}