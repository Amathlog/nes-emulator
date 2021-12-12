#include "core/mapper.h"
#include <core/mappers/mapper_040.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_040;

Mapper_040::Mapper_040(const iNESHeader& header)
    : IMapper(header)
{
}

bool Mapper_040::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // "RAM" is bank 6
        mappedAddress = 6 * 0x2000 + (address & 0x1FFF);
        return true;
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        // Bank 4
        mappedAddress = 4 * 0x2000 + (address & 0x1FFF);
        return true;
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        // Bank 5
        mappedAddress = 5 * 0x2000 + (address & 0x1FFF);
        return true;
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
    {
        // Switchable bank
        mappedAddress = m_switchableBank * 0x2000 + (address & 0x1FFF);
        return true;
    }
    else if (address >= 0xE000 && address <= 0xFFFF) 
    {
        // Bank 7 (last one)
        mappedAddress = 7 * 0x2000 + (address & 0x1FFF);
        return true;
    }

    return false;
}

bool Mapper_040::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // Nothing here
    }
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        // Disable and reset IRQ counter
        m_IRQEnabled = false;
        m_IRQActive = false;
        m_IRQCounter = 0;
    }
    else if (address >= 0xA000 && address <= 0xBFFF) 
    {
        // Enable IRQ Counter
        m_IRQEnabled = true;
        m_IRQCounter = 4096u;
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
    {
        // Nothing here
    }
    else if (address >= 0xE000 && address <= 0xFFFF)
    {
        // Select switchable bank. There is only 8 of them
        m_switchableBank = data & 0x07;
    }

    return false;
}

bool Mapper_040::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        mappedAddress = address;
        return true;
    }

    return false;
}

bool Mapper_040::MapWritePPU(uint16_t /*address*/, uint32_t& /*mappedAddress*/, uint8_t /*data*/)
{
    return false;
}

void Mapper_040::Reset()
{
    IMapper::Reset();
    m_switchableBank = 0;
    m_IRQEnabled = false;
    m_IRQActive = false;
    m_IRQCounter = 0;
}

void Mapper_040::CPUClock()
{
    if (m_IRQCounter == 0 || !m_IRQEnabled)
    {
        return;
    }

    if (--m_IRQCounter == 0 && m_IRQEnabled)
    {
        m_IRQActive = true;
    }
}

bool Mapper_040::ShouldIRQ() const
{
    return m_IRQActive;
}

void Mapper_040::ClearIRQ()
{
    m_IRQActive = false;
}