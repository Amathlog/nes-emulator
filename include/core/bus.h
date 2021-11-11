#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include <core/processor6502.h>
#include <core/processor2C02.h>

namespace NesEmulator
{      
    class Cartridge;

    class Bus
    {
    public:
        Bus();
        ~Bus() = default;

        void WriteCPU(uint16_t address, uint8_t data);
        uint8_t ReadCPU(uint16_t address);

        Processor6502& GetCPU() { return m_cpu; }
        Processor2C02& GetPPU() { return m_ppu; }
        const std::vector<uint8_t> GetCPURAM() { return m_cpuRam; }

        void InsertCartridge(const std::shared_ptr<Cartridge>& cartridge);

        void Clock();
        void Reset();

    private:
        Processor6502 m_cpu;
        Processor2C02 m_ppu;

        std::vector<uint8_t> m_cpuRam;
        std::shared_ptr<Cartridge> m_cartridge;
        size_t m_clockCounter = 0;
    };
}