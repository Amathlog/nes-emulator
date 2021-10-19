#pragma once

#include <cstdint>

namespace NesEmulator
{
    class IMapper
    {
    public:
        IMapper(uint8_t nbPrgBanks, uint8_t nbChrBanks)
            : m_nbPrgBanks(nbPrgBanks)
            , m_nbChrBanks(nbChrBanks)
        {}

        virtual ~IMapper() = default;

        virtual bool MapReadCPU(uint16_t address, uint32_t& mappedAddress) = 0;
        virtual bool MapWriteCPU(uint16_t address, uint32_t& mappedAddress) = 0;
        virtual bool MapReadPPU(uint16_t address, uint32_t& mappedAddress) = 0;
        virtual bool MapWritePPU(uint16_t address, uint32_t& mappedAddress) = 0;

    protected:
        uint8_t m_nbPrgBanks;
        uint8_t m_nbChrBanks;
    };
}