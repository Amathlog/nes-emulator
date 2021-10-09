#pragma once

#include <cstdint>

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor
        {
        public:
            virtual void Read(uint8_t* data, std::size_t size) = 0;

            template<typename T>
            void Read(T* data, std::size_t size)
            {
                Read(reinterpret_cast<uint8_t*>(data), size * sizeof(T));
            }

            template<typename T>
            void ReadAll(T* data)
            {
                Read(T, Remaining() / sizeof(T));
            }
            
            virtual std::size_t Remaining() const = 0;
        };

        class IWriteVisitor
        {
        public:
            virtual void Write(uint8_t* data, std::size_t size) = 0;

            virtual std::size_t Written() const = 0;
        };
    }
}