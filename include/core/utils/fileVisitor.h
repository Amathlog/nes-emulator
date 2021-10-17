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

    void Read(uint8_t* data, std::size_t size) override;
    std::size_t Remaining() const override;

private:
    std::ifstream m_file;
    std::size_t m_ptr;
    std::size_t m_size;
};


class FileWriteVisitor : public IWriteVisitor
{
public:
    FileWriteVisitor(const std::string& file);
    ~FileWriteVisitor();

    void Write(const uint8_t* data, std::size_t size) override;
    std::size_t Written() const override;

private:
    std::ofstream m_file;
    std::size_t m_ptr;
};


}
}