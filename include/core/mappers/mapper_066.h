#pragma once

#include <core/mapper.h>
#include <cstdint>

namespace NesEmulator
{
    class Mapper_066 : public IMapper
    {
    public:
        Mapper_066(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring intialMirroring);
        ~Mapper_066() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        Mirroring GetMirroring() const override { return m_mirroring; }
        void Reset() override;

    private:
        Mirroring m_mirroring;
        uint8_t m_currentSwitchPrgBank = 0;
        uint8_t m_currentSwitchChrBank = 0;
    };
}