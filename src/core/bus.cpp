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
    m_ram.resize(Cst::RAM_SIZE); // 2KB
    for (auto& v : m_ram)
        v = 0x00;

    // Connect CPU to this bus
    m_cpu.ConnectBus(this);
}

void Bus::Write(uint16_t address, uint8_t data)
{
    if (address >= Cst::RAM_START_ADDR && address <= Cst::RAM_END_ADDR)
    {
        // Mirroring. Map to 0x0000 to 0x0800
        m_ram[address % Cst::RAM_SIZE] = data;
    }
}

uint8_t Bus::Read(uint16_t address)
{
    if (address >= Cst::RAM_START_ADDR && address <= Cst::RAM_END_ADDR)
    {
        // Mirroring. Map to 0x0000 to 0x0800
        return m_ram[address % Cst::RAM_SIZE];
    }

    return 0;
}