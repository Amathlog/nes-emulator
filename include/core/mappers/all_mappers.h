#pragma once

#include <memory>
#include <core/mappers/mapper_000.h>

namespace NesEmulator
{
    std::unique_ptr<IMapper> CreateMapper(uint8_t mapperId, uint8_t nbPrgBanks, uint8_t nbChrBanks)
    {
        switch(mapperId)
        {
        case 0:
            return std::make_unique<Mapper_000>(nbPrgBanks, nbChrBanks);
        default:
            return nullptr;
        }
    }
}