#pragma once

#include <core/mapper.h>
#include <cstdint>
#include <array>
#include <vector>

namespace NesEmulator
{
    class Mapper_040 : public IMapper
    {
    public:
        Mapper_040(const iNESHeader& header);
        ~Mapper_040() = default;

        bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
        bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override; 
        bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) override;
        void Reset() override;

        // Called at the end of each scanline
        void CPUClock() override;
        // Check if we should fire an IRQ
        bool ShouldIRQ() const override;
        void ClearIRQ() override;

    private:
        uint8_t m_switchableBank = 0;

        uint16_t m_IRQCounter = 0;
        bool m_IRQEnabled = false;
        bool m_IRQActive = false;
    };
}