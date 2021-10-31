#include <core/processor2C02.h>
#include <core/cartridge.h>
#include <cstring>
#include <cstdlib>

using NesEmulator::Processor2C02;

Processor2C02::Processor2C02()
{
    Reset();
}

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
    // For now some random noise is added at each clock.
    static unsigned i = 0, j = 0;
    m_screen[i][j] = rand() % 2 == 0 ? 0x30 : 0x0f;
    j++;
    if (j == GetWidth())
    {
        j = 0;
        i = (i + 1) % GetHeight();
    }
}

void Processor2C02::Reset()
{
    memset(m_screen, 0, GetWidth()*GetHeight());
    m_namedTables.fill({0});
    m_paletteTable.fill({0});
}

void Processor2C02::RandomizeScreen()
{
    for(int i = 0; i < GetWidth(); ++i)
    {
        for (int j = 0; j < GetHeight(); ++j)
        {
            m_screen[i][j] = rand() % 2 == 0 ? 0x30 : 0x0f;
        }
    }
}

uint8_t Processor2C02::GetColorFromPaletteRam(uint8_t n, uint8_t i) const
{
    return n * 4 + i;
}

void Processor2C02::FillFromPatternTable(uint8_t index, uint8_t /*selectedPalette*/, uint8_t* buffer) const
{
    memcpy(buffer, m_namedTables[index].data(), m_namedTables[index].size());
}