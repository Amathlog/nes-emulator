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
        CoreMessageService(NesEmulator::Bus& bus) : m_bus(bus) {}

        bool Push(const Message& message) override;
        bool Pull(Message& message) override { return true; }

    private:
        // Handlers
        bool LoadNewGame(const std::string& file);
        bool SaveState();
        bool LoadState();

        NesEmulator::Bus& m_bus;

        // Temporary
        std::vector<uint8_t> m_stateData;
    };
}