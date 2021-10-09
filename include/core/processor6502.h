#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace NesEmulator
{
    class Bus;
    
    // To store the flags of the processor
    union Status
    {
        struct
        {
            uint8_t C : 1;
            uint8_t Z : 1;
            uint8_t I : 1;
            uint8_t D : 1;
            uint8_t B : 1;
            uint8_t U : 1;
            uint8_t V : 1;
            uint8_t N : 1;
        };
        uint8_t flags = 0x00;
    };


    class Processor6502
    {
    public:
        Processor6502();
        ~Processor6502() = default;

        void ConnectBus(Bus* bus) { m_bus = bus; }

        const Status& GetStatus() const { return m_status; }

        // Addressing modes
        uint8_t IMP(); uint8_t IMM(); uint8_t ZP0(); uint8_t ZPX();
        uint8_t ZPY(); uint8_t REL(); uint8_t ABS(); uint8_t ABX();
        uint8_t ABY(); uint8_t IND(); uint8_t IZX(); uint8_t IZY();

        // Op codes
        uint8_t ADC(); uint8_t AND(); uint8_t ASL(); uint8_t BCC();
        uint8_t BCS(); uint8_t BEQ(); uint8_t BIT(); uint8_t BMI();
        uint8_t BNE(); uint8_t BPL(); uint8_t BRK(); uint8_t BVC();
        uint8_t BVS(); uint8_t CLC(); uint8_t CLD(); uint8_t CLI();
        uint8_t CLV(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
        uint8_t DEC(); uint8_t DEX(); uint8_t DEY(); uint8_t EOR();
        uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t JMP();
        uint8_t JSR(); uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
        uint8_t LSR(); uint8_t NOP(); uint8_t ORA(); uint8_t PHA();
        uint8_t PHP(); uint8_t PLA(); uint8_t PLP(); uint8_t ROL();
        uint8_t ROR(); uint8_t RTI(); uint8_t RTS(); uint8_t SBC();
        uint8_t SEC(); uint8_t SED(); uint8_t SEI(); uint8_t STA();
        uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY();
        uint8_t TSX(); uint8_t TXA(); uint8_t TXS(); uint8_t TYA();
        
        // Illegal instruction
        uint8_t XXX();

        void Clock();
        void Reset();
        void IRQ();
        void NMI();

        uint8_t Fetch();

    private:
        void ResetFlags() { m_status.flags = 0; }
        void Write(uint16_t address, uint8_t data);
        uint8_t Read(uint16_t address);

        // Bus pointer
        Bus* m_bus = nullptr;

        // Registers
        uint8_t m_A = 0x00;
        uint8_t m_X = 0x00;
        uint8_t m_Y = 0x00;

        uint8_t m_SP = 0x00;
        uint16_t m_PC = 0x0000;

        // Addresses
        uint16_t m_absAddress = 0x0000;
        uint16_t m_relAddress = 0x0000;

        // Helpers
        uint8_t m_fetched = 0x00;
        uint8_t m_opcode = 0x00;
        uint8_t m_cycles = 0x00;

        Status m_status;

        // Mapping between opcodes and functions
        struct Instruction
        {
            std::string name;
            uint8_t(Processor6502::*operate)(void) = nullptr;
            uint8_t(Processor6502::*addrmode)(void) = nullptr;
            uint8_t cycles = 0x00;
        };

        std::vector<Instruction> m_opCodeMapper;
    };
}