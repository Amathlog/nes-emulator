#pragma once

#include <core/mapper.h>
#include <cstdint>

namespace NesEmulator
{
    class Mapper_001 : public IMapper
    {
    public:
        Mapper_001(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring);
        ~Mapper_001() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        Mirroring GetMirroring() const override { return m_mirroring; }

    private:
        void ResetShiftRegister();
        void HandleLoadRegister(uint8_t data);
        uint8_t m_shiftRegister = 0x01;
        uint8_t m_internalRegister = 0x00;
        bool m_loadRegisterDone = false;
        Mirroring m_mirroring;
    };
}