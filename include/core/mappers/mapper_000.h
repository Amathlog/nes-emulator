#pragma once

#include <core/mapper.h>
#include <cstdint>

namespace NesEmulator
{
    class Mapper_000 : public IMapper
    {
    public:
        Mapper_000(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring intialMirroring);
        ~Mapper_000() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        Mirroring GetMirroring() const override { return m_mirroring; }

    private:
        Mirroring m_mirroring;
    };
}