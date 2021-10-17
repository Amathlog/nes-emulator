#pragma once

#include <cstdint>

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor;
        
        void Disassemble(IReadVisitor& visitor, std::size_t startOffset = 0, std::size_t stopOffset = 0);
    }
}