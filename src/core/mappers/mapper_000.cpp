#include "core/mapper.h"
#include <core/mappers/mapper_000.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_000;

Mapper_000::Mapper_000(uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring)
    : IMapper(nbPrgBanks, nbChrBanks)
    , m_mirroring(initialMirroring)
{
    // We need to be sure that there is 1 or 2 prgBanks and 1 chrBanks
    assert(nbPrgBanks <= 2 && nbChrBanks == 1 && "Wrong number of prgBanks or chrBans in mapper 000");
}

bool Mapper_000::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
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

bool Mapper_000::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t /*data*/)
{ 
    // Can only write to the PrgRam, located at $6000 to $7FFF, mirroring to fit size 8kB
    // There is also a switch?
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mappedAddress = address & 0x01FF;
        return true;
    }

    return false;
}

bool Mapper_000::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = address;        
        return true;
    }

    return false;
}

bool Mapper_000::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    return false;
}