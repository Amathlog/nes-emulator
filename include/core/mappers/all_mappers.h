#pragma once

#include "core/cartridge.h"
#include <memory>
#include <core/mappers/mapper_000.h>
#include <core/mappers/mapper_001.h>
#include <core/mappers/mapper_002.h>
#include <core/mappers/mapper_003.h>
#include <core/mappers/mapper_066.h>

namespace NesEmulator
{
    inline std::unique_ptr<IMapper> CreateMapper(uint8_t mapperId, uint8_t nbPrgBanks, uint8_t nbChrBanks, Mirroring initialMirroring)
    {
        switch(mapperId)
        {
        case 0:
            return std::make_unique<Mapper_000>(nbPrgBanks, nbChrBanks, initialMirroring);
        case 1:
            return std::make_unique<Mapper_001>(nbPrgBanks, nbChrBanks, initialMirroring);        
        case 2:
            return std::make_unique<Mapper_002>(nbPrgBanks, nbChrBanks, initialMirroring);        
        case 3:
            return std::make_unique<Mapper_003>(nbPrgBanks, nbChrBanks, initialMirroring);
        case 66:
            return std::make_unique<Mapper_066>(nbPrgBanks, nbChrBanks, initialMirroring);
        default:
            return nullptr;
        }
    }
}