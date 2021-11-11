#include "core/utils/busVisitor.h"
#include "core/utils/disassembly.h"
#include <core/bus.h>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include <core/cartridge.h>
#include <core/constants.h>


using NesEmulator::Bus;
using NesEmulator::Processor6502;
using NesEmulator::Processor2C02;
using NesEmulator::Cartridge;

Bus::Bus()
{
    // Allocate RAM and set it to 0
    m_cpuRam.resize(Cst::RAM_SIZE); // 2KB
    for (auto& v : m_cpuRam)
        v = 0x00;

    // Connect CPU to this bus
    m_cpu.ConnectBus(this);
}

void Bus::WriteCPU(uint16_t address, uint8_t data)
{
    if (m_cartridge->WriteCPU(address, data))
    {
        // Do nothing
    }
    else if (address >= Cst::RAM_START_ADDR && address <= Cst::RAM_END_ADDR)
    {
        // Mirroring. Map to 0x0000 to 0x0800
        m_cpuRam[address % Cst::RAM_SIZE] = data;
    }
    else if (address >= Cst::PPU_REG_START_ADDR && address <= Cst::PPU_REG_END_ADDR)
    {
        // Mirroring.
        m_ppu.WriteCPU(address % Cst::PPU_REG_SIZE, data);
    }
}

uint8_t Bus::ReadCPU(uint16_t address)
{
    uint8_t data = 0;
    if (m_cartridge->ReadCPU(address, data))
    {
        // Done
    }
    else if (address >= Cst::RAM_START_ADDR && address <= Cst::RAM_END_ADDR)
    {
        // Mirroring. Map to 0x0000 to 0x07FF
        data = m_cpuRam[address % Cst::RAM_SIZE];
    }
    else if (address >= Cst::PPU_REG_START_ADDR && address <= Cst::PPU_REG_END_ADDR)
    {
        // Mirroring.
        data = m_ppu.ReadCPU(address % Cst::PPU_REG_SIZE);
    }

    return data;
}

void Bus::Clock()
{
    constexpr bool verbose = false;

    // PPU runs 3 times faster than the CPU
    m_ppu.Clock();
    if (m_clockCounter % 3 == 0)
    {
        if (verbose && m_cpu.IsOpComplete())
        {
            auto addPadding = [](std::size_t p)
            {
                for (auto i = 0; i < p; ++i)
                {
                    std::cout << " ";
                }
            };

            Utils::BusReadVisitor visitor(*this, m_cpu.GetPC(), m_cpu.GetPC() + 3);
            uint16_t dummy;
            auto line = NesEmulator::Utils::Disassemble(visitor, m_cpu.GetPC(), dummy);
            std::cout << line[0];
            // Padding
            std::size_t padding = 30;
            padding -= line[0].size();
            addPadding(padding);
            std::cout << " A:" << std::hex << +m_cpu.GetA();
            std::cout << " X:" << std::hex << +m_cpu.GetX();
            std::cout << " Y:" << std::hex << +m_cpu.GetY();
            std::cout << " P:" << std::hex << +m_cpu.GetStatus().flags;
            std::cout << " SP:" << std::hex << +m_cpu.GetSP();
            int16_t scanlines = m_ppu.GetScanlines();
            int16_t cycles = m_ppu.GetCycles();
            padding = scanlines >= 100 ? 0 : ((scanlines >= 10 || scanlines == -1) ? 1 : 2);
            std::cout << " PPU:";
            addPadding(padding);
            std::cout << std::dec << scanlines << ",";
            padding = cycles >= 100 ? 0 : ((cycles >= 10 || cycles == -1) ? 1 : 2);
            addPadding(padding);
            std::cout << cycles;
            std::cout << " CYC:" << m_cpu.GetNbOfTotalCycles() << std::endl;
        }
        m_cpu.Clock();
    }

    if (m_ppu.IsNMISet())
    {
        m_ppu.ResetNMI();
        m_cpu.NMI();
    }

    m_clockCounter++;
}

void Bus::Reset()
{
    m_clockCounter = 0;
    m_cpu.Reset();
    m_ppu.Reset();
}

void Bus::InsertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    m_cartridge = cartridge;
    m_ppu.ConnectCartridge(cartridge);
}