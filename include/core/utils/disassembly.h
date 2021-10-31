#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <core/utils/busVisitor.h>

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor;
        
        std::vector<std::string> Disassemble(BusReadVisitor& busVisitor, uint16_t wantedPC, uint16_t& indexOfWantedPC);
    }
}