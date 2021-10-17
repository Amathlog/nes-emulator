#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor;
    }

    class IMapper;

    class Cartridge
    {
    public:
        Cartridge(Utils::IReadVisitor& visitor);
        ~Cartridge();

        const std::vector<uint8_t>& GetPrgData() const { return m_prgData; }
        const std::vector<uint8_t>& GetChrData() const { return m_chrData; }

    private:
        std::vector<uint8_t> m_prgData;
        std::vector<uint8_t> m_chrData;
        std::unique_ptr<IMapper> m_mapper;
    };
}