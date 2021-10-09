#include <core/bus.h>
#include <iostream>


using NesEmulator::Bus;
using NesEmulator::Processor6502;

Bus::Bus()
{
    // Allocate RAM and set it to 0
    m_ram.resize(64 * 1024); // 64KB
    for (auto& v : m_ram)
        v = 0x00;

    // Connect CPU to this bus
    m_cpu.ConnectBus(this);
}

void Bus::Write(uint16_t address, uint8_t data)
{
    m_ram[address] = data;
}

uint8_t Bus::Read(uint16_t address)
{
    return m_ram[address];
}