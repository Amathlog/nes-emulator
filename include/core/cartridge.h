#pragma once

#include <core/serializable.h>
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

    class Cartridge : public ISerializable
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
        bool HasPersistantMemory() const { return m_mapper->HasPersistantMemory(); }

        const std::string& GetSHA1() const { return m_sha1; }

        void SaveRAM(Utils::IWriteVisitor& visitor) const;
        void LoadRAM(Utils::IReadVisitor& visitor);
        void SerializeTo(Utils::IWriteVisitor& visitor) const override;
        void DeserializeFrom(Utils::IReadVisitor& visitor) override;

    private:
        std::vector<uint8_t> m_prgData;
        std::vector<uint8_t> m_chrData;
        std::vector<uint8_t> m_prgRam;
        std::vector<uint8_t> m_vRam;

        std::string m_sha1;
        
        std::unique_ptr<IMapper> m_mapper;
        uint16_t m_nbPrgBanks = 0;
        uint16_t m_nbChrBanks = 0;
        bool m_useVRam = false;
        Mapping m_mapping;
    };
}