#include <core/utils/visitor.h>
#include <string>
#include <fstream>

namespace NesEmulator
{
namespace Utils
{

class FileReadVisitor : public IReadVisitor
{
public:
    FileReadVisitor(const std::string& file);
    ~FileReadVisitor();

    void Read(uint8_t* data, size_t size) override;
    void Peek(uint8_t* data, size_t size) override;
    size_t Remaining() const override;
    void Advance(size_t size) override;

private:
    std::ifstream m_file;
    size_t m_ptr;
    size_t m_size;
};


class FileWriteVisitor : public IWriteVisitor
{
public:
    FileWriteVisitor(const std::string& file);
    ~FileWriteVisitor();

    void Write(const uint8_t* data, size_t size) override;
    size_t Written() const override;

private:
    std::ofstream m_file;
    size_t m_ptr;
};


}
}