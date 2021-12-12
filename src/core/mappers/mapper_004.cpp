#include "core/mapper.h"
#include <core/mappers/mapper_004.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_004;

Mapper_004::Mapper_004(const iNESHeader& header)
    : IMapper(header)
{
}

bool Mapper_004::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x6000 && address <= 0x7FFF && m_prgRamEnabled)
    {
        // PrgRam range
        mappedAddress = address & 0x1FFF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        // First bank
        // On mode 0, it is index 6 (R6)
        // On mode 1, it is the second-to-last bank
        if (m_prgRomBankMode == 0)
            mappedAddress = m_indexes[6] * 0x2000 + (address & 0x1FFF);
        else
            mappedAddress = (m_nbPrgBanks * 2 - 2) * 0x2000 + (address & 0x1FFF);

        return true;
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        // Second bank
        // On mode 0 and 1, it is always the index 7 (R7)
        mappedAddress = m_indexes[7] * 0x2000 + (address & 0x1FFF);
        return true;
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
    {
        // Third bank
        // On mode 0, it is the second-to-last
        // On mode 1, it is the index 6 (R6)
        if (m_prgRomBankMode == 1)
            mappedAddress = m_indexes[6] * 0x2000 + (address & 0x1FFF);
        else
            mappedAddress = (m_nbPrgBanks * 2 - 2) * 0x2000 + (address & 0x1FFF);

        return true;
    }
    else if (address >= 0xE000 && address <= 0xFFFF) 
    {
        // Fourth bank
        // It is always the last bank
        mappedAddress = (m_nbPrgBanks * 2 - 1) * 0x2000 + (address & 0x1FFF);
        return true;
    }

    return false;
}

bool Mapper_004::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF && m_prgRamEnabled)
    {
        // PrgRam range
        mappedAddress = address & 0x1FFF;
        return true;
    }
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (address & 0x0001)
        {
            // Change the value of the selected index.
            // Ignore top 2 bits on R6 and R7 (only 64 prg rom banks), and ignore bottom bit 
            // on R0 and R1 (since it is 2kB instead of 1kB for chr rom index)
            if (m_nextIndexToUpdate >= 6)
            {
                data &= 0x3F;
            }
            else if (m_nextIndexToUpdate <= 1)
            {
                data &= 0xFE;
            }
            
            m_indexes[m_nextIndexToUpdate] = data;
        }
        else
        {
            // First 3 bits define the next index to update
            m_nextIndexToUpdate = data & 0x07;

            // 6th bit for the prg mode
            m_prgRomBankMode = (data & 0x40) > 0;

            // 7th bit for the chr mode
            m_chrRomBankMode = (data & 0x80) > 0;
        }

        return true;
    }
    else if (address >= 0xA000 && address <= 0xBFFF) 
    {
        if (address & 0x0001)
        {
            // Don't support write protection
            // PrgRam is on bit 7
            m_prgRamEnabled = (data & 0x80) > 0;
        }
        else
        {
            // Mirroring on bit 0
            m_mirroring = (data & 0x01) ? Mirroring::HORIZONTAL : Mirroring::VERTICAL;
        }
        return true;
    }
    else if (address >= 0xC000)
    {
        // IRQ stuff, not yet supported
        return true;
    }

    return false;
}

bool Mapper_004::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        // On mode 0, it is R0, R1 (with 2kB each) and then R2, R3, R4 and R5 (with 1kB each)
        // On mode 1, it is R2, R3, R4, R5 (with 1kB each) and then R0, R1 (with 2kB each)
        bool isZeroAndOneIndex = (address <= 0x0FFF) ^ m_chrRomBankMode;
        uint8_t index = 0;

        if (isZeroAndOneIndex)
        {
            index = address >= (0x0800 + (m_chrRomBankMode ? 0x1000 : 0x0000)); 
            address &= 0x07FF;
        }
        else
        {
            // Limits are 0x000, 0x400, 0x800 and 0xC00
            // (address & 0x0C00) >> 10 will give us 0, 1, 2 or 3
            // by adding 2, we have R2, R3, R4 and R5
            index = 2 + ((address & 0x0C00) >> 10);
            address &= 0x03FF;
        }

        mappedAddress = m_indexes[index] * 0x0400 + address; 
        return true;
    }

    return false;
}

bool Mapper_004::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    return false;
}

void Mapper_004::Reset()
{
    IMapper::Reset();
    m_indexes.fill(0);
    m_prgRomBankMode = 0;
    m_chrRomBankMode = 0;
    m_nextIndexToUpdate = 0;
    m_prgRamEnabled = true;
}