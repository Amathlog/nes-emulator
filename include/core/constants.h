#include <cstdint>

namespace NesEmulator
{
namespace Cst
{
    // Bus addresses
    constexpr uint16_t RAM_START_ADDR = 0x0000;
    constexpr uint16_t RAM_END_ADDR = 0x1FFF;
    constexpr uint16_t RAM_SIZE = 0x0800; // 8kB addr range but only 2kB RAM. Mirroring

    constexpr uint16_t PPU_REG_START_ADDR = 0x2000;
    constexpr uint16_t PPU_REG_END_ADDR = 0x3FFF;
    constexpr uint16_t PPU_REG_SIZE = 0x0008; // 8kB addr range, but only 8 bytes usable. Mirroring

    constexpr uint16_t APU_IO_START_ADDR = 0x4000;
    constexpr uint16_t APU_IO_END_ADDR = 0x4017;
    constexpr uint16_t APU_IO_SIZE = APU_IO_END_ADDR - APU_IO_START_ADDR;

    constexpr uint16_t ROM_START_ADDR = 0x4020;
    constexpr uint16_t ROM_END_ADDR = 0xFFFF;
    constexpr uint16_t ROM_SIZE = ROM_END_ADDR - ROM_START_ADDR;

    // Roms constants, following iNES format (https://wiki.nesdev.org/w/index.php/INES)
    constexpr char NES_HEADER[5] = "\x4E\x45\x53\x1A";
    constexpr uint16_t ROM_PRG_CHUNK_SIZE = 0x4000;
    constexpr uint16_t ROM_CHR_CHUNK_SIZE = 0x2000;

    constexpr uint16_t ROM_TRAINER_SIZE = 0x200;
}
}