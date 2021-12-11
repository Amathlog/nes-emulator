#pragma once

#include "core/ines.h"
#include <cstdint>

namespace NesEmulator
{
    enum class Mirroring
    {
        VERTICAL,
        HORIZONTAL,
    };

    class IMapper
    {
    public:
        IMapper(const iNESHeader& header)
            : m_header(header)
            , m_nbPrgBanks(m_header.GetPRGROMSize())
            , m_nbChrBanks(m_header.GetCHRROMSize())
            , m_originalMirroring((m_header.flag6.mirroring) ? Mirroring::VERTICAL : Mirroring::HORIZONTAL)
            , m_mirroring(m_originalMirroring)
        {}

        virtual ~IMapper() = default;

        virtual bool MapReadCPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) = 0;
        virtual bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) = 0;
        virtual bool MapReadPPU(uint16_t address, uint32_t& mappedAddress, uint8_t& data) = 0;
        virtual bool MapWritePPU(uint16_t address, uint32_t& mappedAddress, uint8_t data) = 0;
        virtual Mirroring GetMirroring() const { return m_mirroring; }

        virtual void Reset() 
        {
            m_mirroring = m_originalMirroring;
        }

    protected:
        iNESHeader m_header;
        uint16_t m_nbPrgBanks;
        uint16_t m_nbChrBanks;
        Mirroring m_originalMirroring;
        Mirroring m_mirroring;
    };
}