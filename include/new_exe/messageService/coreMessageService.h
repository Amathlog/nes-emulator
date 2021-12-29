#pragma once

#include <new_exe/messageService/messageService.h>
#include <vector>
#include <string>

namespace NesEmulator 
{
    class Bus;
}

namespace NesEmulatorGL
{
    class CoreMessageService : public IMessageService
    {
    public:
        CoreMessageService(NesEmulator::Bus& bus, std::string exePath) 
            : m_bus(bus)
            , m_exePath(exePath)
        {}

        bool Push(const Message& message) override;
        bool Pull(Message& message) override { return true; }

    private:
        // Handlers
        bool LoadNewGame(const std::string& file);
        bool LoadSaveGame(const std::string& file);
        bool SaveGame(const std::string& file);
        bool SaveState(const std::string& file, int number);
        bool LoadState(const std::string& file, int number);

        NesEmulator::Bus& m_bus;
        std::string m_exePath;

        // Temporary
        std::vector<uint8_t> m_stateData;
    };
}