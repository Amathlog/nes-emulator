#include "core/mapper.h"
#include <core/mappers/mapper_001.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_001;

Mapper_001::Mapper_001(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring)
    : IMapper(nbPrgBanks, nbChrBanks)
    , m_mirroring(initialMirroring)
{
    // We need to be sure that there is 1 or 2 prgBanks and 1 chrBanks
    assert(nbPrgBanks >= 2 && nbPrgBanks <= 16 && (nbChrBanks == 0 || nbChrBanks >= 2) && nbChrBanks <= 32 && "Wrong number of prgBanks or chrBans in mapper 001");
}

bool Mapper_001::MapReadCPU(uint16_t address, uint32_t& mappedAddress)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xBFFF)
    {
        // Check bank 0
        // If the bank 0 is fixed, it is the first one
        uint8_t bankNumber = (m_32kBModePrgBank || m_PrgBank0IsSwitch) ? m_currentPrgBankSwitch : 0;
        mappedAddress = 0x4000 * bankNumber + (address & 0x3FFF);
        return true;
    }
    else if (address >= 0xC000 && address <= 0xFFFF)
    {
        // Check bank 0
        // If the bank 1 is fixed, it is the last one
        uint8_t bankNumber = 0;
        if (m_32kBModePrgBank)
            bankNumber = m_currentPrgBankSwitch + 1;
        else if (!m_PrgBank0IsSwitch)
            bankNumber = m_currentPrgBankSwitch;
        else
            bankNumber = m_nbPrgBanks - 1;

        mappedAddress = 0x4000 * bankNumber + (address & 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_001::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mappedAddress = address & 0x01FF;
        return true;
    }
    if (address >= 0x8000)
    {
        // In this case, we need to load some data in the shift register.
        // The address doesn't mean anything.
        // It's when the shift register is full that the address is important.
        HandleLoadRegister(data);
        if (m_loadRegisterDone)
        {
            // Control (0x8000-0x9FFF)
            if (address <= 0x9FFF)
            {
                uint8_t mirroring = m_internalRegister & 0x03;
                uint8_t prgROMBank = (m_internalRegister >> 2) & 0x03;
                uint8_t chrROMBank = (m_internalRegister >> 4) & 0x01;

                // First check if we switch our prg ROM bank
                m_32kBModePrgBank = prgROMBank <= 1;
                if (prgROMBank == 2)
                {
                    // Fix the bank 0 to the first bank in prg data
                    m_PrgBank0IsSwitch = false;
                }
                else if (prgROMBank == 3)
                {
                    // Fix the bank 1 to the last bank in prg data
                    m_PrgBank0IsSwitch = true;
                }

                // Second check for CHR bank
                m_8kBModeChrBank = chrROMBank == 0;

                // Finally check the mirroring
                // TODO: check 0 and 1 value
                switch (mirroring) 
                {
                case 0:
                case 1:
                    // TODO
                    break;
                case 2:
                    m_mirroring = Mirroring::VERTICAL;
                    break;
                case 3:
                    m_mirroring = Mirroring::HORIZONTAL;
                    break;
                default:
                    break;
                }
            }
            // CHR Bank 0 (0xA000-0xBFFF)
            else if(address <= 0xBFFF)
            {
                uint8_t chrBank0 = m_internalRegister & 0x1F;
                // Ignore lowest bit in 8kB mode
                m_currentChrBank0 = m_8kBModeChrBank ? (chrBank0 & 0xF2) : chrBank0;
            }
            // CHR Bank 1 (0xC000-0xDFFF)
            else if (address <= 0xDFFF)
            {
                uint8_t chrBank1 = m_internalRegister & 0x1F;
                // Ignored in 8kB mode
                if (!m_8kBModeChrBank)
                {
                    m_currentChrBank1 = chrBank1;
                }
            }
            // PRG Bank (0xE000-0xFFFF)
            else 
            {
                uint8_t prgROMBank = m_internalRegister & 0x0F;
                uint8_t prgRAMChipEnable = (m_internalRegister & 0x10) > 0;

                // Low bit ignored in 32kB mode
                m_currentPrgBankSwitch = m_32kBModePrgBank ? (prgROMBank & 0xF2) : prgROMBank;
            }
        }
    }

    return false;
}

bool Mapper_001::MapReadPPU(uint16_t address, uint32_t& mappedAddress)
{
    // First check if we have Chr bank. If not, we need to read from RAM
    if (m_nbChrBanks == 0)
    {
        mappedAddress = (address & 0x1FFF);
        return true;
    }
    else
    {
        if (address >= 0x0000 && address <= 0x0FFF)
        {
            mappedAddress = 0x1000 * m_currentChrBank0 + (address & 0x0FFF);        
            return true;
        }
        else if (address >= 0x1000 && address <= 0x1FFF)
        {
            uint8_t bankNumber = m_8kBModeChrBank ? (m_currentChrBank0 + 1) : m_currentChrBank1;
            mappedAddress = 0x1000 * bankNumber + (address & 0x0FFF);
            return true;
        }
    }

    return false;
}

bool Mapper_001::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    return false;
}

void Mapper_001::ResetShiftRegister()
{
    m_shiftRegister = 0x01;
}

void Mapper_001::HandleLoadRegister(uint8_t data)
{
    if ((m_shiftRegister & 0x10) > 0)
    {
        m_loadRegisterDone = true;
        m_internalRegister = ((m_shiftRegister << 1) | (data & 0x01)) & 0x1F;
        ResetShiftRegister();
    }
    else
    {
        m_shiftRegister = (m_shiftRegister << 1) | (data & 0x01);
    }
}