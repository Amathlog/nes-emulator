#pragma once

#include <core/mapper.h>
#include <cstdint>
#include <vector>

namespace NesEmulator
{
    class Mapper_001 : public IMapper
    {
    public:
        Mapper_001(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring);
        ~Mapper_001() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        Mirroring GetMirroring() const override { return m_mirroring; }

        void Reset() override;

    private:
        void ResetShiftRegister();
        void ResetControlRegister();
        void HandleLoadRegister(uint8_t data);
        uint8_t m_shiftRegister = 0x00;
        uint8_t m_shiftCounter = 0x00;
        uint8_t m_internalRegister = 0x00;
        bool m_loadRegisterDone = false;
        Mirroring m_mirroring;
        Mirroring m_originalMirroring;

        uint8_t m_currentPrgBankSwitch = 0;
        bool m_PrgBank0IsSwitch = false;
        uint8_t m_currentChrBank0 = 0;
        uint8_t m_currentChrBank1 = 1;
        bool m_32kBModePrgBank = false;
        bool m_8kBModeChrBank = false;

        std::vector<uint8_t> m_staticRAM;
    };
}