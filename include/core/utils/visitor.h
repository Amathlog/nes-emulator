#pragma once

#include <cstdint>
#include <cstddef>

using std::size_t;

namespace NesEmulator
{
    namespace Utils
    {
        class IReadVisitor
        {
        public:
            virtual ~IReadVisitor() = default;

            virtual void Read(uint8_t* data, size_t size) = 0;
            virtual void Peek(uint8_t* data, size_t size) = 0;

            template<typename T>
            void Read(T* data, size_t size)
            {
                Read(reinterpret_cast<uint8_t*>(data), size * sizeof(T));
            }

            template<typename T>
            void Peek(T* data, size_t size)
            {
                Peek(reinterpret_cast<uint8_t*>(data), size * sizeof(T));
            }

            template<typename T>
            void ReadAll(T* data)
            {
                Read(data, Remaining() / sizeof(T));
            }

            template<typename T>
            void PeekAll(T* data)
            {
                Read(data, Remaining() / sizeof(T));
            }

            virtual void Advance(size_t size) = 0;
            
            virtual size_t Remaining() const = 0;
        };

        class IWriteVisitor
        {
        public:
            virtual ~IWriteVisitor() = default;

            virtual void Write(const uint8_t* data, size_t size) = 0;

            template <typename T>
            void Write(const T* data, size_t size)
            {
                Write(reinterpret_cast<const uint8_t*>(data), size / sizeof(T));
            }

            template <typename Container>
            void WriteAll(const Container& data)
            {
                Write(data.data(), data.size());
            }

            virtual size_t Written() const = 0;
        };
    }
}