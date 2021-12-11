#include "core/mapper.h"
#include <core/mappers/mapper_004.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_004;

Mapper_004::Mapper_004(const iNESHeader& header)
    : IMapper(header)
{
    assert(m_nbPrgBanks > 0 && m_nbPrgBanks <= 2 && (m_nbChrBanks >= 2) && "Wrong number of prgBanks or chrBans in mapper 004");
}

bool Mapper_004::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
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

bool Mapper_004::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    if (address >= 0x8000 && address <= 0xFFFF)
    {
    }
    return false;
}

bool Mapper_004::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
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
}