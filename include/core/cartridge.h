#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <core/mapper.h>

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor;
    }

    class Cartridge
    {
    public:
        Cartridge(Utils::IReadVisitor& visitor);
        ~Cartridge();

        const std::vector<uint8_t>& GetPrgData() const { return m_prgData; }
        const std::vector<uint8_t>& GetChrData() const { return m_chrData; }

        bool WriteCPU(uint16_t addr, uint8_t data);
        bool ReadCPU(uint16_t addr, uint8_t& data);

        bool WritePPU(uint16_t addr, uint8_t data);
        bool ReadPPU(uint16_t addr, uint8_t& data);

        Mirroring GetMirroring() const { return m_mapper->GetMirroring(); }
        void Reset();
        IMapper* GetMapper() { return m_mapper.get(); }

        const std::string& GetSHA1() const { return m_sha1; }

    private:
        std::vector<uint8_t> m_prgData;
        std::vector<uint8_t> m_chrData;
        std::vector<uint8_t> m_prgRam;

        std::string m_sha1;
        
        std::unique_ptr<IMapper> m_mapper;
        uint16_t m_nbPrgBanks = 0;
        uint16_t m_nbChrBanks = 0;
    };
}