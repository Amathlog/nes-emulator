#pragma once

#include <core/mapper.h>
#include <cstdint>
#include <array>
#include <vector>

namespace NesEmulator
{
    class Mapper_004 : public IMapper
    {
    public:
        Mapper_004(const iNESHeader& header);
        ~Mapper_004() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        void Reset() override;

    private:
        std::array<uint8_t, 8> m_indexes;
        uint8_t m_prgRomBankMode = 0;
        uint8_t m_chrRomBankMode = 0;
        uint8_t m_nextIndexToUpdate = 0;
        bool m_prgRamEnabled = true;

        std::vector<uint8_t> m_staticRAM;
    };
}