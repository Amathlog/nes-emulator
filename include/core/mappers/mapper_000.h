#pragma once

#include <core/mapper.h>

namespace NesEmulator
{
    class Mapper_000 : public IMapper
    {
    public:
        Mapper_000(uint8_t nbPrgBanks, uint8_t nbChrBanks);
        ~Mapper_000() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress) override;
    };
}