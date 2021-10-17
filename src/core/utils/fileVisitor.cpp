#include <core/utils/fileVisitor.h>
#include <cstring>
#include <cassert>
#include <limits>

using NesEmulator::Utils::FileReadVisitor;
using NesEmulator::Utils::FileWriteVisitor;

template <typename T>
void Open(T& stream, const std::string& filename)
{
    stream.open(filename, ios::binary);
}

template <typename T>
void Close(T& stream)
{
    stream.close();
}

FileReadVisitor::FileReadVisitor(const std::string& file)
    : m_ptr(0)
{
    Open(m_file, file);

    m_size = 0;

    // Gets its size
    if (m_file.is_open())
    {
        m_file.ignore( std::numeric_limits<std::streamsize>::max() );
        m_size = file.gcount();
        m_file.clear();   //  Since ignore will have set eof.
        m_file.seekg( 0, std::ios_base::beg );
    }
}

FileReadVisitor::~FileReadVisitor()
{
    Close(m_file);
}

void FileReadVisitor::Read(uint8_t* data, std::size_t size)
{
    assert(size <= Remaining());
    m_file.read(data, size);
    m_ptr += size;
}

std::size_t FileReadVisitor::Remaining() const
{
    return m_size - m_ptr;
}

FileWriteVisitor::FileWriteVisitor(const std::string& file)
    : m_ptr(0)
{
    Open(file);
}

FileWriteVisitor::~FileWriteVisitor()
{
    Close(m_file);
}

void FileWriteVisitor::Write(const uint8_t* data, std::size_t size)
{
    assert(m_file.is_open());
    m_file.write(data, size);
    m_ptr += size;
}

std::size_t FileWriteVisitor::Written() const
{
    return m_ptr;
}