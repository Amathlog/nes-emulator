#pragma once

#include <new_exe/messageService/message.h>
#include <cstdint>

namespace NesEmulatorGL
{
    using ScreenMessageType = uint32_t;

    enum DefaultScreenMessageType : ScreenMessageType
    {
        CHANGE_FORMAT = 0,
        RESIZE,
        GET_FORMAT,
        RENDER
    };

    enum class Format : unsigned
    {
        STRETCH = 0,
        ORIGINAL = 1,
        FOUR_THIRD = 2,

        COUNT = 3,
        UNDEFINED = 0xFFFFFFFF
    };

    class ScreenPayload : public Payload
    {
    public:
        ScreenPayload(ScreenMessageType type, Format format, int width, int height)
            : m_type(type)
            , m_format(format)
            , m_width(width)
            , m_height(height)
        {}

        ScreenPayload(ScreenMessageType type, const uint8_t* data, size_t size)
            : m_type(type)
            , m_screenData(data)
            , m_screenDataSize(size)
        {}

        ScreenMessageType m_type;
        Format m_format = Format::UNDEFINED;
        int m_width = 0;
        int m_height = 0;
        const uint8_t* m_screenData = nullptr;
        size_t m_screenDataSize = 0;
    };
}