#pragma once

#include <cstdint>
#include <array>

namespace NesEmulator 
{

namespace Registers
{

// 0x2000
union PPUCTRL
{
    // Ctrl part
    struct
    {
        uint8_t msbXScroll: 1;
        uint8_t msbYScroll: 1;
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

struct OAM
{
    uint8_t y;
    uint8_t tileId;
    uint8_t attribute;
    uint8_t x;

    void Reset()
    {
        y = tileId = attribute = x = 0xFF;
    }
};

constexpr int oamSize = 64;

struct PPURegisters
{
    Registers::PPUCTRL ctrl;        // 0x2000
    Registers::PPUMASK mask;        // 0x2001
    Registers::PPUSTATUS status;    // 0x2002
    uint8_t oamaddr;                // 0x2003
    OAM oam[oamSize];               // 0x2004
    uint8_t addr;                   // 0x2006
    uint8_t data;                   // 0x2007

    Registers::LoopyRegister vramAddr;
    Registers::LoopyRegister tramAddr;

    uint8_t fineX;

    // For rendering (background)
    uint8_t bgNextTileId = 0x00;
    uint8_t bgNextTileAttr = 0x00;
    uint8_t bgNextTileLsb = 0x00;
    uint8_t bgNextTileMsb = 0x00;
    // Shift registers (background)
    uint16_t bgShifterPatternLsb = 0x0000;
    uint16_t bgShifterPatternMsb = 0x0000;
    uint16_t bgShifterAttrLsb = 0x0000;
    uint16_t bgShifterAttrMsb = 0x0000;
    // Shift registers (foreground)
    std::array<uint8_t, 8> fgShifterPatternLsb;
    std::array<uint8_t, 8> fgShifterPatternMsb;

    // Sprite zero hit detection
    bool spriteZeroIsPossible;
    bool spriteZeroIsRendered;

    void Reset()
    {
        ctrl.flags = 0;
        mask.flags = 0;
        status.flags = 0;
        oamaddr = 0;
        for (int i = 0; i < oamSize; ++i)
            oam[i].Reset();

        addr = 0;
        data = 0;
        vramAddr.reg = 0;
        tramAddr.reg = 0;
        fineX = 0;

        bgNextTileId = 0;
        bgNextTileAttr = 0;
        bgNextTileLsb = 0;
        bgNextTileMsb = 0;
        bgShifterAttrLsb = 0;
        bgShifterAttrMsb = 0;
        bgShifterPatternLsb = 0;
        bgShifterPatternMsb = 0;

        fgShifterPatternLsb.fill(0x00);
        fgShifterPatternMsb.fill(0x00);

        spriteZeroIsPossible = false;
        spriteZeroIsRendered = false;
    }

    uint8_t* GetOAM() { return (uint8_t*)oam; }
};
}