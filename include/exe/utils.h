#pragma once

#include <QImage>
#include <core/palette.h>
#include <cstdint>

namespace NesEmulatorExe 
{
    inline void CreatePalette(QImage* image)
    {
        image->setColorCount(NesEmulator::Palette::paletteSize);

        for (uint8_t i = 0; i < NesEmulator::Palette::paletteSize; ++i)
        {
            image->setColor(i, NesEmulator::Palette::GetRGBColor(i));
        }
    }
}
