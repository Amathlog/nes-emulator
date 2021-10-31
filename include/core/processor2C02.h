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
        Processor2C02();
        ~Processor2C02() = default;

        void WriteCPU(uint16_t addr, uint8_t data);
        uint8_t ReadCPU(uint16_t addr);

        void WritePPU(uint16_t addr, uint8_t data);
        uint8_t ReadPPU(uint16_t addr);

        void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) { m_cartridge = cartridge; }
        void Clock();
        void Reset();

        const uint8_t* GetScreen() const { return reinterpret_cast<const uint8_t*>(m_screen); }
        constexpr unsigned GetWidth() const { return 256u; }
        constexpr unsigned GetHeight() const { return 240u; }
        bool IsFrameComplete() const { return m_isFrameComplete; }

        void RandomizeScreen();
        
    private:
        std::shared_ptr<Cartridge> m_cartridge;

        // Memory
        std::array<std::array<uint8_t, Cst::PPU_NAMED_TABLES_SIZE>, Cst::PPU_NB_NAMED_TABLES> m_namedTables;
        std::array<uint8_t, Cst::PPU_PALETTE_SIZE> m_paletteTable;

        uint8_t m_screen[256][240];
        bool m_isFrameComplete = false;
    };
}