#include <core/bus.h>
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
        m_cpuRam[address % Cst::PPU_REG_SIZE] = data;
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
        // Mirroring. Map to 0x0000 to 0x0800
        data = m_cpuRam[address % Cst::RAM_SIZE];
    }
    else if (address >= Cst::PPU_REG_START_ADDR && address <= Cst::PPU_REG_END_ADDR)
    {
        // Mirroring.
        data = m_cpuRam[address % Cst::PPU_REG_SIZE];
    }

    return data;
}

void Bus::Clock()
{
    // PPU runs 3 times faster than the CPU
    m_ppu.Clock();
    if (m_clockCounter % 3 == 0)
        m_cpu.Clock();

    m_clockCounter++;
}

void Bus::Reset()
{
    m_clockCounter = 0;
    m_cpu.Reset();
}

void Bus::InsertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    m_cartridge = cartridge;
    m_ppu.ConnectCartridge(cartridge);
}