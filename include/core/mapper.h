#pragma once

#include <core/constants.h>
#include <core/utils/visitor.h>
#include <core/ines.h>
#include <core/serializable.h>
#include <cstdint>
#include <vector>

namespace NesEmulator
{
    enum class Mirroring
    {
        VERTICAL,
        HORIZONTAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
        FOUR_SCREEN
    };

    class IMapper : public ISerializable
    {
    public:
        IMapper(const iNESHeader& header)
            : m_header(header)
            , m_nbPrgBanks(m_header.GetPRGROMSize())
            , m_nbChrBanks(m_header.GetCHRROMSize())
            , m_id(header.GetMapperId())
        {
            if (m_header.flag6.ignoreMirroringControl)
                m_originalMirroring = Mirroring::FOUR_SCREEN;
            else
                m_originalMirroring = (m_header.flag6.mirroring) ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;

            m_mirroring = m_originalMirroring;
        }

        virtual ~IMapper() = default;

        virtual bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) = 0;
        virtual bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) = 0;
        virtual bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) = 0;
        virtual bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) = 0;
        virtual Mirroring GetMirroring() const { return m_mirroring; }

        NesEmulator::Mode GetMode() const { return m_header.GetRegion(); }
        uint16_t GetMapperId() const { return m_id; }

        virtual void Reset() 
        {
            m_mirroring = m_originalMirroring;
        }

        // Functions specific for the PPU. Handle IRQ from the mapper. Used for mapper 004 for example.
        // It is not very accurate but will do the job nonetheless
        // By default, all those functions does nothing. Each mapper can handle this as they please

        // Called at the end of each scanline
        virtual void ScanlineDone() {}
        // Called at the end for each PPU cycle
        virtual void CPUClock() {}
        // Check if we should fire an IRQ
        virtual bool ShouldIRQ() const { return false; }
        // Clear the IRQ state
        virtual void ClearIRQ() {}

        bool HasPersistantMemory() const { return m_header.flag6.hasPersistentMemory; }

        void SaveRAM(Utils::IWriteVisitor& visitor) const
        {
            visitor.WriteContainer(m_staticRAM);
        }

        void LoadRAM(Utils::IReadVisitor& visitor)
        {
            visitor.ReadContainer(m_staticRAM);
        }

        void SerializeTo(Utils::IWriteVisitor& visitor) const override
        {
            visitor.WriteValue(m_mirroring);
            SaveRAM(visitor);
        }

        void DeserializeFrom(Utils::IReadVisitor& visitor) override
        {
            visitor.ReadValue(m_mirroring);
            LoadRAM(visitor);
        }

    protected:
        uint16_t m_id;
        iNESHeader m_header;
        uint16_t m_nbPrgBanks;
        uint16_t m_nbChrBanks;
        Mirroring m_originalMirroring;
        Mirroring m_mirroring;

        std::vector<uint8_t> m_staticRAM;
    };
}