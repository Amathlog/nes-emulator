#include "core/utils/sha1.h"
#include <core/cartridge.h>
#include <core/utils/visitor.h>
#include <core/mappers/all_mappers.h>
#include <core/constants.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <core/ines.h>

using NesEmulator::Cartridge;
using NesEmulator::Utils::IReadVisitor;


Cartridge::~Cartridge() {}

Cartridge::Cartridge(IReadVisitor& visitor)
{
    // First read the header
    iNESHeader header;
    visitor.Read(&header, 1);

    // Check that the first bytes are the one we expect. If not, perhaps it is not the right
    // format so abort there
    assert(header.isValid() && "The NES header is what we expected. Aborting.");

    // If there is trainer data, ignore it
    if (header.flag6.hasTrainerData)
        visitor.Advance(NesEmulator::Cst::ROM_TRAINER_SIZE);

    m_nbPrgBanks = header.GetPRGROMSize();
    m_prgData.resize(m_nbPrgBanks * NesEmulator::Cst::ROM_PRG_CHUNK_SIZE);
    visitor.Read(m_prgData.data(), m_prgData.size());

    m_nbChrBanks = header.GetCHRROMSize();
    if (m_nbChrBanks > 0)
    {
        m_chrData.resize(m_nbChrBanks * NesEmulator::Cst::ROM_CHR_CHUNK_SIZE);
        visitor.Read(m_chrData.data(), m_chrData.size());
    }
    else 
    {
        m_chrData.resize(0x2000);
    }

    m_vRam.resize(0x2000);

    // Setup the mapper
    m_mapper = NesEmulator::CreateMapper(header);
    m_useVRam = m_mapper->GetMirroring() == Mirroring::FOUR_SCREEN;
    
    assert(m_mapper.get() != nullptr && "Invalid mapper id, unsupported");

    // Allocate 8kB of prgRam
    m_prgRam.resize(0x2000);

    // When all is done, compute the SHA1 of the ROM
    Utils::SHA1 sha1;
    sha1.update(m_prgData);
    sha1.update(m_chrData);
    m_sha1 = sha1.final();
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

        // if (address >= 0x6000 && address <= 0x7FFF)
        //     data = m_prgRam[mappedAddress];
        // else
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
        if (mappedAddress == 0xFFFFFFFF)
            return true;

        m_prgData[mappedAddress] = data;
        return true;
    }
    
    return false;
}

bool Cartridge::WritePPU(uint16_t addr, uint8_t data)
{
    uint32_t mappedAddress = 0;
    // First check if we are currently in Four screen mode, and in the vram/palette range
    if (m_useVRam && addr >= Cst::PPU_START_VRAM && addr <= Cst::PPU_END_PALETTE)
    {
        m_vRam[addr - Cst::PPU_START_VRAM] = data;
        return true;
    }

    if (m_mapper->MapWritePPU(addr, mappedAddress, data))
    {
        m_chrData[mappedAddress] = data;
        return true;
    }
    return false;
}

bool Cartridge::ReadPPU(uint16_t addr, uint8_t& data)
{
    uint32_t mappedAddress = 0;

    // First check if we are currently in Four screen mode, and in the vram/palette range
    if (m_useVRam && addr >= Cst::PPU_START_VRAM && addr <= Cst::PPU_END_PALETTE)
    {
        data = m_vRam[addr - Cst::PPU_START_VRAM];
        return true;
    }

    if (m_mapper->MapReadPPU(addr, mappedAddress, data))
    {
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