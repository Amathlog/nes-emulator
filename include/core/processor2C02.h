#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <core/constants.h>

namespace NesEmulator
{
    class Cartridge;

    class Processor2C02
    {
    public:
        Processor2C02() = default;
        ~Processor2C02() = default;

        void WriteCPU(uint16_t addr, uint8_t data);
        uint8_t ReadCPU(uint16_t addr);

        void WritePPU(uint16_t addr, uint8_t data);
        uint8_t ReadPPU(uint16_t addr);

        void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) { m_cartridge = cartridge; }
        void Clock();

    private:
        std::shared_ptr<Cartridge> m_cartridge;

        // Memory
        std::array<std::array<uint8_t, Cst::PPU_NAMED_TABLES_SIZE>, Cst::PPU_NB_NAMED_TABLES> m_namedTables;
        std::array<uint8_t, Cst::PPU_PALETTE_SIZE> m_paletteTable;
    };
}