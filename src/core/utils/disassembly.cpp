#include <core/utils/disassembly.h>
#include <core/utils/visitor.h>
#include <core/processor6502.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

using NesEmulator::Utils::IReadVisitor;
using NesEmulator::Processor6502;

template <typename Stream>
void Hex(Stream& s, uint16_t v, uint8_t n, const char* prefix="", const char* suffix="")
{
    s << std::internal << std::setfill('0');
    s << prefix << std::hex << std::uppercase << std::setw(n) << (int)v << suffix;
}

void NesEmulator::Utils::Disassemble(IReadVisitor& visitor, std::size_t startOffset /* = 0 */, std::size_t stopOffset /* = 0 */)
{
    std::stringstream res;
    //auto& res = std::cout;
    // Dummy cpu
    Processor6502 dummyCPU;
    auto opCodeMapper = dummyCPU.GetOpCodeMapper();

    assert(visitor.Remaining() >= startOffset + stopOffset);
    visitor.Advance(startOffset);

    std::size_t stopRemaining = stopOffset != 0 ? visitor.Remaining() - stopOffset : 0;

    while (visitor.Remaining() >= stopRemaining)
    {
        // First read the first byte, it's the opcode
        uint8_t opCode;
        visitor.Read(&opCode, 1);

        const Processor6502::Instruction& instruction = opCodeMapper[opCode];

        res << instruction.name << " ";

        uint16_t operand = 0;

        // Depending on the addressing mode, we need to read 0, 1 or 2 operands
        if (instruction.addrmode == &Processor6502::IMP)
        {
            // Nothing to read
        }
        else if (instruction.addrmode == &Processor6502::ABS ||
                 instruction.addrmode == &Processor6502::ABX ||
                 instruction.addrmode == &Processor6502::ABY ||
                 instruction.addrmode == &Processor6502::IND
                )
        {
            // 2 bytes to read, it's an address
            visitor.Read(&operand, 1);
        }
        else
        {
            // 1 byte to read
            uint8_t data;
            visitor.Read(&data, 1);
            operand = data;
        }

        // Depending on the addr mode, we output something different
        if (instruction.addrmode == &Processor6502::IMP)
        {
            // Nothing to do
        }
        else if (instruction.addrmode == &Processor6502::IMM)
        {
            Hex(res, operand, 2, "#");
        }
        else if (instruction.addrmode == &Processor6502::ZP0)
        {
            Hex(res, operand, 2, "$");
        }
        else if (instruction.addrmode == &Processor6502::ZPX)
        {
            Hex(res, operand, 2, "$", ",X");
        }
        else if (instruction.addrmode == &Processor6502::ZPY)
        {
            Hex(res, operand, 2, "$", ",Y");
        }
        else if (instruction.addrmode == &Processor6502::REL)
        {
            Hex(res, operand, 2, "PC,$");
        }
        else if (instruction.addrmode == &Processor6502::ABS)
        {
            Hex(res, operand, 4, "$");
        }
        else if (instruction.addrmode == &Processor6502::ABX)
        {
            Hex(res, operand, 4, "$", ",X");
        }
        else if (instruction.addrmode == &Processor6502::ABY)
        {
            Hex(res, operand, 4, "$", ",Y");
        }
        else if (instruction.addrmode == &Processor6502::IND)
        {
            Hex(res, operand, 4, "($", ")");
        }
        else if (instruction.addrmode == &Processor6502::IZX)
        {
            Hex(res, operand, 2, "($", ",X)");
        }
        else if (instruction.addrmode == &Processor6502::IZY)
        {
            Hex(res, operand, 2, "($", "),Y");           
        }

        res << std::endl;
    }

    std::cout << res.str();
}