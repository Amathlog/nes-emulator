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
    assert(nbPrgBanks <= 2 && nbChrBanks == 1 && "Wrong number of prgBanks or chrBans in mapper 000");
}

bool Mapper_001::MapReadCPU(uint16_t address, uint32_t& mappedAddress)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        // Prg
        mappedAddress = address & (m_nbPrgBanks == 2 ? 0x7FFF : 0x3FFF);
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
            // Control
            if (address <= 0x9FFF)
            {

            }
        }
    }

    return false;
}

bool Mapper_001::MapReadPPU(uint16_t address, uint32_t& mappedAddress)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = address;        
        return true;
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