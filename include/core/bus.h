#pragma once

#include <core/constants.h>
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
#include <filesystem>
#include <MyTonic.h>

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

        // Clock will advance 1 PPU clock. Will return true if an audio sample is ready
        bool Clock();

        void Reset();
        void Verbose();

        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

        void SaveRAM(Utils::IWriteVisitor& visitor) const;
        void LoadRAM(Utils::IReadVisitor& visitor);

        std::filesystem::path GetSaveStateFile(std::filesystem::path exeDir, int number);
        std::filesystem::path GetSaveFile(std::filesystem::path exeDir);
        std::filesystem::path GetSaveFolder(std::filesystem::path exeDir);

        Tonic::Synth* GetSynth() { return m_apu.GetSynth(); }

        void SetMode(Mode mode);
        NesEmulator::Mode GetMode() const { return m_mode; }
        void SetSampleFrequency(unsigned sampleFrequency);

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

        Mode m_mode = Mode::NTSC;

        bool m_dmaTransfer = false;
        bool m_dmaWaitForCPU = true;
        
        double m_audioTime = 0.0;
        double m_audioTimePerSystemSample = 0.0;
        double m_audioTimePerPPUClock = 0.0;
    };
}