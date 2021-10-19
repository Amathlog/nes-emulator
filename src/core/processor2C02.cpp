#include <core/processor2C02.h>
#include <core/cartridge.h>

using NesEmulator::Processor2C02;

uint8_t Processor2C02::ReadCPU(uint16_t addr)
{
    uint8_t data = 0;
    switch(addr)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    }

    return data;
}

void Processor2C02::WriteCPU(uint16_t addr, uint8_t data)
{
    data = 0;
    switch(addr)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    }
}

void Processor2C02::WritePPU(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    if (m_cartridge->WritePPU(addr, data))
    {
        // Nothing to do
    }
}

uint8_t Processor2C02::ReadPPU(uint16_t addr)
{
    uint8_t data = 0;
    addr &= 0x3FFF;
    if (m_cartridge->ReadPPU(addr, data))
    {
        // Nothing to do
    }
    return data;
}

void Processor2C02::Clock()
{
    // TODO
}