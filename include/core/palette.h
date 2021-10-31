#pragma once
#include <array>
#include <cstdint>

namespace NesEmulator
{
    struct Palette
    {
        static constexpr std::array<uint8_t, 3> GetColor(int idx)
        {
            idx %= paletteSize;
            return std::array<uint8_t, 3> { palette[3 * idx], palette[3 * idx + 1], palette[3 * idx + 2] };
        }

        static constexpr uint32_t GetRGBColor(int idx)
        {
            idx %= paletteSize;
            uint32_t alpha = 0xff;
            uint32_t r = palette[3 * idx];
            uint32_t g = palette[3 * idx + 1];
            uint32_t b = palette[3 * idx + 2];

            return (alpha << 24) | (r << 16) | (g << 8) | b;
        }

        static constexpr int paletteSize = 64;
        static constexpr uint8_t palette[paletteSize * 3] = {
            84, 84, 84, 0, 30, 116, 8, 16, 144, 48, 0, 136, 68, 0, 100, 92, 0, 48, 84, 4, 0, 60, 24, 0, 32, 42, 0, 8, 58, 0, 0, 64, 0, 0, 60, 0, 0, 50, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            152, 150, 152, 8, 76, 196, 48, 50, 236, 92, 30, 228, 136, 20, 176, 160, 20, 100, 152, 34, 32, 120, 60, 0, 84, 90, 0, 40, 114, 0, 8, 124, 0, 0, 118, 40, 0, 102, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            236, 238, 236, 76, 154, 236, 120, 124, 236, 176, 98, 236, 228, 84, 236, 236, 88, 180, 236, 106, 100, 212, 136, 32, 160, 170, 0, 116, 196, 0, 76, 208, 32, 56, 204, 108, 56, 180, 204, 60, 60, 60, 0, 0, 0, 0, 0, 0,
            236, 238, 236, 168, 204, 236, 188, 188, 236, 212, 178, 236, 236, 174, 236, 236, 174, 212, 236, 180, 176, 228, 196, 144, 204, 210, 120, 180, 222, 120, 168, 226, 144, 152, 226, 180, 160, 214, 228, 160, 162, 160, 0, 0, 0, 0, 0, 0,
        };
    };
}