#include "core/mapper.h"
#include <core/mappers/mapper_066.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_066;

Mapper_066::Mapper_066(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring)
    : IMapper(nbPrgBanks, nbChrBanks)
    , m_mirroring(initialMirroring)
{
    assert(nbPrgBanks >= 2 && nbChrBanks >= 2 && "Wrong number of prgBanks or chrBans in mapper 066");
}

bool Mapper_066::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        // Low bank address
        mappedAddress = (m_currentSwitchPrgBank * 0x8000) + (address & 0x7FFF);
        return true;
    }

    return false;
}

bool Mapper_066::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        m_currentSwitchChrBank = (data & 0x03);
        m_currentSwitchPrgBank = ((data >> 4) & 0x03);
    }
    return false;
}

bool Mapper_066::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = m_currentSwitchChrBank * 0x2000 + address;        
        return true;
    }

    return false;
}

bool Mapper_066::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    // if (m_nbChrBanks == 0 && address >= 0x0000 && address <= 0x1FFF)
    // {
    //     mappedAddress = address;        
    //     return true;
    // }

    return false;
}

void Mapper_066::Reset()
{
    m_currentSwitchPrgBank = 0;
    m_currentSwitchChrBank = 0;
}