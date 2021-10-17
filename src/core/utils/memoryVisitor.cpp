#include <core/utils/memoryVisitor.h>
#include <cstring>
#include <cassert>

using NesEmulator::Utils::MemoryReadVisitor;
using NesEmulator::Utils::MemoryWriteVisitor;

MemoryReadVisitor::MemoryReadVisitor(const uint8_t* data, std::size_t size)
    : m_data(data)
    , m_size(size)
    , m_ptr(0)
{

}

void MemoryReadVisitor::Read(uint8_t* data, std::size_t size)
{
    assert(size <= Remaining());
    memcpy(data, m_data + m_ptr, size);
    m_ptr += size;
}

std::size_t MemoryReadVisitor::Remaining() const
{
    return m_size - m_ptr;
}

MemoryWriteVisitor::MemoryWriteVisitor(uint8_t* data, std::size_t size)
    : m_data(data)
    , m_size(size)
    , m_ptr(0)
{

}

void MemoryWriteVisitor::Write(const uint8_t* data, std::size_t size)
{
    assert(size <= (m_size - m_ptr));
    memcpy(m_data + m_ptr, data, size);
    m_ptr += size;
}

std::size_t MemoryWriteVisitor::Written() const
{
    return m_ptr;
}