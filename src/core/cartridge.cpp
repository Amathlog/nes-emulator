#include <core/cartridge.h>
#include <core/utils/visitor.h>
#include <core/mappers/all_mappers.h>
#include <core/constants.h>
#include <cassert>
#include <cstring>

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

    m_prgData.resize(header.prgRomSize * NesEmulator::Cst::ROM_PRG_CHUNK_SIZE);
    visitor.Read(m_prgData.data(), m_prgData.size());

    if (header.chrRomSize > 0)
    {
        m_chrData.resize(header.chrRomSize * NesEmulator::Cst::ROM_CHR_CHUNK_SIZE);
        visitor.Read(m_chrData.data(), m_chrData.size());
    }

    // Setup the mapper
    uint8_t mapperId = (header.flag7 & 0xF0) | ((header.flag6 & 0xF0) >> 4);
    m_mapper = NesEmulator::CreateMapper(mapperId);
    
    assert(m_mapper.get() != nullptr && "Invalid mapper id, unsupported");
}