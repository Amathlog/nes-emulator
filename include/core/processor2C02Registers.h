#pragma once

#include <cstdint>

namespace NesEmulator 
{

namespace Registers
{

// 0x2000
union PPUCTRL
{
    // Scroll part (MSB = Most Significant Bit)
    struct
    {
        uint8_t msbXScroll: 1;
        uint8_t msbYScroll: 1;
        uint8_t unused: 6;
    };

    // Ctrl part
    struct
    {
        uint8_t baseNametableAddress: 2;
        uint8_t VRAMAddressIncrement: 1;
        uint8_t spritePatternTableAddress: 1;
        uint8_t backgroundPatternTableAddress: 1;
        uint8_t spriteSize: 1;
        uint8_t PPUMasterSlaveSelect: 1;
        uint8_t generateNMIWhenVBI: 1;
    };

    uint8_t flags = 0x00;
};

// 0x2001
union PPUMASK
{
    struct
    {
        uint8_t greyscale: 1;
        uint8_t showBackgroundLeftmost: 1;
        uint8_t showSpritesLeftmost: 1;
        uint8_t showBackground: 1;
        uint8_t showSprites: 1;
        uint8_t emphasizeRed: 1; // Green on PAL/Dendy
        uint8_t emphasizeGreen: 1; // Red on PAL/Dendy
        uint8_t emphasizeBlue: 1;
    };

    uint8_t flags = 0x00;
};

// 0x2002
union PPUSTATUS
{
    struct
    {
        uint8_t lsbWrittenIntoPPU: 5;
        uint8_t spriteOverflow: 1;
        uint8_t sprite0Hit: 1;
        uint8_t verticalBlankStarted: 1;
    };

    uint8_t flags = 0x00;
};

union LoopyRegister
{
    struct
    {
        uint16_t coarseX: 5;
        uint16_t coarseY: 5;
        uint16_t nametableX: 1;
        uint16_t nametableY: 1;
        uint16_t fineY: 3;
        uint16_t ununsed: 1;
    };

    uint16_t reg;
};
}

struct PPURegisters
{
    Registers::PPUCTRL ctrl;        // 0x2000
    Registers::PPUMASK mask;        // 0x2001
    Registers::PPUSTATUS status;    // 0x2002
    uint8_t oamaddr;                // 0x2003
    uint8_t oamdata;                // 0x2004
    uint8_t scroll;                 // 0x2005
    uint8_t addr;                   // 0x2006
    uint8_t data;                   // 0x2007

    Registers::LoopyRegister vram_addr;
    Registers::LoopyRegister tram_addr;

    uint8_t fineX;

    void Reset()
    {
        ctrl.flags = 0;
        mask.flags = 0;
        status.flags = 0;
        oamaddr = 0;
        oamdata = 0;
        scroll = 0;
        addr = 0;
        data = 0;
        vram_addr.reg = 0;
        tram_addr.reg = 0;
        fineX = 0;
    }
};

}