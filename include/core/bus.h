#pragma once

#include <core/utils/visitor.h>
#include <core/serializable.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <array>
#include <core/processor6502.h>
#include <core/processor2C02.h>
#include <core/processor2A03.h>
#include <core/controller.h>

namespace NesEmulator
{      
    class Cartridge;

    class Bus : public ISerializable
    {
    public:
        Bus();
        ~Bus() = default;

        void WriteCPU(uint16_t address, uint8_t data);
        uint8_t ReadCPU(uint16_t address);

        Processor6502& GetCPU() { return m_cpu; }
        Processor2C02& GetPPU() { return m_ppu; }
        const std::vector<uint8_t> GetCPURAM() { return m_cpuRam; }

        void InsertCartridge(const std::shared_ptr<Cartridge>& cartridge);
        void ConnectController(const std::shared_ptr<Controller>& controller, uint8_t controllerIndex);
        void DisconnectController(uint8_t controllerIndex);

        void Clock();
        void Reset();
        void Verbose();

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

    private:
        Processor6502 m_cpu;
        Processor2C02 m_ppu;
        Processor2A03 m_apu;

        std::vector<uint8_t> m_cpuRam;
        std::shared_ptr<Cartridge> m_cartridge;

        size_t m_clockCounter = 0;

        std::array<std::shared_ptr<Controller>, 2> m_controllers;
        std::array<uint8_t, 2> m_controllersState;

        // DMA specific
        uint8_t m_dmaPage = 0x00;
        uint8_t m_dmaAddr = 0x00;
        uint8_t m_dmaData = 0x00;

        bool m_dmaTransfer = false;
        bool m_dmaWaitForCPU = true;
    };
}