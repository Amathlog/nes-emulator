#include "core/mapper.h"
#include <core/mappers/mapper_001.h>
#include <cassert>
#include <cstdint>

using NesEmulator::Mapper_001;

Mapper_001::Mapper_001(const iNESHeader& header)
    : IMapper(header)
{
    // We need to be sure that there is 1 or 2 prgBanks and 1 chrBanks
    assert(m_nbPrgBanks >= 2 && m_nbPrgBanks <= 16 && (m_nbChrBanks == 0 || m_nbChrBanks >= 2) && m_nbChrBanks <= 32 && "Wrong number of prgBanks or chrBans in mapper 001");

    m_staticRAM.resize(32 * 1024); // 32kB
}

bool Mapper_001::MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data)
{
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        // PrgRam range
        mappedAddress = 0xFFFFFFFF;
        data = m_staticRAM[address & 0x1FFF];
        return true;
    }
    else if (address >= 0x8000)
    {
        if (m_32kBModePrgBank)
        {
            mappedAddress = m_currentPrgBankSwitch * 0x8000 + (address & 0x7FFF);
        }
        else
        {
            uint8_t bankNumber = 0;
            if (address <= 0xBFFF)
            {
                // Low bank
                // If bank 0 is fixed, it is the first one
                bankNumber = m_PrgBank0IsSwitch ? m_currentPrgBankSwitch : 0;
            }
            else
            {
                // High bank
                // If bank 1 is fixed, it is the last one
                bankNumber = !m_PrgBank0IsSwitch ? m_currentPrgBankSwitch : m_nbPrgBanks - 1; 
            }

            mappedAddress = bankNumber * 0x4000 + (address & 0x3FFF);
        }
        return true;
    }

    return false;
}

bool Mapper_001::MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data)
{ 
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mappedAddress = 0xFFFFFFFF;
        m_staticRAM[address & 0x1FFF] = data;
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
                m_currentChrBank0 = m_8kBModeChrBank ? (chrBank0 >> 1) : chrBank0;
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
                //uint8_t prgRAMChipEnable = (m_internalRegister & 0x10) > 0;

                // Low bit ignored in 32kB mode
                m_currentPrgBankSwitch = m_32kBModePrgBank ? (prgROMBank >> 1) : prgROMBank;
            }

            m_loadRegisterDone = false;
        }
    }

    return false;
}

bool Mapper_001::MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& /*data*/)
{
    // First check if we have Chr bank. If not, we need to read from RAM
    if (address <= 0x1FFF)
    {
        if (m_nbChrBanks == 0)
        {
            mappedAddress = address;
            return true;
        }

        if (m_8kBModeChrBank)
        {
            mappedAddress = m_currentChrBank0 * 0x2000 + (address & 0x1FFF);
        }
        else 
        {
            uint8_t bankNumber = address <= 0x0FFF ? m_currentChrBank0 : m_currentChrBank1;
            mappedAddress = 0x1000 * bankNumber + (address & 0x0FFF);
        }

        return true;
    }

    return false;
}

bool Mapper_001::MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t /*data*/)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        if (m_nbChrBanks == 0)
            mappedAddress = address;

        return true;
    }

    return false;
}

void Mapper_001::ResetShiftRegister()
{
    m_shiftRegister = 0x00;
    m_shiftCounter = 0;
}

void Mapper_001::ResetControlRegister()
{
    m_PrgBank0IsSwitch = true;
    m_32kBModePrgBank = false;
    m_8kBModeChrBank = false;
    m_mirroring = m_originalMirroring;
}

void Mapper_001::HandleLoadRegister(uint8_t data)
{
    // If the highest bit is set, we just reset
    if ((data & 0x80) > 0)
    {
        ResetShiftRegister();
        // Control register is also reset
        ResetControlRegister();
        return;
    }
    
    // We receive bits, starting with LSB. So we need this shift gymnastic.
    m_shiftRegister |= ((data & 0x01) << m_shiftCounter);

    if (++m_shiftCounter == 5)
    {
        m_loadRegisterDone = true;
        m_internalRegister = m_shiftRegister & 0x1F;
        ResetShiftRegister();
    }
}

void Mapper_001::Reset()
{
    ResetShiftRegister();
    m_internalRegister = 0x00;
    m_loadRegisterDone = false;

    m_currentPrgBankSwitch = 0;
    m_currentChrBank0 = 0;
    m_currentChrBank1 = 0;
    ResetControlRegister();
}

void Mapper_001::SerializeTo(Utils::IWriteVisitor& visitor) const
{
    IMapper::SerializeTo(visitor);

    visitor.WriteValue(m_shiftRegister);
    visitor.WriteValue(m_shiftCounter);
    visitor.WriteValue(m_internalRegister);
    visitor.WriteValue(m_loadRegisterDone);

    visitor.WriteValue(m_currentPrgBankSwitch);
    visitor.WriteValue(m_PrgBank0IsSwitch);
    visitor.WriteValue(m_currentChrBank0);
    visitor.WriteValue(m_currentChrBank1);
    visitor.WriteValue(m_32kBModePrgBank);
    visitor.WriteValue(m_8kBModeChrBank);
}

void Mapper_001::DeserializeFrom(Utils::IReadVisitor& visitor)
{
    IMapper::DeserializeFrom(visitor);

    visitor.ReadValue(m_shiftRegister);
    visitor.ReadValue(m_shiftCounter);
    visitor.ReadValue(m_internalRegister);
    visitor.ReadValue(m_loadRegisterDone);

    visitor.ReadValue(m_currentPrgBankSwitch);
    visitor.ReadValue(m_PrgBank0IsSwitch);
    visitor.ReadValue(m_currentChrBank0);
    visitor.ReadValue(m_currentChrBank1);
    visitor.ReadValue(m_32kBModePrgBank);
    visitor.ReadValue(m_8kBModeChrBank);
}