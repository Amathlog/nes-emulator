#include <QApplication>
#include <QPushButton>

#include <core/utils/fileVisitor.h>
#include <core/utils/memoryVisitor.h>
#include <core/cartridge.h>
#include <core/utils/disassembly.h>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    // Load a rom from a file
    auto dir = fs::weakly_canonical(fs::path(argv[0])).parent_path();
    NesEmulator::Utils::FileReadVisitor visitor(dir / ".." / ".." / ".." / "tests" / "test_roms" / "nestest.nes");

    NesEmulator::Cartridge cartridge(visitor);

    NesEmulator::Utils::MemoryReadVisitor prgData(cartridge.GetPrgData().data(), cartridge.GetPrgData().size());

    NesEmulator::Utils::Disassemble(prgData, 0, 0x57);

    // QApplication app (argc, argv);

    // QPushButton button ("Hello world !");
    // button.show();

    // return app.exec();

    return 0;
}