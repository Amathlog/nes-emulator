#pragma once

#include <cstdint>
#include <vector>

#include <core/processor6502.h>

namespace NesEmulator
{
    class Bus
    {
    public:
        Bus();
        ~Bus() = default;

        void Write(uint16_t address, uint8_t data);
        uint8_t Read(uint16_t address);

        Processor6502& GetCPU() { return m_cpu; }

    private:
        Processor6502 m_cpu;
        std::vector<uint8_t> m_ram;
    };
}