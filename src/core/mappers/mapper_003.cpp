#include "core/mapper.h"
#include <core/mappers/mapper_003.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_003;

Mapper_003::Mapper_003(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring)
    : IMapper(nbPrgBanks, nbChrBanks)
    , m_mirroring(initialMirroring)
{
    assert(nbPrgBanks > 0 && nbPrgBanks <= 2 && (nbChrBanks >= 2) && "Wrong number of prgBanks or chrBans in mapper 003");
}

bool Mapper_003::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && (address <= 0xBFFF || m_nbPrgBanks == 1))
    {
        // Low bank address (or mirroring high bank if there is only one prgBank)
        mappedAddress = address & 0x3FFF;
        return true;
    }
    else if (address >= 0xC000 && address <= 0xFFFF)
    {
        // High bank address
        mappedAddress = address & 0x7FFF;
        return true;
    }

    return false;
}

bool Mapper_003::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    if (address >= 0x8000 && address <= 0xFFFF)
    {
        m_currentSwitchChrBank = data;
    }
    return false;
}

bool Mapper_003::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = m_currentSwitchChrBank * 0x2000 + address;        
        return true;
    }

    return false;
}

bool Mapper_003::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    return false;
}

void Mapper_003::Reset()
{
    m_currentSwitchChrBank = 0;
}