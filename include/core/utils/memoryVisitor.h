#include <core/utils/visitor.h>

namespace NesEmulator
{
namespace Utils
{

class MemoryReadVisitor : public IReadVisitor
{
public:
    MemoryReadVisitor(const uint8_t* data, std::size_t size);

    void Read(uint8_t* data, std::size_t size) override;
    void Peek(uint8_t* data, std::size_t size) override;
    std::size_t Remaining() const override;
    void Advance(std::size_t size) override;

private:
    const uint8_t* m_data;
    std::size_t m_ptr;
    std::size_t m_size;
};


class MemoryWriteVisitor : public IWriteVisitor
{
public:
    MemoryWriteVisitor(uint8_t* data, std::size_t size);

    void Write(const uint8_t* data, std::size_t size) override;
    std::size_t Written() const override;

private:
    uint8_t* m_data;
    std::size_t m_ptr;
    std::size_t m_size;
};

}
}