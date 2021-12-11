#include "core/mapper.h"
#include <core/mappers/mapper_002.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_002;

Mapper_002::Mapper_002(const iNESHeader& header)
    : IMapper(header)
{
    assert(m_nbPrgBanks >= 2 && (m_nbChrBanks == 0 || m_nbChrBanks == 2) && "Wrong number of prgBanks or chrBans in mapper 002");
}

bool Mapper_002::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xBFFF)
    {
        // Low bank address
        mappedAddress = (m_currentSwitchPrgBank * 0x4000) + (address & 0x3FFF);
        return true;
    }
    else if (address >= 0xC000 && address <= 0xFFFF)
    {
        // High bank address, always mapped to the lastest bank on the cartridge
        mappedAddress = (m_nbPrgBanks - 1) * 0x4000 + (address & 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_002::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = address & 0x01FF;
        return true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        m_currentSwitchPrgBank = (data & 0x0F);
    }
    return false;
}

bool Mapper_002::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = address;        
        return true;
    }

    return false;
}

bool Mapper_002::MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t /*data*/)
{
    if (m_nbChrBanks == 0 && address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = address;        
        return true;
    }

    return false;
}

void Mapper_002::Reset()
{
    IMapper::Reset();
    m_currentSwitchPrgBank = 0;
}