#include <core/cartridge.h>
#include <core/utils/visitor.h>
#include <core/mappers/all_mappers.h>
#include <core/constants.h>
#include <cassert>
#include <cstring>
#include <iostream>

using NesEmulator::Cartridge;
using NesEmulator::Utils::IReadVisitor;

Cartridge::~Cartridge() {}

Cartridge::Cartridge(IReadVisitor& visitor)
{
    // First read the header
    struct Header
    {
        char nesName[4];
        uint8_t prgRomSize;
        uint8_t chrRomSize;
        uint8_t flag6;
        uint8_t flag7;
        uint8_t flag8;
        uint8_t flag9;
        uint8_t flag10;
        uint8_t unused[5];
    } header;

    visitor.Read(&header, 1);

    // Check that the first bytes are the one we expect. If not, perhaps it is not the right
    // format so abort there
    assert(strncmp(header.nesName, NesEmulator::Cst::NES_HEADER, 4) == 0 && "The NES header is what we expected. Aborting.");

    // If there is trainer data, ignore it
    if ((header.flag6 & 0x04) == 0x04)
        visitor.Advance(NesEmulator::Cst::ROM_TRAINER_SIZE);

    // Mirorring is the less significant bit of the flag6
    Mirroring initialMirroring = (header.flag6 & 0x01) ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;

    m_nbPrgBanks = header.prgRomSize;
    m_prgData.resize(m_nbPrgBanks * NesEmulator::Cst::ROM_PRG_CHUNK_SIZE);
    visitor.Read(m_prgData.data(), m_prgData.size());

    if (header.chrRomSize > 0)
    {
        m_nbChrBanks = header.chrRomSize;
        m_chrData.resize(m_nbChrBanks * NesEmulator::Cst::ROM_CHR_CHUNK_SIZE);
        visitor.Read(m_chrData.data(), m_chrData.size());
    }

    // Setup the mapper
    uint8_t mapperId = (header.flag7 & 0xF0) | ((header.flag6 & 0xF0) >> 4);
    m_mapper = NesEmulator::CreateMapper(mapperId, m_nbPrgBanks, m_nbChrBanks, initialMirroring);
    
    assert(m_mapper.get() != nullptr && "Invalid mapper id, unsupported");

    // Allocate 8kB of prgRam
    m_prgRam.resize(0x2000);
}

bool Cartridge::ReadCPU(uint16_t address, uint8_t& data)
{
    uint32_t mappedAddress = 0;
    if (m_mapper->MapReadCPU(address, mappedAddress, data))
    {
        if (mappedAddress == 0xFFFFFFFF)
        {
            return true;
        }

        if (address >= 0x6000 && address <= 0x7FFF)
            data = m_prgRam[mappedAddress];
        else
            data = m_prgData[mappedAddress];

        return true;
    }

    return false;
}

bool Cartridge::WriteCPU(uint16_t addr, uint8_t data)
{
    uint32_t mappedAddress = 0;
    if (m_mapper->MapWriteCPU(addr, mappedAddress, data))
    {
        if (mappedAddress != 0xFFFFFFFF)
            m_prgRam[mappedAddress] = data;

        return true;
    }
    
    return false;
}

bool Cartridge::WritePPU(uint16_t addr, uint8_t data)
{
    uint32_t mappedAddress = 0;
    if (m_mapper->MapWritePPU(addr, mappedAddress, data))
    {
        // If we have no chrBank, we need to write to RAM
        if (m_nbChrBanks == 0)
        {
            m_prgRam[mappedAddress] = data;
            return true;
        }
    }
    return false;
}

bool Cartridge::ReadPPU(uint16_t addr, uint8_t& data)
{
    uint32_t mappedAddress = 0;
    if (m_mapper->MapReadPPU(addr, mappedAddress, data))
    {
        // If we have no chrBank, we need to read from RAM
        if (m_nbChrBanks == 0)
            data = m_prgRam[mappedAddress];
        else
            data = m_chrData[mappedAddress];
        return true;
    }

    return false;
}

void Cartridge::Reset()
{
    if (m_mapper)
        m_mapper->Reset();
}