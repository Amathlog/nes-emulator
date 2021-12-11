#pragma once

#include "core/cartridge.h"
#include "core/ines.h"
#include <memory>
#include <core/mappers/mapper_000.h>
#include <core/mappers/mapper_001.h>
#include <core/mappers/mapper_002.h>
#include <core/mappers/mapper_003.h>
#include <core/mappers/mapper_004.h>
#include <core/mappers/mapper_066.h>

namespace NesEmulator
{
    inline std::unique_ptr<IMapper> CreateMapper(const iNESHeader& header)
    {
        switch(header.GetMapperId())
        {
        case 0:
            return std::make_unique<Mapper_000>(header);
        case 1:
            return std::make_unique<Mapper_001>(header);        
        case 2:
            return std::make_unique<Mapper_002>(header);        
        case 3:
            return std::make_unique<Mapper_003>(header);
        case 4:
            return std::make_unique<Mapper_004>(header);
        case 66:
            return std::make_unique<Mapper_066>(header);
        default:
            return nullptr;
        }
    }
}