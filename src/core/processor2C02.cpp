#include <core/processor2C02.h>
#include <core/cartridge.h>
#include <cstdint>
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
        data = m_registers.ctrl.flags;
        break;
    case 1:
        data = m_registers.mask.flags;
        break;
    case 2:
        data = m_registers.status.flags;
        break;
    case 3:
        data = m_registers.oamaddr;
        break;
    case 4:
        data = m_registers.oamdata;
        break;
    case 5:
        data = m_registers.scroll;
        break;
    case 6:
        data = m_registers.addr;
        break;
    case 7:
        data = m_registers.data;
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
        m_registers.ctrl.flags = data;
        break;
    case 1:
        m_registers.mask.flags = data;
        break;
    case 2:
        m_registers.status.flags = data;
        break;
    case 3:
        m_registers.oamaddr = data;
        break;
    case 4:
        m_registers.oamdata = data;
        break;
    case 5:
        m_registers.scroll = data;
        break;
    case 6:
        m_registers.addr = data;
        break;
    case 7:
        m_registers.data = data;
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
    else if(addr >= 0x000 && addr <= 0x1F00)
    {
        m_namedTables[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr &= 0x0007;
        WriteCPU(addr, data);
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr &= 0x001F;
        // Mirroring, to review
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;

        m_paletteTable[addr] = data;
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
    else if(addr >= 0x000 && addr <= 0x1F00)
    {
        data = m_namedTables[(addr & 0x1000) >> 12][addr & 0x0FFF];
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr &= 0x0007;
        data = ReadCPU(addr);
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr &= 0x001F;
        // Mirroring, to review
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;

        data = m_paletteTable[addr];
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
    m_registers.Reset();
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

uint8_t Processor2C02::GetColorFromPaletteRam(uint8_t n, uint8_t i)
{
    uint16_t addr = 0x3F00 + n * 4 + i;
    return ReadPPU(addr);
}

void Processor2C02::FillFromPatternTable(uint8_t index, uint8_t selectedPalette, uint8_t* buffer)
{
    // memcpy(buffer, m_namedTables[index].data(), m_namedTables[index].size());
    for (uint16_t tileY = 0; tileY < 16; ++tileY)
    {
        for (uint16_t tileX = 0; tileX < 16; ++tileX)
        {
            uint16_t offset = tileY * 256 + tileX * 16;
            for (uint8_t row = 0; row < 8; ++row)
            {
                uint16_t addr = index * 0x1000 + offset + row;
                uint8_t lowPixel = ReadPPU(addr);
                uint8_t highPixel = ReadPPU(addr + 1);

                for (uint8_t col = 0; col < 8; ++col)
                {
                    uint8_t pixelValue = ((highPixel & 0x01) << 1) | (lowPixel & 0x01);
                    lowPixel >>= 1;
                    highPixel >>= 1;

                    uint16_t x = tileX * 8 + (7 - col);
                    uint16_t y = tileY * 8 + row;
                    uint16_t value = y * 128 + x;
                    buffer[value] = GetColorFromPaletteRam(selectedPalette, pixelValue);
                }
            }
        }
    }
}