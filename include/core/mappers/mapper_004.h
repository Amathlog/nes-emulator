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

        // Called at the end of each scanline
        void ScanlineDone() override;
        // Check if we should fire an IRQ
        bool ShouldIRQ() const override;
        void ClearIRQ() override;

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

    private:
        std::array<uint8_t, 8> m_indexes;
        uint8_t m_prgRomBankMode = 0;
        uint8_t m_chrRomBankMode = 0;
        uint8_t m_nextIndexToUpdate = 0;
        bool m_prgRamEnabled = true;

        uint8_t m_IRQCounter = 0;
        uint8_t m_IRQReload = 0;
        bool m_IRQShouldReload = false;
        bool m_IRQEnabled = false;
        bool m_IRQActive = false;
    };
}