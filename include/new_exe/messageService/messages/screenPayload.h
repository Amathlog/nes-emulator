#pragma once

#include <new_exe/messageService/message.h>

namespace NesEmulatorGL
{
    using ScreenMessageType = uint32_t;

    enum DefaultScreenMessageType : ScreenMessageType
    {
        CHANGE_FORMAT,
        RESIZE,
        GET_FORMAT,
    };

    enum Format : unsigned
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

        ScreenMessageType m_type;
        Format m_format;
        int m_width;
        int m_height;
    };
}