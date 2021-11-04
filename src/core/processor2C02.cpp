#include "core/constants.h"
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
        // Can't read from Ctrl register
        break;
    case 1:
        // Can't read from Mask register
        break;
    case 2:
        // HACK: Uncomment if you want to force the verticalBlank state
        // m_registers.status.verticalBlankStarted = 1;
        data = (m_registers.status.flags & 0xE0) | (m_registers.data & 0x1F);
        m_registers.status.verticalBlankStarted = 0;
        m_registers.addr = 0;
        break;
    case 3:
        // Can't read from OAMAddress register
        break;
    case 4:
        data = m_registers.oamdata;
        break;
    case 5:
        // Can't read from Scroll register
        break;
    case 6:
        // Can't read from Addr register
        break;
    case 7:
    {
        // delay read
        data = m_registers.data;
        m_registers.data = ReadPPU(m_registers.fullAddress);

        // Note: In case of reading to the palette ram, there is no delay
        // Therefore skip the delay if we are in this range (with mirroring)
        uint16_t tempAddr = m_registers.fullAddress &= Cst::PPU_MASK_MIRROR;
        if (tempAddr >= Cst::PPU_START_PALETTE && tempAddr <= Cst::PPU_END_PALETTE)
            data = m_registers.data;

        m_registers.fullAddress += (m_registers.ctrl.VRAMAddressIncrement ? 32 : 1);

        break;
    }
    }

    return data;
}

void Processor2C02::WriteCPU(uint16_t addr, uint8_t data)
{
    switch(addr)
    {
    case 0:
        m_registers.ctrl.flags = data;
        break;
    case 1:
        m_registers.mask.flags = data;
        break;
    case 2:
        // Can't write to Status register
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
        if (m_registers.addr == 0)
        {
            m_registers.fullAddress = (m_registers.fullAddress & 0x00FF) | (data << 8);
            m_registers.addr = 1;
        }
        else 
        {
            m_registers.fullAddress = (m_registers.fullAddress & 0xFF00) | data;
            m_registers.addr = 0;
        }
        break;
    case 7:
        WritePPU(m_registers.fullAddress, data);
        m_registers.fullAddress += (m_registers.ctrl.VRAMAddressIncrement ? 32 : 1);
        break;
    }
}

void Processor2C02::WritePPU(uint16_t addr, uint8_t data)
{
    addr &= Cst::PPU_MASK_MIRROR;
    if (m_cartridge->WritePPU(addr, data))
    {
        // Nothing to do
    }
    else if(addr >= Cst::PPU_START_CHR_ROM && addr <= Cst::PPU_END_CHR_ROM)
    {
        m_namedTables[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    else if (addr >= Cst::PPU_START_VRAM && addr <= Cst::PPU_END_VRAM)
    {
        addr &= 0x0FFF;
        // if (m_cartridge->GetMirroring() == Mirroring::VERTICAL)
        {
            // Vertical
            if (addr >= 0x0000 && addr <= 0x03FF)
                m_namedTables[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF)
                m_namedTables[1][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF)
                m_namedTables[0][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF)
                m_namedTables[1][addr & 0x03FF] = data;
        }
        // else 
        // {
        
        // }
    }
    else if (addr >= Cst::PPU_START_PALETTE && addr <= Cst::PPU_END_PALETTE)
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
    addr &= Cst::PPU_MASK_MIRROR;
    if (m_cartridge->ReadPPU(addr, data))
    {
        // Nothing to do
    }
    else if(addr >= Cst::PPU_START_CHR_ROM && addr <= Cst::PPU_END_CHR_ROM)
    {
        data = m_namedTables[(addr & 0x1000) >> 12][addr & 0x0FFF];
    }
    else if (addr >= Cst::PPU_START_VRAM && addr <= Cst::PPU_END_VRAM)
    {
        addr &= 0x0FFF;
        // if (m_cartridge->GetMirroring() == Mirroring::VERTICAL)
        {
            // Vertical
            if (addr >= 0x0000 && addr <= 0x03FF)
                data = m_namedTables[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF)
                data = m_namedTables[1][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF)
                data = m_namedTables[0][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF)
                data = m_namedTables[1][addr & 0x03FF];
        }
        // else 
        // {
        // }
    }
    else if (addr >= Cst::PPU_START_PALETTE && addr <= Cst::PPU_END_PALETTE)
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
//     static unsigned i = 0, j = 0;
//     m_screen[i][j] = rand() % 2 == 0 ? 0x30 : 0x0f;
//     j++;
//     if (j == GetWidth())
//     {
//         j = 0;
//         i = (i + 1) % GetHeight();
//     }
    m_cycles++;
    if (m_cycles >= 341)
    {
        m_cycles = 0;
        m_scanlines++;
        if (m_scanlines >= 261)
        {
            m_scanlines = -1;
            m_isFrameComplete = true;
        }
        else if (m_scanlines == 0) 
        {
            m_isFrameComplete = false;
        }
    }
    else if (m_scanlines == 241 && m_cycles == 1)
    {
        m_registers.status.verticalBlankStarted = 1;
        if (m_registers.ctrl.generateNMIWhenVBI)
            m_nmi = true;
    }
    else if (m_scanlines == -1 && m_cycles == 1) {
        m_registers.status.verticalBlankStarted = 0;
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
    uint16_t addr = 0x3F00 + (n * 4 + i);
    return ReadPPU(addr) & 0x3F;
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
                // It seems in the rom, it stores 64 bits of lowPixel and then 64 bits of
                // highPixel. Therefore, we need to offset our read for high by 8 bytes.
                uint8_t lowPixel = ReadPPU(addr);
                uint8_t highPixel = ReadPPU(addr + 8);

                for (uint8_t col = 0; col < 8; ++col)
                {
                    uint8_t pixelValue = ((highPixel & 0x01) << 1) | (lowPixel & 0x01);
                    lowPixel >>= 1;
                    highPixel >>= 1;

                    uint16_t x = tileX * 8 + (7 - col);
                    uint16_t y = tileY * 8 + row;
                    uint16_t value = y * 128 + x;
                    uint8_t pixelColor = GetColorFromPaletteRam(selectedPalette, pixelValue);
                    buffer[value] = pixelColor;
                }
            }
        }
    }
}