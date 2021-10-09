#include <core/utils/visitor.h>

namespace NesEmulator
{
namespace Utils
{

class MemoryReadVisitor : public Visitor
{
public:
    MemoryReadVisitor(uint8_t* data, std::size_t size);

    void Read(uint8_t* data, std::size_t size) override;
    std::size_t Remaining() override;
};

}
}